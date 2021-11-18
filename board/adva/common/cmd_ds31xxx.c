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
#include <spi.h>
#include "spi_common.h"
#include "cpld.h"
DECLARE_GLOBAL_DATA_PTR;

typedef enum { OP_NONE, OP_READ, OP_WRITE, OP_DUMP, OP_INIT } op_t;

unsigned char  last_op;
unsigned short last_addr;
unsigned char  last_data;
unsigned short last_dump_addr;

static int spi_cs = 0;
static int spi_hz = SPI_SPEED_2MHZ;

__weak int setup_ds31407 (void)
{
	return 0;
}

static int
setup_addr(op_t op, unsigned short reg, unsigned char *data, int len)
{
	data[0] = ((reg >> 7) & 0x7F) | ((op == OP_WRITE) ? WRITE : READ);
	data[1] = ((reg << 1) & 0xFE) | ((len > 1) ? BURST : SINGLE);

	return 0;
}

int
spi_access (   cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	char            *cmd;
	int             cmdlen;
	int             ret = 0;
	unsigned char   op;
	unsigned short  addr;
	unsigned short  dump_addr;
	unsigned char   data;
	struct spi_slave *slave;
	unsigned char   dout[18];
	unsigned char   din[18];
	
#ifdef CONFIG_DM_SPI
	char name[30], *str;
	struct udevice *dev;
#endif

	cmd = argv[1];
	cmdlen = strlen(cmd);

	op = last_op;
	addr = last_addr;
	data = last_data;
	dump_addr = last_dump_addr;

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		op = OP_NONE;
		if (argc >= 3) {
			addr = simple_strtoul (argv[2], NULL, 16);
		}
		if (argc >= 4) {
			data = simple_strtoul (argv[3], NULL, 16);
		}
	}

	if ((argc < 2) || (cmdlen < 2)) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 2;
	}

	if (addr > MAX_REG) {
		printf("error: <reg> out of range\n");
		return 1;
	}

#ifdef CONFIG_DM_SPI
	snprintf(name, sizeof(name), "generic_%d:%d", SPI_BUS, spi_cs);
	str = strdup(name);
	if (!str)
		return -ENOMEM;
	ret = spi_get_bus_and_cs(SPI_BUS, spi_cs, spi_hz, SPI_MODE, "spi_generic_drv",
	           str, &dev, &slave);
	if (ret)
		return ret;
#else
	slave = spi_setup_slave(SPI_BUS, spi_cs, spi_hz, SPI_MODE);
	if (!slave) {
		printf("Invalid device %d:%d\n", SPI_BUS, cs);
		return -EINVAL;
	}
