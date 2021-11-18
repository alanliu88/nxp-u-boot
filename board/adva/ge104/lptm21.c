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
#include <i2c.h>

#include "cpld.h"
#include "lptm21.h"

#define PAGE_MASK   0x3fff /* 14bits */

static uint32_t g_cfg_i2c_lptm21_bus = CFG_I2C_LPTM21_BUS;
static uint8_t g_cfg_i2c_lptm21_cfgaddr = ECM2_I2C_LPTM21CFG_ADDR;


void lptm21_busnr_set(const uint32_t busnr)
{
   g_cfg_i2c_lptm21_bus = busnr;
}


void lptm21_busnr_reset(void)
{
    g_cfg_i2c_lptm21_bus = CFG_I2C_LPTM21_BUS;
}

void lptm21_cfgaddr_set(const uint8_t cfgaddr)
{
   g_cfg_i2c_lptm21_cfgaddr = cfgaddr;
}

void lptm21_cfgaddr_reset(const uint8_t cfgaddr)
{
   g_cfg_i2c_lptm21_cfgaddr = CFG_I2C_LPTM21_CONFIG;
}

uint8_t lptm21_cfgaddr_get()
{
    return g_cfg_i2c_lptm21_cfgaddr;
}

int32_t lptm21_open_cfgmode(uint8_t dev, const flash_mode_t mode)
{
	uint8_t buf[4];
	int32_t rc = 0;

    buf[0] = 0x74;              /* Enable Config Interface in Transparent Mode opcode */
    buf[1] = 0x08;              /* arg0 */
    buf[2] = 0x00;              /* arg1 */

    if (mode == mode_offline)
    {
        buf[0] = 0xC6;          /* Enable Config  Interface in Offline Mode opcode */
    }
    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, 3);

    rc += lptm21_wait_busy(dev);

    return rc;
}


int32_t lptm21_wait_busy(uint8_t dev)
{
    uint32_t addr;
    uint8_t  buf[4];
    int32_t  rc = 0;
    int32_t  loop;

    loop = LPTM21_CMD_LOOP_TIMEOUT;
    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);

    do {
        addr = (LPTM21_STATUS_REGISTER << 24) | /* opcode */
               (0 << 16) | (0 << 8) | 0;        /* arg0,arg1,arg2 */

        rc += i2c_read(dev, addr, 4, buf, 4);
        --loop;

        if (rc)
        {
            printf("##ERROR: I2C transfer\n");
            return rc;
        }

        if (buf[2] & 0x20)           /* FAIL bit set */
        {
            printf("##ERROR: failed bit set in status register: 0x%02x\n", buf[2]);
            return -1;
        }

    } while (loop && (buf[2] & 0x10));
    /*  printf("status: 0x%02x\n",buf[2]); */

    if (!loop)
    {
        printf("##ERROR: timeout at wait for busy flag in status register\n");
    }

    return rc;
}

int32_t lptm21_close_cfgmode(uint8_t dev)
{
    uint8_t buf[4];
    int32_t rc = 0;

    buf[0] = 0x26;      /* Disable Config Interface in Transparent Mode opcode */
    buf[1] = 0x00;      /* arg0 */
    buf[2] = 0x00;      /* arg1 */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, 3);
    buf[0] = 0xFF;      /* Bypass opcode - supposedly does not have arguements, just command byte */
    rc += i2c_write(dev, 0, 0, buf, 1);

    return rc;
}

int32_t lptm21_erase(uint8_t dev, const flash_sector_t sector)
{
	uint8_t buf[LPTM21_CMD_SIZE_BYTE];
	int32_t rc = 0;

	buf[0] = 0x0E;                  /* Erase Flash opcode */
	buf[2] = 0x00;                  /* Operand 2 */
	buf[3] = 0x00;                  /* Operand 3 */

	/* Operand 1: */
	switch (sector)
	{
		case cfg_sector:
			buf[1] = (uint8_t)0x04;
		break;

		case ufm_sector:
			buf[1] = (uint8_t)0x08;
		break;

		case feature_row:
			buf[1] = (uint8_t)0x02;
		break;

		case lptm21_sram:
			buf[1] = (uint8_t)0x01;
		break;

		default:
			printf("lptm21_erase(): illegal flash sector specified\n");
			rc = 1;
		break;
	}


	if (0 == rc)
	{
	    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
	    rc += i2c_write(dev, 0, 0,buf, 4);
	    mdelay(700);
	    rc += lptm21_wait_busy(dev);
	}

    return rc;
}

int32_t lptm21_refresh(uint8_t dev)
{
	uint8_t buf[4];
	int32_t rc = 0;

    buf[0] = 0x79;              /* opcode */
    buf[1] = 0x00;              /* arg0 */
    buf[2] = 0x00;              /* arg1 */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, 3);

    return rc;
}

int32_t lptm21_reset_address(uint8_t dev, const flash_sector_t sector)
{
	uint8_t buf[4];
	int32_t rc = 0;

    buf[0] = 0x46;              /* opcode */
    if (sector == ufm_sector)
    {
        buf[0] = 0x47;          /* UFM opcode */
    }
    buf[1] = 0x00;              /* arg0 */
    buf[2] = 0x00;              /* arg1 */
    buf[3] = 0x00;              /* arg2 */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, 4);

    return rc;
}

