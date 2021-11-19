/*
 * Copyright (c) 2014
 * ADVA Optical Networking SE.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <i2c.h>
#include "console.h"
#include "cpld_burn.h"
#include "lptm21.h"


DECLARE_GLOBAL_DATA_PTR;

/* The CPLD-Register; */
#define CPLD_ID                        0x0000
#define CPLD_REV                       0x0004
#define CPLD_RSV_08                    0x0008
#define CPLD_RSV_0C                    0x000c
#define CPLD_TEST_REG                  0x0010
#define CPLD_RESET_REG0                0x0014
#define CPLD_RSV_18                    0x0018
#define CPLD_RSV_1C                    0x001c
#define CPLD_RSV_20                    0x0020
#define CPLD_INTSTAT0                  0x0024
#define CPLD_RSV_28                    0x0028
#define CPLD_INTSRC0                   0x002c
#define CPLD_RSV_30                    0x0030
#define CPLD_INTEN0                    0x0034
#define CPLD_RSV_38                    0x0038
#define CPLD_FPGA_PROG                 0x003c
#define CPLD_RSV_40                    0x0040
#define CPLD_SYS_STAT                  0x0044
#define CPLD_CONT_REG0                 0x0012
#define CPLD_CONT_REG1                 0x004c
#define CPLD_SCRATCH                   0x0050
#define CPLD_RES_CAUSE_REG             0x0054
#define CPLD_RES_TYPE_REG              0x0058
#define CPLD_RES_GEN_REG               0x005c
#define CPLD_MISC1_REG                 0x0090
#define CPLD_MISC2_REG                 0x0040
#define CPLD_SLOT_ID_REG               0x009c
#define CPLD_LED_CONTROL_VEC           0x1000
#define CPLD_LED_CONTROL_VEC_MAX_BYTE  0x0800
#define CPLD_LED_DIAG_VEC              0x1900
#define CPLD_LED_DIAG_VEC_MAX_BYTE     0x0100


/* Used bitmasks:*/
#define CPLD_BIT_WATCHDOG_DISABLE  0x02
#define CPLD_BIT_FLASH_UP_ENABLE   0x01
#define CPLD_BIT_CUR_FB            0x80  /* current flashbank */
#define CPLD_BIT_PROG_FB           0x40  /* programmed flashbank*/
#define CPLD_BIT_I2CINV_WP         0x01  /* toggle bit for writeprotection */

/* used in ufm-memory: */
#define TOGGLE_FLAG_ADDR     0
#define TOGGLE_FLAG_PAGE     1
#define TOGGLE_FLAG_PAGE_1  1//74

#define CEM_PCA9555_I2C0 0x22
#define CONFIG_ASC_I2C_EN 5

#ifdef ADVA_BOARD_XJ128
/*LCMMX03LF-2100C*/
#define LPTM21_MAX_CFG_PAGES           3198     /* max. CFG pages */
#define LPTM21_MAX_UFM_PAGES           639      /* max. UFM pages */
#define LPTM21_MAX_ASC_CFG_BYTES       120      /* max. ASC configuration EEPROM bytes */
#define LPTM21_MAX_ASC_BYTES           271      /* max. ASC memory bytes */
#define LPTM21_UFM_DATA                0xC7E0   /* start of UFM data in binary file */
#define LPTM21_UFM_DATA_mach           0xC7E0
#define LPTM21_ASC_DATA                0xEFD0   /* start of ASC data in binary file */
#define LPTM21_ROW_DATA                0x0000   /* start of FEATURE ROW data in binary file */
#else
#define LPTM21_MAX_CFG_PAGES           575     /* max. CFG pages */
#define LPTM21_MAX_UFM_PAGES           0      /* max. UFM pages */
#define LPTM21_MAX_ASC_CFG_BYTES       120      /* max. ASC configuration EEPROM bytes */
#define LPTM21_MAX_ASC_BYTES           271      /* max. ASC memory bytes */
#define LPTM21_UFM_DATA                0xC7E0   /* start of UFM data in binary file */
#define LPTM21_UFM_DATA_mach           0xC7E0
#define LPTM21_ASC_DATA                0xEFD0   /* start of ASC data in binary file */
#define LPTM21_ROW_DATA                0x0000   /* start of FEATURE ROW data in binary file */
#endif

