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
#include <asm/io.h>
#include <i2c.h>
#include "console.h"
#include "cpld.h"
#include "lptm21.h"


DECLARE_GLOBAL_DATA_PTR;

#define err(fmt,args...)	printf(fmt ,##args)
#define dbg(fmt,args...)	printf(fmt ,##args)

/***************************************************************/
/*! read a CPLD register from address space and print its value
 *
 * argv[2] - register number
 */
static int cpld_reg_read(struct udevice* dev, int argc, char * const argv[])
{
    ulong reg;
    char *end;

    if (argc < 3)
    {
        err("missing register number");
        return CMD_RET_USAGE;
    }

    reg = simple_strtoul(argv[2], &end, 16);
    if (argv[2] == end)
    {
        err("invalid register number %s", argv[2]);
        return CMD_RET_USAGE;
    }
    
    printf("0x%02lx: 0x%x\n", reg, dm_i2c_reg_read(dev, reg));
    
    return CMD_RET_SUCCESS;
}

/***************************************************************/
/*! write to a CPLD register from address space
 *
 * argv[2] - register number
 * argv[3] - new value
 */
static int cpld_reg_write(struct udevice *dev, int argc, char * const argv[])
{
    ulong reg;
    ulong val;
    char *end;

    if (argc < 4)
    {
        err("missing register number and/or value");
        return CMD_RET_USAGE;
    }

    reg = simple_strtoul(argv[2], &end, 16);
    if (argv[2] == end)
    {
        err("invalid register number %s", argv[2]);
        return CMD_RET_USAGE;
    }

    val = simple_strtoul(argv[3], &end, 16);
    if (argv[3] == end || val > 255)
    {
        err("invalid value %s", argv[3]);
        return CMD_RET_USAGE;
    }

    dm_i2c_reg_write(dev, reg, val);

    return CMD_RET_SUCCESS;
}

/***************************************************************/
/*! write to a CPLD register from address space
 *
 * Update only the bits which are set in mask
 *
 * argv[2] - register number
 * argv[3] - new value
 * argv[4] - mask
 */
static int cpld_reg_write_masked(struct udevice *dev, int argc, char * const argv[])
{
    ulong reg;
    ulong val;
    ulong mask;
    unsigned char old_val, new_val;
    char *end;

    if (argc < 5)
    {
        err("missing register number, value and/or mask");
        return CMD_RET_USAGE;
    }

    reg = simple_strtoul(argv[2], &end, 16);
    if (argv[2] == end)
    {
        err("invalid register number %s", argv[2]);
        return CMD_RET_USAGE;
    }

    val = simple_strtoul(argv[3], &end, 16);
    if (argv[3] == end || val > 255)
    {
        err("invalid value %s", argv[3]);
        return CMD_RET_USAGE;
    }

    mask = simple_strtoul(argv[4], &end, 16);
    if (argv[4] == end || mask > 255)
    {
        err("invalid mask %s", argv[4]);
        return CMD_RET_USAGE;
    }

    old_val = dm_i2c_reg_read(dev, reg);

    new_val = (old_val & ~mask) | (val & mask);
    dbg("register 0x%lx := 0x%02x (old_val 0x%02x, cmdline 0x%lx mask 0x%lx)",
        reg, new_val, old_val, val, mask);

    dm_i2c_reg_write(dev, reg, new_val);

    return CMD_RET_SUCCESS;
}

/***************************************************************/
static int do_cpld(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
    int ret = 0;
    struct udevice* i2cdev = 0;

    ret = i2c_get_chip_for_busnum(CONFIG_SYS_I2C_CPLD_BUS_NUM, 
            CONFIG_SYS_I2C_CPLD_ADDR, 1, &i2cdev);
    if (ret != 0) 
    {
        printf("%s failed to get i2c dev %d:0x%x, ret=%d\n",
            __func__, CONFIG_SYS_I2C_CPLD_BUS_NUM, CONFIG_SYS_I2C_CPLD_ADDR,ret);
        return 0;
    }

    else if (!strcmp(argv[1], "rr"))
    {
        return cpld_reg_read(i2cdev, argc, argv);
    }
    else if (!strcmp(argv[1], "rw"))
    {
        return cpld_reg_write(i2cdev, argc, argv);
    }
    else if (!strcmp(argv[1], "rwm"))
    {
        return cpld_reg_write_masked(i2cdev, argc, argv);
    }
    else
    {
        return CMD_RET_USAGE;
    }

    return 0;
}

U_BOOT_CMD(
    cpld, 7, 2, do_cpld,
    "subsystem CPLD",
    "rr <reg_addr>                        - read a CPLD register (from address space)\n"
    "cpld rw <reg_addr> <value>                - write <value> to a CPLD register\n"
    "cpld rwm <reg_addr> <value> <mask>        - update only the bits set in <mask> in the CPLD register\n"
);


