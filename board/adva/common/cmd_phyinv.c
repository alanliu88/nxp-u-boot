/*
 * Copyright (C) ADVA Optical Networking, Inc. 2009
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <command.h>
#include <i2c.h>
#include <asm/global_data.h>

#include "phyinv.h"
#include "cpld.h"
#include "i2c_mux.h"

#if CONFIG_SYS_I2C_EEPROM_ADDR_LEN != 1 && defined(CONFIG_SYS_EEPROM_OFFSET)
#define BASE_OFFSET CONFIG_SYS_EEPROM_OFFSET
#else
#define BASE_OFFSET 0x0000
#endif

DECLARE_GLOBAL_DATA_PTR;

#define PROBE_I2C_EEPROM_ADDR -1

int phyinv_dev = 0;
int phyinv_addr = CONFIG_SYS_I2C_EEPROM_ADDR;

#define	PHYINV_EEPROM_PAGE_SIZE	(1 << CONFIG_SYS_EEPROM_PAGE_WRITE_BITS)
#define	PHYINV_EEPROM_PAGE_OFFSET(x)	((x) & (PHYINV_EEPROM_PAGE_SIZE - 1))
#define I2C_RXTX_LEN	128	/* maximum tx/rx buffer length */

extern int _computeChecksum (struct phyInv_t *phyInv, bool silent);

int __weak phyinv_protect(bool value)
{
	return value;
}

struct udevice* __weak get_i2c_inv_dev(int ext)
{
	struct udevice *devp;
	int ret = -1;
	uint addr = 0x54;

	if (ext)
		addr = 0x55;

	ret = i2c_get_chip_for_busnum(0, addr, 1, &devp);
	if (ret != 0) {
		printf("get inv dev failed 0:0x54,ret=%d\n", ret);
		return NULL;
	}

	return devp;
}

int
parseMac (char *s, struct phyInv_t *phyInv)
{
	int reg;
	char *e;
	for (reg = 0; reg < 6; ++reg) {
		/* turn string into mac value. u-boot orgin */
		phyInv->p0->basePhyAddr[reg] = s ? simple_strtoul (s, &e, 16) : 0;
		if (s)
			s = (*e) ? e + 1 : e;
	}

	return 0;
}

static int phyinv_eeprom_len(unsigned offset, unsigned end)
{
	unsigned len = end - offset;

	unsigned blk_off = offset & 0xff;
	unsigned maxlen = PHYINV_EEPROM_PAGE_SIZE - PHYINV_EEPROM_PAGE_OFFSET(blk_off);

	if (maxlen > I2C_RXTX_LEN)
		maxlen = I2C_RXTX_LEN;

	if (len > maxlen)
		len = maxlen;

	return len;
}

int phyinv_i2c_write(struct udevice* i2cdev, uint offset, uint8_t* buffer, int len)
{
	int ret = 0;
	unsigned end = offset + len;
	int wrlen = 0;
	
	while (offset < end) {
		wrlen = phyinv_eeprom_len(offset, end);

		ret = dm_i2c_write(i2cdev, offset, buffer, wrlen);

		buffer += wrlen;
		offset += wrlen;

		if (1)
			udelay(CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS * 1000);
	}
	
	return ret;
}

int phyinv_read(struct phyInv_t* phyInv)
{
	int ret = 1;
	uint offset = BASE_OFFSET;
	struct udevice* i2cdev;
	
	memset (phyInv->p0, 0, sizeof (PhyInv_t_page0));
	memset (phyInv->p1, 0, sizeof (PhyInv_t_page1));

	i2cdev = get_i2c_inv_dev(0);
	if (i2cdev == NULL) {
		printf("no valid inventory device\n");
		return 1;
	}

	ret = dm_i2c_read(i2cdev, offset,(u8*) phyInv->p0, sizeof(PhyInv_t_page0));
	if (ret != 0) {
		printf("page1 FAIL\n");
		return -1;
	}

	/*update page2 address and offset*/
#if CONFIG_SYS_I2C_EEPROM_ADDR_LEN == 1
	i2cdev = get_i2c_inv_dev(1);
	if (i2cdev == NULL) {
		puts("no valid inventory device\n");
		return 1;
	}
#else
	offset = BASE_OFFSET + sizeof (PhyInv_t_page1);
#endif
	
	/*read page 2*/
	ret = dm_i2c_read(i2cdev, offset, (u8*)phyInv->p1, sizeof(PhyInv_t_page1));
	if (ret == 0) {
		if (_computeChecksum (phyInv, true))
			return 1;
	}else {
		printf ("page2 FAIL\n");
		return 1;
	}

	return 0;
}