/*LCMMX03LF-2100C*/
#define MB_LPTM21_MAX_CFG_PAGES           3198     /* max. CFG pages */
#define MB_LPTM21_MAX_UFM_PAGES           639      /* max. UFM pages */
#define MB_LPTM21_MAX_ASC_CFG_BYTES       120      /* max. ASC configuration EEPROM bytes */
#define MB_LPTM21_MAX_ASC_BYTES           271      /* max. ASC memory bytes */
#define MB_LPTM21_UFM_DATA                0xC7E0   /* start of UFM data in binary file */
#define MB_LPTM21_UFM_DATA_mach           0xC7E0
#define MB_LPTM21_ASC_DATA                0xEFD0   /* start of ASC data in binary file */
#define MB_LPTM21_ROW_DATA                0x0000   /* start of FEATURE ROW data in binary file */

/*LCMMX03LF-4300C-6BG400I*/
#define IO_LPTM21_MAX_CFG_PAGES           5758     /* max. CFG pages */
#define IO_LPTM21_MAX_UFM_PAGES           767      /* max. UFM pages */
#define IO_LPTM21_MAX_ASC_CFG_BYTES       120      /* max. ASC configuration EEPROM bytes */
#define IO_LPTM21_MAX_ASC_BYTES           271      /* max. ASC memory bytes */
#define IO_LPTM21_UFM_DATA                0x167E0  /* start of UFM data in binary file */
#define IO_LPTM21_UFM_DATA_mach           0x167E0
#define IO_LPTM21_ASC_DATA                0x197D0  /* start of ASC data in binary file */
#define IO_LPTM21_ROW_DATA                0x0000   /* start of FEATURE ROW data in binary file */

#define ADVA_CPLD_LOAD_OPTIMIZE
#define err(fmt,args...)	printf(fmt ,##args)
#define dbg(fmt,args...)	printf(fmt ,##args)

unsigned int gCpldImageAddr = DEFAULT_SRC_ADDR;

static uint32_t memory_compare(uint8_t *reference, uint8_t *probe, uint32_t num_bytes, const uint8_t dbg)
{
	uint32_t  i;
	uint32_t  retval = 0;

	for (i = 0; i < num_bytes && (retval == 0); i++) {
		if (probe[i] != reference[i]){
			printf("\n##ERROR: adr: 0x%04x probe: 0x%02x memory: 0x%02x\n", 
				i, probe[i], reference[i]);
			retval = 1;
		}
	}

	if (dbg)
		printf(".");

	return retval; 
}                    


/***************************************************************/
/* checks and maps user gui argument from uint_8 to 'flash_sector_t'
 * 
 * returns: if  valid: the mapped type
 *          if invalid: -1 
 */
flash_sector_t userentryidx2flash_sector(const uint8_t userentryidx)
{
	flash_sector_t ret_val= -1;
	
	switch (userentryidx)
	{
		case 0: ret_val=cfg_sector;
		break;

		case 1: ret_val=ufm_sector;
		break;

		case 2: ret_val=asc_eeprom;
		break;

		case 3: ret_val=feature_row;
		break;

		case 4: ret_val=lptm21_sram;
		break;

		case 5: ret_val=golden_spi;
		break;

		case 10: ret_val=all_sectors;
		break;

		default:
		printf("userentryidx2flash_sector(): unspecified value\n");
		break;
	}

	return ret_val;
}