int32_t lptm21_set_flash_page(uint8_t dev, const flash_sector_t sector, uint16_t page)
{
	uint8_t buf[8];
	int32_t rc = 0;

    buf[0] = 0xB4;              /* opcode */
    buf[1] = 0x00;              /* arg0 */
    buf[2] = 0x00;              /* arg1 */
    buf[3] = 0x00;              /* arg2 */

    if (sector == cfg_sector)
    {
        buf[4] = 0x00;          /* page[0] = 0=Config, 1=UFM */
    }
    else
    {
        buf[4] = 0x40;          /* page[0] = 0=Config, 1=UFM */
    }

    buf[5] = 0x00;                         /* page[1] */

    page  &= (uint16_t)PAGE_MASK;          /* mask 14 lsb bits , see spec. page 51*/
    buf[6] = (uint8_t) (page >> 8);        /* page[2] = page number MSB */
    buf[7] = (uint8_t) (page & 0x00ff);    /* page[3] = page number LSB */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc = i2c_write(dev, 0, 0, buf, 8);

    return rc;
}

/*
 * @note This command is only useful if the USERCODE contents is previously all 0's.
 * The USERCODE is cleared when the Cfg sector is erased.  So there is no way to
 * individually clear just the USERCODE and reprogram with a new value using this command.
 * Its usefulness is questionable, seeing as the USERCODE is set when programming the
 * Cfg sector anyway.
 */
int32_t lptm21_set_user_id(uint8_t dev, uint32_t id)
{
	uint8_t buf[8];
	int32_t rc = 0;

    buf[0] = 0xC2;              /* opcode */
    buf[1] = 0x00;              /* arg0 */
    buf[2] = 0x00;              /* arg1 */
    buf[3] = 0x00;              /* arg2 */

    buf[4] = (uint8_t)(id >> 24);
    buf[5] = (uint8_t)(id >> 16);
    buf[6] = (uint8_t)(id >> 8);
    buf[7] = (uint8_t)(id);

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, 8);

    rc += lptm21_wait_busy(dev);

    return rc;
}

int32_t lptm21_set_feature_row(uint8_t dev, uint8_t *buf)
{
	int32_t rc = 0;

    buf[0] = LPTM21_FEATURE_ROW_WR;   /* opcode */
    buf[1] = 0x00;                    /* arg0 */
    buf[2] = 0x00;                    /* arg1 */
    buf[3] = 0x00;                    /* arg2 */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, 12);

    rc += lptm21_wait_busy(dev);

    return rc;
}

int32_t lptm21_set_fea_bits(uint8_t dev, uint8_t *buf)
{
	int32_t rc = 0;

    buf[0] = LPTM21_FEA_BITS_WR;      /* opcode */
    buf[1] = 0x00;                    /* arg0 */
    buf[2] = 0x00;                    /* arg1 */
    buf[3] = 0x00;                    /* arg2 */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, 6);

    rc += lptm21_wait_busy(dev);

    return rc;
}

int32_t lptm21_read_flash_page(uint8_t dev, const flash_sector_t sector, uint8_t *buf)
{
    uint32_t addr;

    if (sector == ufm_sector)
    {
        addr = 0xCA << 24;      /* read UFM opcode */
    }
    else
    {
        addr = 0x73 << 24;      /* read CFG opcode */

    }
    addr |= (0 << 16) | (0 << 8) | 1;     /* arg0, arg1, 1 page */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    return i2c_read(dev, addr, 4, buf, LPTM21_FLASH_PAGE_SIZE);
}

int32_t lptm21_write_flash_page(uint8_t dev, const flash_sector_t sector, uint8_t *buf)
{
	int32_t rc = 0;

    buf[0]     = 0x70;           /* write CFG opcode */
    if (sector == ufm_sector)
    {
        buf[0]     = 0xC9;       /* write UFM opcode */
    }
    buf[1] = 0x00;         /* arg0 */
    buf[2] = 0x00;         /* arg1 */
    buf[3] = 0x01;         /* 1 page */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, LPTM21_CMD_SIZE_BYTE + LPTM21_FLASH_PAGE_SIZE);

    rc += lptm21_wait_busy(dev);

    return rc;
}

int32_t lptm21_set_done(uint8_t dev)
{
	int32_t rc    = 0;
	uint8_t buf[LPTM21_CMD_SIZE_BYTE];

    buf[0] = 0x5E;              /* opcode */
    buf[1] = 0x00;              /* arg0 */
    buf[2] = 0x00;              /* arg1 */
    buf[3] = 0x00;              /* arg2 */

    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    rc += i2c_write(dev, 0, 0, buf, LPTM21_CMD_SIZE_BYTE);
    udelay(10000);

    return rc;
}

int32_t lptm21_get_info(uint8_t dev, uint8_t reg, uint8_t *buf)
{
	int32_t  rc = 0;
    uint32_t addr;

    addr = (reg << 24) | (0 << 16) | (0 << 8) | 0; /* opcode, arg0, arg1, arg2 */
    i2c_set_bus_num(g_cfg_i2c_lptm21_bus);
    if (reg == LPTM21_TRACE_ID || reg == LPTM21_FEATURE_ROW_RD)
    {
        rc += i2c_read(dev, addr, 4, buf, 8);
    }
    else
    {
        rc += i2c_read(dev, addr, 4, buf, 4);
    }

    return rc;
}