int phyinv_commit(struct phyInv_t* phyInv, bool silent)
{
	int ret = 0;
	uint offset = BASE_OFFSET;
	struct udevice* dev;
	    
	_computeChecksum (phyInv, silent);

	dev = get_i2c_inv_dev(0);
	if (dev == NULL) {
		printf("get i2c inv dev failed\n");
		return 0;
	}
	
	ret = phyinv_i2c_write (dev, offset, (uint8_t *) phyInv->p0, sizeof (PhyInv_t_page0));
	if (ret != 0) {
		puts ("commit page0 to EEPROM FAILED\nERROR\n");
		ret = 1;
	}

	/*update page2 address and offset*/
#if CONFIG_SYS_I2C_EEPROM_ADDR_LEN == 1
	dev = get_i2c_inv_dev(1);
	if (dev == NULL) {
		puts("no valid inventory device\n");
		return 1;
	}
#else
	offset = BASE_OFFSET + sizeof (PhyInv_t_page1);
#endif

	ret = phyinv_i2c_write (dev, offset, (uint8_t *) phyInv->p1, sizeof (PhyInv_t_page1));
	if (ret != 0) {
		printf("commit page1 to EEPROM FAILED, ret = %d\n", ret);
		ret = 1;
	}

	return ret;
}

int phyinv_fipsmode(int value)
{
	static struct phyInv_t phyInv;
	static struct phyInv_t_page0 phyInvPage0;
	static struct phyInv_t_page1 phyInvPage1;

	phyInv.p0 = &phyInvPage0;
	phyInv.p1 = &phyInvPage1;

	phyinv_protect(0);

	if (phyinv_read(&phyInv) != 0)
	{
		puts ("read fipsmode flag failed\n");
		return 1;
	}

	phyInv.p0->fipsMode = value;

	if (phyinv_commit(&phyInv, true) != 0)
	{
		puts ("save fipsmode flag failed\n");
		return 1;
	}

	phyinv_protect(1);

	return 0;
}

int
phyinv_update(struct phyInv_t* phyInv, char* token, char* val)
{
	return 0;
}


int
do_dumpPhyInv (struct cmd_tbl * cmdtp, int flag, int argc, char * const argv[])
{
	struct phyInv_t_page0 phyInvPage0;
	struct phyInv_t_page1 phyInvPage1;
	struct phyInv_t phyInv = { &phyInvPage0, &phyInvPage1 };

	if ((argc < 1) || (argc > 2)) {
		cmd_usage (cmdtp);
		return 1;
	}

	puts ("Reading EEPROM....\n");
	if (!phyinv_read(&phyInv))
		dumpPhyInv (&phyInv);

	return 0;
}