/***************************************************************/
static int32_t cpld_info(struct udevice* dev)
{
	uint8_t    buf[10];
	int32_t    rc = 0, i;
	uint32_t   statusReg;

	cpld_info_short_print(dev);

	rc += lptm21_open_cfgmode(dev, mode_transparent);
	rc += lptm21_get_info(dev, LPTM21_USER_ID, buf);
	printf("UserID     : 0x%02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3]);

	rc += lptm21_get_info(dev, LPTM21_FEATURE_ROW_RD, buf);
	
	printf("Feature Row: 0x");
	for (i = 0; i < 8; i++)	{
		printf("%02x", buf[i]);
	}
	printf("\n");

	rc += lptm21_get_info(dev, LPTM21_FEA_BITS_RD, buf);
	printf("FEA bits   : 0x%02x%02x\n", buf[0], buf[1]);

	rc += lptm21_get_info(dev, LPTM21_TRACE_ID,buf);
	printf("TraceID    : 0x");
	for (i = 0; i < 8; i++){
		printf("%02x", buf[i]);
	}
	printf("\n");

	rc += lptm21_get_info(dev, LPTM21_STATUS_REGISTER, buf);
	printf("CFG Status : 0x%02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3]);
	
	statusReg = (buf[0] << 16) | (buf[1] << 8) | buf[2];

	if (statusReg & 0x01){
		printf("\t     DONE\n");
	}else if (statusReg & 0x02) {
		printf("\t     INTERFACE ENABLED\n");
	}else if (statusReg & 0x10) {
		printf("\t     BUSY\n");
	}else if (statusReg & 0x20) {
		printf("\t     FAIL\n");
	}
	
	printf("\t     Flash Check: ");
	
	switch ((statusReg >> 18) & 0x07) {
		case 0:	printf("No Err\n");break;
		case 1:	printf("ID Err\n");break;
		case 2:	printf("CMD Err\n");break;
		case 3:	printf("CRC Err\n");break;
		case 4:	printf("Preamble Err\n");break;
		case 5:	printf("Abort Err\n");break;
		case 6:	printf("Overflow Err\n");break;
		case 7:	printf("SDM EOF\n");break;
		default:printf("'cpld_info()': strange status\n");break;
	}

	if (rc)
		printf( "'cpld_info()':at least one error occured (rc=%d)\n",rc);

	return rc;
}

#ifdef NOT_USED
static int32_t cpld_read_flash(struct udevice* dev, 
	const flash_sector_t sector, uint16_t page, uint16_t num)
{
    int32_t  i;
    int32_t  rc = 0;
    uint8_t  buf[280];

    switch (sector)
    {
        case cfg_sector:
            printf("CFG page: %d\n", page+1);
            break;
        case ufm_sector:
            printf("UFM page: \n");
            break;
        case asc_eeprom:
            printf("ASC EEPROM: \n");
            break;
        case feature_row:
            printf("Feature row: %d\n", page);
            break;
        case lptm21_sram:
            printf("SRAM: %d\n", page+1);
            break;
        default:
            break;
    }

    switch (sector)
    {
        case asc_eeprom:
        break;
        
        default:
            rc += lptm21_open_cfgmode(dev, mode_transparent);
            while (num)
            {
                rc += lptm21_set_flash_page(dev, sector, page);
                rc += lptm21_read_flash_page(dev, sector, buf);
                printf("%04d:",page+1);
                for (i = 0; i < LPTM21_FLASH_PAGE_SIZE - 1; i++)
                {
                    printf(" %02x", buf[i]);
                }
                printf(" %02x\n", buf[LPTM21_FLASH_PAGE_SIZE - 1]);
                num--;
                page++;
            }
            rc += lptm21_close_cfgmode(dev);        
        break;
    }

    return rc;
}
#endif

int32_t cpld_verify_flash(struct udevice* dev, const flash_sector_t sector, uint8_t dbg)
{
	uint16_t  block_size;
	uint32_t  byte_offset;
	int32_t   rc = 0;
	uint8_t   buf[LPTM21_MAX_ASC_BYTES+3+8];
	int32_t   i;
	uint8_t  *src_reference;  
	uint32_t  max_pages;

	switch (sector)  {
	case cfg_sector:
		src_reference = (uint8_t*)gCpldImageAddr;
		max_pages     = LPTM21_MAX_CFG_PAGES;
		byte_offset   = 0;
		block_size    = LPTM21_FLASH_PAGE_SIZE;

		printf("Verifying Configuration with memory at 0x%p\n",src_reference);
		rc += lptm21_open_cfgmode(dev, mode_transparent);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		rc += lptm21_reset_address(dev, sector);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif

		printf("Verifying page: ");
		for (i = 0; i < max_pages; i++){
			rc = lptm21_read_flash_page(dev, sector, buf);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
			if(rc){
				printf("read page %d failed.\n", i);
			}
			udelay(5);
#endif
			rc += memory_compare(&src_reference[byte_offset], buf, block_size, dbg);
			printf("\033[100D\033[16C %04i, from address: %p",i,(&src_reference[byte_offset]));
			byte_offset += block_size;
		}

		rc += lptm21_close_cfgmode(dev);    		    

	break;

	case ufm_sector:
		src_reference = (uint8_t*)(gCpldImageAddr + LPTM21_UFM_DATA);
		max_pages     = LPTM21_MAX_UFM_PAGES;
		byte_offset   = 0;
		block_size    = LPTM21_FLASH_PAGE_SIZE;

		printf("Verifying UFM with memory at 0x%p\n",src_reference);
		rc += lptm21_open_cfgmode(dev, mode_transparent);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		rc += lptm21_reset_address(dev, sector);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		printf("Verifying page: ");
		for (i = 0; i < max_pages; i++){
			rc += lptm21_read_flash_page(dev, sector, buf);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
			udelay(5);
#endif
			rc += memory_compare(&src_reference[byte_offset], buf, block_size, dbg);
			printf("\033[100D\033[16C %04i, from address: 0x%p",i,&src_reference[byte_offset]);
			byte_offset += block_size;
		}

		rc += lptm21_close_cfgmode(dev);                
	break;

	case asc_eeprom:
	break;

	case feature_row:
		src_reference = (uint8_t*)(gCpldImageAddr + LPTM21_ROW_DATA);
		max_pages     = 1;
		byte_offset   = 0;
		block_size    = 10;
		printf("Verifying FEATURE ROW with memory at 0x%p\n",src_reference);

		rc += lptm21_open_cfgmode(dev, mode_transparent);
		rc += lptm21_reset_address(dev, sector);

		for (i = 0; i < max_pages; i++){
			rc += lptm21_get_info(dev, LPTM21_FEATURE_ROW_RD, buf);
			rc += lptm21_get_info(dev, LPTM21_FEA_BITS_RD, buf+8);
			rc += memory_compare(&src_reference[byte_offset], buf, block_size, dbg);
			byte_offset += block_size;
		}

		rc += lptm21_close_cfgmode(dev);
	break;

	default:
		printf("unspecified sector\n");
		rc =1;
	break;
	} /* switch */

	printf("\n");
	return rc;
}

/***************************************************************/
static int32_t cpld_verify_all(struct udevice* dev, uint8_t dbg)
{
	int32_t  rc = 0;

	rc += cpld_verify_flash(dev, cfg_sector, 0);
	rc += cpld_verify_flash(dev, ufm_sector, 0);
	rc += cpld_verify_flash(dev, asc_eeprom, 0);
	rc += cpld_verify_flash(dev, feature_row, 0);

	return rc;
}

/***************************************************************/
int32_t cpld_write_flash(struct udevice* dev, const flash_sector_t sector, uint16_t offset)
{
	int32_t  rc = 0;
	uint8_t  buf[LPTM21_FLASH_PAGE_SIZE + LPTM21_CMD_SIZE_BYTE];
	int32_t  i;
	uint8_t *psrc_addr = (uint8_t*)gCpldImageAddr;
	uint32_t max_pages = LPTM21_MAX_CFG_PAGES;

	psrc_addr += offset;
	printf("write ");

	switch (sector){
	case  ufm_sector:
		printf("UFM");
		max_pages = LPTM21_MAX_UFM_PAGES;
	break;

	case  asc_eeprom:
		printf("ASC");
		max_pages = LPTM21_MAX_UFM_PAGES;
	break;

	case  feature_row:
		printf("FEATURE ROW");
		max_pages = LPTM21_MAX_UFM_PAGES;
	break;

	default:
		printf("CFG");
		max_pages = LPTM21_MAX_CFG_PAGES;
	break;
	}

	printf(" with memory at 0x%x\n", gCpldImageAddr + offset);

	rc += lptm21_open_cfgmode(dev, mode_transparent);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
	udelay(5);
#endif

	if (sector==feature_row) {
		memcpy(&buf[4], psrc_addr, 8);
		rc += lptm21_set_feature_row(dev, buf);
		memcpy(&buf[4], psrc_addr+8, 2);
		rc += lptm21_set_fea_bits(dev, buf);      
	}else {
		rc += lptm21_erase(dev, sector);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		rc += lptm21_reset_address(dev, sector);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		printf("Writing page: ");

		for (i = 0; i < max_pages; i++)	{
			memcpy(&buf[4], psrc_addr, LPTM21_FLASH_PAGE_SIZE);
			//printf(".");
			printf("\033[100D\033[14C %04i, from address: 0x%p",i,psrc_addr);

			rc = lptm21_write_flash_page(dev, sector, buf);
			if(rc){
				printf("write page %d failed.\n", i);
			}
			
			psrc_addr += LPTM21_FLASH_PAGE_SIZE;
			udelay(400);
		}
	}
	
	printf("\n");

	rc += lptm21_close_cfgmode(dev);
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
	udelay(5);
#endif
	return rc;
}

/***************************************************************/
static int32_t cpld_erase(struct udevice* dev, const flash_sector_t sector)
{
	int32_t rc = 0;
	uint8_t buf[2*(LPTM21_FLASH_PAGE_SIZE + LPTM21_CMD_SIZE_BYTE)];

	rc = lptm21_open_cfgmode(dev, mode_transparent);

#ifdef ADVA_CPLD_LOAD_OPTIMIZE
	udelay(5);
#endif
	switch (sector){
	case cfg_sector:
		printf("Erasing CFG flash ...\n");
		rc += lptm21_erase(dev,cfg_sector);
	break;

	case ufm_sector:
		printf("Erasing UFM flash ...\n");
		lptm21_reset_address(dev, ufm_sector);
		lptm21_read_flash_page(dev, ufm_sector, buf+LPTM21_CMD_SIZE_BYTE);  // page 0 and 73 has to restored after erase
		lptm21_set_flash_page(dev, ufm_sector, 73);                         // due to unknown Lattice reasons
		lptm21_read_flash_page(dev, ufm_sector, buf+sizeof(buf)/2+LPTM21_CMD_SIZE_BYTE);         

		rc += lptm21_erase(dev,ufm_sector);

		lptm21_reset_address(dev, ufm_sector);
		lptm21_write_flash_page(dev, ufm_sector, buf);
		lptm21_set_flash_page(dev, ufm_sector, 73);
		lptm21_write_flash_page(dev, ufm_sector, buf+sizeof(buf)/2);
	break;

	case feature_row:
		printf("Erasing FEATURE ROW ...\n");
		rc += lptm21_erase(dev,feature_row);
	break;

	case lptm21_sram:
		printf("Erasing SRAM ...\n");
		rc += lptm21_erase(dev,lptm21_sram);
	break;

	case golden_spi:
		printf("Erasing SPI flash ... waiting ...\n");
		printf("Done\n");
	break;

	default:
		printf("not supported sector!\n");
		rc += 1;
	break;
	}

	rc += lptm21_close_cfgmode(dev);
	
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
	udelay(5);
#endif

	return rc;
}

int32_t cpld_update(struct udevice* dev, const flash_sector_t sector)
{
	int32_t  rc = 0;
	uint8_t  buf[4];
	int32_t  statusReg;
	//uint8_t *psrc_addr = (uint8_t*)DEFAULT_SRC_ADDR;

	switch (sector){
	case cfg_sector:
		cpld_write_flash(dev, cfg_sector, 0);                    /* CFG data */
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		rc += cpld_verify_flash(dev, cfg_sector, 0);
	break;

	case ufm_sector:
		cpld_write_flash(dev, ufm_sector, LPTM21_UFM_DATA);      /* UFM data */
		
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		rc += cpld_verify_flash(dev, ufm_sector, 0);
	break;

	case asc_eeprom:
		printf("Not implemented yet\n");
		return 1;
	break;

	case feature_row:
		cpld_write_flash(dev, feature_row, LPTM21_ROW_DATA);              /* Feature row */
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		rc += cpld_verify_flash(dev, feature_row, 0);
	break;

	case golden_spi:
		printf("\nno golden_spi\n");
	break;

	default:
		printf("unspecified sector\n");
	break;
	}

	if (sector!=golden_spi){
		rc += lptm21_open_cfgmode(dev, mode_transparent);
		
#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
		rc += lptm21_set_done(dev);

#ifdef ADVA_CPLD_LOAD_OPTIMIZE
		udelay(5);
#endif
	rc += lptm21_get_info(dev, LPTM21_STATUS_REGISTER, buf);
	statusReg = (buf[0] << 16) | (buf[1] << 8) | buf[2];

	if ((statusReg & 0x31) == 0x01)	{
		printf("Done programmed\n");
	}else{
		printf("failed to program Done, status = 0x%02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3]);
	}

#ifdef ADVA_CPLD_LOAD_OPTIMIZE
	udelay(5);
#endif

	rc += lptm21_close_cfgmode(dev);
	}

	return rc;
}

/***************************************************************/
int32_t cpld_update_all(struct udevice* dev)
{
	int32_t  rc = 0;

	rc += cpld_update(dev, cfg_sector);
	//rc += cpld_update(dev, ufm_sector);
	//rc += cpld_update(dev, feature_row);

	return rc;
}

#ifdef NOT_USED
static int32_t cpld_modify_flash(struct udevice* dev, const flash_sector_t sector, 
					uint16_t page, uint8_t addr, uint8_t data)
{
	uint8_t  *psrc_addr = (uint8_t*)gCpldImageAddr+LPTM21_UFM_DATA;
	int32_t   rc = 0;
	uint8_t   buf[LPTM21_FLASH_PAGE_SIZE + LPTM21_CMD_SIZE_BYTE];
	int32_t   i;
	uint32_t  max_pages = LPTM21_MAX_CFG_PAGES;

	if (sector)	{
		max_pages = LPTM21_MAX_UFM_PAGES;
	}

	rc = lptm21_open_cfgmode(dev, mode_transparent);
	rc = lptm21_reset_address(dev, sector);

	rc = 0;
	for (i = 0; i < max_pages; i++){
		rc += lptm21_read_flash_page(dev, sector, buf);
		memcpy(psrc_addr, buf, LPTM21_FLASH_PAGE_SIZE);
		psrc_addr += LPTM21_FLASH_PAGE_SIZE;
	}

	((uint8_t*)gCpldImageAddr+LPTM21_UFM_DATA)[(LPTM21_FLASH_PAGE_SIZE * page) + addr] = data;

	rc = lptm21_erase(dev, sector);

	rc = lptm21_reset_address(dev, sector);

	psrc_addr = (uint8_t*)gCpldImageAddr+LPTM21_UFM_DATA;
	rc = 0;
	for (i = 0; i < max_pages; i++){
		memcpy(&buf[LPTM21_CMD_SIZE_BYTE], psrc_addr, LPTM21_FLASH_PAGE_SIZE);
		rc += lptm21_write_flash_page(dev, sector, buf);
		psrc_addr += LPTM21_FLASH_PAGE_SIZE;
		udelay(200);
	}

	rc = lptm21_close_cfgmode(dev);
	rc = cpld_verify_flash(dev, sector, 0);
	
	return rc;
}
#endif

static int32_t cpld_set_user_id(struct udevice* dev, uint32_t id)
{
	int32_t rc = 0;
	uint8_t buf[32];

	rc += lptm21_open_cfgmode(dev, mode_transparent);
	rc += lptm21_set_user_id(dev, id);
	rc += lptm21_get_info(dev, LPTM21_USER_ID, (uchar*)&buf);
	rc += lptm21_close_cfgmode(dev);
	
	printf("UserID     : 0x%02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3]);

	return rc;
}

static int do_cpld(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint8_t           mode;
	flash_sector_t    sector;
	uint32_t          val;
	struct udevice* i2cdev = NULL;
	int ret = 0;
	
 	ret = i2c_get_chip_for_busnum(CONFIG_SYS_I2C_CPLD_BUS_NUM, 
		CONFIG_SYS_I2C_CPLD_PROG_ADDR, 1, &i2cdev);
	if (ret != 0) {
		printf("%s failed to get i2c dev, ret=%d\n",
			__func__, ret);
		return 0;
	}

	if (!strncmp(argv[1], "info", 2)) {
		cpld_info(i2cdev);
	}else if (!strncmp(argv[1], "update", 2)) {
		if (argc < 4)
			return CMD_RET_USAGE;
		
		gCpldImageAddr = simple_strtoul(argv[2], NULL, 16);
		mode = simple_strtoul(argv[3], NULL, 16);
		sector = userentryidx2flash_sector(mode);
		if (sector == all_sectors)
			cpld_update_all(i2cdev);
		else	
			cpld_update(i2cdev, sector);
	}else if (!strncmp(argv[1], "erase", 3)) {
		if (argc < 3)
			return CMD_RET_USAGE;

		mode = simple_strtoul(argv[2], NULL, 10);
		cpld_erase(i2cdev, mode);
	}else if (!strncmp(argv[1], "verify", 3)) {
		if (argc < 4)
			return CMD_RET_USAGE;
		
		gCpldImageAddr = simple_strtoul(argv[2], NULL, 16);
		mode = simple_strtoul(argv[3], NULL, 16);
		sector = userentryidx2flash_sector(mode);
		if (sector == all_sectors)
			cpld_verify_all(i2cdev, 0);
		else
			cpld_verify_flash(i2cdev, sector, 0);
    }else if (!strcmp(argv[1], "uw")) {
        if (argc < 3)
            return CMD_RET_USAGE;
		
        val = simple_strtoul(argv[2], NULL, 16);
		cpld_set_user_id(i2cdev, val);
	}else {
		return CMD_RET_USAGE;
	}

	return 0;
}

/***************************************************************/
int32_t cpld_info_short_print(struct udevice* dev)
{
	uint8_t   buf[8];
	int32_t   rc = 0;

	rc += lptm21_get_info(dev, LPTM21_DEVICE_ID, buf);

	printf("DevID:0x%02x%02x%02x%02x\n",
		buf[0], buf[1], buf[2], buf[3]);

	return rc;
}

U_BOOT_CMD(
    burncpld, 7, 2, do_cpld,
    "subsystem CPLD",
    "info                                 - read ID's from device\n"
    "burncpld update <addr> <sector>               - update CPLD flash with memory image\n"
    "                                               sector: 0 - CFG\n"
    "                                                       1 - UFM\n"
    "                                                       a - ALL\n"
    "burncpld erase <sector>                       - erase any/all sectors of device\n"
    "                                               sector: 0 - CFG\n"
    "                                                       1 - UFM\n"
    "burncpld verify <addr> <sector>               - verify with memory image\n"
    "                                               sector: 0 - CFG\n"
    "                                                       1 - UFM\n"
    "                                                       a - ALL\n"
    "burncpld uw <user_id>                         - set user ID\n"
);

