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

#ifndef _LPTM21_H_
#define _LPTM21_H_

#define ECM2_I2C_CPLD_BUS_NUM              0  /* on which I2C bus is the CPLD/LPTM21ASC (verify the index of array 'CONFIG_SYS_I2C_BUSES' below) */

#define XG480_MB_I2C_BUS_NUM             0
#define XG480_IO1_I2C_BUS_NUM            6
#define XG480_IO2_I2C_BUS_NUM            7

#define XG480_MB_I2C_LPTM21CFG_ADDR      0x40
#define XG480_IO1_I2C_LPTM21CFG_ADDR     0x40
#define XG480_IO2_I2C_LPTM21CFG_ADDR     0x40

#define DEFAULT_SRC_ADDR  0x80100000

/* I2C Ctrl1 = 0  -------------------------------------------------------- */
#define ECM2_I2C_GPIO0_ADDR         0x20
#define ECM2_I2C_GPIO1_ADDR         0x21
#define ECM2_I2C_BOARDEEPROM_ADDR   0x50  /* for the first bank, 4 Banks a 256 Bytes Address 0x50..0x53 */
#define ECM2_I2C_LPTM21CFG_ADDR     0x40//0x30
#define ECM2_I2C_LPTM21ASC_ADDR     0x60

#define CFG_I2C_LPTM21_CONFIG       lptm21_cfgaddr_get() //ECM2_I2C_LPTM21CFG_ADDR
#define CFG_I2C_LPTM21_RENO_CONFIG  0x40
#define CFG_I2C_LPTM21_ASC          ECM2_I2C_LPTM21ASC_ADDR
#define CFG_I2C_LPTM21_BUS          ECM2_I2C_CPLD_BUS_NUM

#define LPTM21_CMD_SIZE_BYTE        4

#define LPTM21_DEVICE_ID            0xE0
#define LPTM21_USER_ID              0xC0
#define LPTM21_TRACE_ID             0x19
#define LPTM21_FEATURE_ROW_WR       0xE4
#define LPTM21_FEATURE_ROW_RD       0xE7
#define LPTM21_FEA_BITS_WR          0xF8
#define LPTM21_FEA_BITS_RD          0xFB
#define LPTM21_STATUS_REGISTER      0x3C

#define LPTM21_CMD_LOOP_TIMEOUT     10000   /* number of times to poll in a loop before aborting */


#define LPTM21_FLASH_PAGE_SIZE      (16)                        /* < 16 bytes per page in Cfg and UFM sectors */
#define LPTM21_FLASH_PAGES_LEN(n)   (n * CPLD_FLASH_PAGE_SIZE)  /* < Number of bytes in that many pages */

typedef enum
{
	cfg_sector  = 0,   /* configuration sector*/
	ufm_sector  = 1,   /* ufm ("user flash memory') sector */
	asc_eeprom  = 2,   /* asc-EEPROM */
	feature_row = 3,   /* feature row */
	lptm21_sram = 4,   /* LPTM sram */
	golden_spi  = 5,   /* Golden SPI flash */
	all_sectors = 10
} flash_sector_t;

typedef enum
{
	mode_transparent = 0,   /* program in Background, user logic runs while doing it */
    mode_offline     = 1    /* program in Direct mode, user logic halts */
}flash_mode_t;

typedef struct {
        /*chip resource info*/
        int32_t page_size;
        int32_t cfm_pages;
        int32_t ufm_pages;
        int32_t asc_bytes;
        int32_t asc_cfg_bytes;

        /*sector data offset in program file*/
        int32_t ufm_data_offset;
        int32_t asc_data_offset;
        int32_t row_data_offset;
}lptm21_info_t;

int32_t lptm21_open_cfgmode(uint8_t dev, const flash_mode_t mode);
int32_t lptm21_close_cfgmode(uint8_t dev);
int32_t lptm21_erase(uint8_t dev, const flash_sector_t sector);
int32_t lptm21_set_flash_page(uint8_t dev, const flash_sector_t sector, uint16_t page);
int32_t lptm21_read_flash_page(uint8_t dev, const flash_sector_t sector, uint8_t *buf);
int32_t lptm21_write_flash_page(uint8_t dev, const flash_sector_t sector, uint8_t *buf);
int32_t lptm21_get_info(uint8_t dev, uint8_t reg, uint8_t *buf);
int32_t lptm21_set_user_id(uint8_t dev, uint32_t id);
int32_t lptm21_set_feature_row(uint8_t dev, uint8_t *buf);
int32_t lptm21_set_fea_bits(uint8_t dev, uint8_t *buf);
int32_t lptm21_wait_busy(uint8_t dev);
int32_t lptm21_refresh(uint8_t dev);
int32_t lptm21_reset_address(uint8_t dev, const flash_sector_t sector);
int32_t lptm21_set_done(uint8_t dev);
void    lptm21_busnr_set(const uint32_t busnr);
void    lptm21_busnr_reset(void);
void    lptm21_cfgaddr_set(const uint8_t cfgaddr);
void    lptm21_cfgaddr_reset(const uint8_t cfgaddr);

unsigned char  lptm21_cfgaddr_get(void);


#endif /* _LPTM21_H_ */