int
do_cmdPhyInv (struct cmd_tbl * cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	char str[64];
	static struct phyInv_t phyInv;
	static struct phyInv_t_page0 phyInvPage0;
	static struct phyInv_t_page1 phyInvPage1;

	phyInv.p0 = &phyInvPage0;
	phyInv.p1 = &phyInvPage1;

	if (argc == 1) {
		dumpPhyInv (&phyInv);
		return 0;
	}
	
	if (!strncmp (argv[1], "commit", 6)) {
		computeChecksum (&phyInv);
		return phyinv_commit(&phyInv, true);		
	}else if (!strncmp (argv[1], "rd", 2)) {
		puts ("Reading EEPROM \n");
		phyinv_read(&phyInv);
		dumpPhyInv (&phyInv);
	}else if (!strncmp (argv[1], "init", 4)) {
		memset (phyInv.p0, 0, sizeof (PhyInv_t_page0));
		memset (phyInv.p1, 0, sizeof (PhyInv_t_page1));
		puts ("Ram Image Cleared....\n");
	}else if (!strncmp (argv[1], "dump", 4)) {
		dumpPhyInvHex (&phyInv);
	}else if (!strncmp (argv[1], "wr", 2)) {
		if (argc < 4) {
			puts ("invalid wr argument\n");
			return 1; 
		}

		strcpy (str, argv[3]); /* min argv[4], if more that concatinate */	
		for (i = 4; i < argc; i++) {   
			/* concatinate the user strings into a single string */
			/* we need to allow spaces into certain eeprom fields */
			/* so concatinate the argv's */
			strcat (str, " ");
			strcat (str, argv[i]);
		}

		if (!strncmp (argv[2], "sn", 2)) {
			strncpy (phyInv.p0->serNum, str, sizeof (phyInv.p0->serNum));
		}else if (!strncmp (argv[2], "pn", 2)){
			strncpy (phyInv.p0->partNum, str, sizeof (phyInv.p0->partNum));
		}else if (!strncmp (argv[2], "fmtver", 7)){
			strncpy (phyInv.p0->fmtVer, str, sizeof (phyInv.p0->fmtVer));
		}else if (!strncmp (argv[2], "uname", 5)){
			strncpy (phyInv.p0->unitName, str, sizeof (phyInv.p0->unitName));
		}else if (!strncmp (argv[2], "clei", 4)){
			strncpy (phyInv.p0->cleiCode,str,sizeof (phyInv.p0->cleiCode));
		}else if (!strncmp (argv[2], "nummac", 6)){
			unsigned short int numMacs; 
			numMacs = (unsigned short int) simple_strtoul (argv[3], NULL, 10);
			phyInv.p0->numMacAddr[1] = numMacs & 0xFF;
			phyInv.p0->numMacAddr[0] = (numMacs & 0xFF00) >> 8;
		}else if (!strncmp (argv[2], "addr", 4)){
			if (parseMac (str, &phyInv)) {
				printf ("Error Parsing Mac Address %s\n", argv[3]);
				return 1;
			}
		}else if (!strncmp (argv[2], "hwver", 5)) {
			strncpy (phyInv.p0->hwVer, str, sizeof (phyInv.p0->hwVer));
		}else if (!strncmp (argv[2], "compid", 8)) {
			strncpy(phyInv.p0->compatId, str, sizeof (phyInv.p0->compatId));
		}else if (!strncmp (argv[2], "mfg", 9)){
			strncpy (phyInv.p0->mfg, str, sizeof (phyInv.p0->mfg));
		}else if (!strncmp (argv[2], "mfgdate", 9)) {
			strncpy (phyInv.p0->mfgDate, str, sizeof (phyInv.p0->mfgDate));
		}else if (!strncmp (argv[2], "mfgsite", 7)) {
			strncpy (phyInv.p0->mfgSite, str, sizeof (phyInv.p0->mfgSite));
		}else if (!strncmp (argv[2], "testsite", 8)) {
			strncpy (phyInv.p1->testSite, str, sizeof (phyInv.p1->testSite));
		}else if (!strncmp (argv[2], "testdate", 10)) {
			strncpy (phyInv.p1->testDate, str, sizeof (phyInv.p1->testDate));
		}else if (!strncmp (argv[2], "burnintestdate", 14)) {
			strncpy (phyInv.p1->burnInTestDate, str,sizeof (phyInv.p1->burnInTestDate));
		}else if (!strncmp (argv[2], "finaltestdate", 13)) {
			strncpy (phyInv.p1->finalTestDate, str,sizeof (phyInv.p1->finalTestDate));
		}else if (!strncmp (argv[2], "productiontestarea", 18)) {
			strncpy (phyInv.p1->productionTestArea, str,sizeof (phyInv.p1->productionTestArea));
		}else{
			puts ("invalid wr argument\n");
			return 1;                
		}
	
		puts ("ok\n");
		
		if (computeChecksum (&phyInv))
			puts ("checksum updated\n");
	}	else{
		printf ("ERROR command not given as 1st argument\n");
		return 1;
	}

	return 0;
}

U_BOOT_CMD (phyinv, 3, 1, do_dumpPhyInv,
        "displays the contents of physical inventory",
         " - displays physical inventory from EEPROM\n"
        );

U_BOOT_CMD (inv, 10, 1, do_cmdPhyInv,
        "phyical inventory utility",
        "                                 - displays the RAM image and shows field names\n"
        "inv init                             - clears ram EEPROM RAM image\n"
        "inv rd                               - reads the EEPROM to RAM image\n"
        "inv wr <field name> <value to write> - modifies the RAM image\n"
        "inv commit                           - writes the RAM image to EEPROM\n"
        "inv dev "  "  - show or select EEPROM device\n"
        );