#endif

	ret = spi_claim_bus(slave);
	if (ret) {
		printf("spi claim bus failed\n");
		goto done;
	}

	if ((op == OP_READ) || (strncmp(cmd, "read", cmdlen) == 0)) {
		op = OP_READ;
		setup_addr(op, addr, dout, 1);
		dout[2] = 0;
		if (spi_xfer(slave, BIT_LEN, dout, din, SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
			printf("error: unable to read data");
			ret = 1;
		} else {
			printf("%04x: %02x\n", addr, din[2]);
			addr++;
		}
	}else if ((op == OP_WRITE) || (strncmp(cmd, "write", cmdlen) == 0)) {
		op = OP_WRITE;
		setup_addr(op, addr, dout, 1);
		dout[2] = data;
		if (spi_xfer(slave, BIT_LEN, dout, din, SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
			printf("error: unable to write data\n");
			ret = 1;
		} else {
			addr++;
		}
	} else if ((op == OP_DUMP) || (strncmp(cmd, "dump", cmdlen) == 0)) {
		int n, i;

		dump_addr = (((op == OP_NONE) && (argc >= 3)) ? addr : last_dump_addr) & ~0x0f;
		op = OP_DUMP;

		for (n=0; n<0x80; n+=16) {
			if (dump_addr >= MAX_REG){
			    dump_addr = 0;
			}
			setup_addr(op, dump_addr, dout, 18);
			dout[2] = 0;
			if (spi_xfer(slave, 8 * 18, dout, din, SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
				printf("error: unable to read data\n");
				ret = 1;
				break;
			}
			printf("%04x:", dump_addr);
			for (i=0; i<16; i++) {
				printf(" %02x", din[i + 2]);
			}
			printf("\n");
			dump_addr += 16;
		}
	}
#if defined(CONFIG_ADVA_DS31407)
	else if ((op == OP_INIT) || (strncmp(cmd, "init", cmdlen) == 0)) {
		int rc = 0;
		op = OP_INIT;

		/*
		* Initialize the ds31407 clock chip
		*/

		/* Reset the chip */
		CPLD_SET_BITS (CPLD_RESET0_CNTRL_REG, CPLD_RESET0_CNTRL_DS310X);
		udelay (1);
		CPLD_CLR_BITS (CPLD_RESET0_CNTRL_REG, CPLD_RESET0_CNTRL_DS310X);
		udelay (150);

		puts ("Programming clocks... ");
		rc = setup_ds31407();

		if (rc == 0) {
			printf("done\n");
		}else{
			if (rc == -2){
			    puts ("failed - invalid chip ID\n");
			}else{
			    puts ("write failed\n");
			}
			udelay (5000000);
		}
	}
#endif
	else {
		printf("Usage:\n%s\n", cmdtp->usage);
		ret = 2;
	}

	if (ret == 0) {
		last_op = op;
		last_addr = addr;
		last_data = data;
		last_dump_addr = dump_addr;
	}

done:
	spi_release_bus(slave);
#ifndef CONFIG_DM_SPI
	spi_free_slave(slave);
#endif

	return ret;
}

#if defined(CONFIG_ADVA_DS310X)
int
do_ds310x (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	spi_cs = 0;
	return spi_access (cmdtp, flag, argc, argv);
}

U_BOOT_CMD (ds310x, 4, 1, do_ds310x,
        "ds310x commands",
        "dump <reg>           - dump registers\n"
        "ds310x read <reg>           - read register\n"
        "ds310x write <reg> <value>  - write register\n"
        );
#endif

#if defined(CONFIG_ADVA_DS26521)
int
do_ds26521 (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	spi_cs = 1;
	return spi_access (cmdtp, flag, argc, argv);
}

U_BOOT_CMD (ds26521, 4, 1, do_ds26521,
        "ds26521 commands",
        "dump <reg>           - dump registers\n"
        "ds26521 read <reg>           - read register\n"
        "ds26521 write <reg> <value>  - write register\n"
        );
#endif

#if defined(CONFIG_ADVA_DS31408)
int
do_ds31408 (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	spi_cs = CONFIG_ADVA_DS31408_CS;
	return spi_access (cmdtp, flag, argc, argv);
}

U_BOOT_CMD (ds31408, 4, 1, do_ds31408,
        "ds31408 commands",
        "dump <reg>           - dump registers\n"
        "ds31408 read <reg>           - read register\n"
        "ds31408 write <reg> <value>  - write register\n"
        );
#endif

#if defined(CONFIG_ADVA_DS31407)
int
do_ds31407(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	spi_cs = CONFIG_ADVA_DS31407_CS;
    return spi_access (cmdtp, flag, argc, argv);
}

U_BOOT_CMD (ds31407, 5, 1, do_ds31407,
        "ds31407 [dump] | [read] | [write] [init]",
        "dump <reg>           - dump registers\n"
        "ds31407 read <reg>           - read register\n"
        "ds31407 write <reg> <value>  - write register\n"
        "ds31407 init                 - init ds31407\n"
        );
#endif

#if defined(CONFIG_ADVA_SPI_RAM)
static int spi_mem_mw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned char   op;
	unsigned short  addr;
	struct spi_slave *slave;
	unsigned char   dout[18];
	unsigned char   din[18];
	ulong	writeval, count;
	int ret = 0;
	int i;

#if 0
	printf("argc=%d, cmd = :\n", argc);
	for (i=0; i < argc; i++)
	{
		printf("argv[%d]:%s\n", i, argv[i]);
	}
#endif
	if ((argc < 3) || (argc > 4))
		return CMD_RET_USAGE;

	/*check cmd*/
	if(strncmp(argv[1], "mw", 2) == 0)
	{
		op = OP_WRITE;
	}
	else if(strncmp(argv[1], "md", 2) == 0)
	{
		op = OP_READ;
	}
	else
	{
		return -1;
	}

	/* Address is specified since argc > 1
	*/
	addr = simple_strtoul(argv[2], NULL, 16);

	/* Get the value to write.
	*/
	if(op == OP_WRITE)
	{
		writeval = simple_strtoul(argv[3], NULL, 16);
	}
	else if(op == OP_READ)
	{
		/* Count ? */
		if (argc == 4) {
			count = simple_strtoul(argv[3], NULL, 16);
		} else {
			count = 1;
		}
	}

#ifdef CONFIG_DM_SPI
			char name[30], *str;
			struct udevice *dev;
		
			snprintf(name, sizeof(name), "generic_%d:%d", SPI_BUS, spi_cs);
			str = strdup(name);
			if (!str)
				return -ENOMEM;
			ret = spi_get_bus_and_cs(SPI_BUS, spi_cs, spi_hz, SPI_MODE, "spi_generic_drv",
						 str, &dev, &slave);
			if (ret)
				return ret;
#else
			slave = spi_setup_slave(SPI_BUS, spi_cs, spi_hz, SPI_MODE);
			if (!slave) {
				printf("Invalid device %d:%d\n", SPI_BUS, cs);
				return -EINVAL;
			}
#endif

	ret = spi_claim_bus(slave);
	if (ret) {
		printf("spi claim bus failed\n");
		goto done;
	}

	if(op == OP_WRITE)
	{
		dout[0] = 0x02;
		dout[1] = (unsigned char)(addr >> 8);
		dout[2] = (unsigned char)(addr);
		dout[3] = (unsigned char)(writeval);
		if (spi_xfer(slave, 32, dout, din, SPI_XFER_BEGIN | SPI_XFER_END) != 0)
		{
			printf("error: unable to write data\n");
			ret = 1;
		}
		else
		{
			addr++;
		}
	}
	else if(op == OP_READ)
	{
		for (i = 0; i < count; i++)
		{
			dout[0] = 0x03;
			dout[1] = (unsigned char)(addr >> 8);
			dout[2] = (unsigned char)(addr);
			if (spi_xfer(slave, 32, dout, din, SPI_XFER_BEGIN | SPI_XFER_END) != 0)
			{
				printf("error: unable to read data");
				ret = 1;
			}
			else
			{
				printf("%04x: %02x\n", addr, din[3]);
				addr++;
			}
		}
	}

done:
	spi_release_bus(slave);
#ifdef CONFIG_DM_SPI
	spi_free_slave(slave);
#endif

	return ret;
}

int
do_spiram (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	spi_cs = CONFIG_ADVA_SPI_RAM_CS;
	ret = spi_mem_mw(cmdtp, flag, argc , &argv[0]);
	return ret;
}

U_BOOT_CMD (spiram, 5, 1, do_spiram,
        "spiram commands\n",
        "md[.b, .w, .l] address [count] - memory display\n"
        "mw[.b, .w, .l] address value   - memory write (fill)\n"
        );
#endif

