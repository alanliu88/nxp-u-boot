/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/fsl_serdes.h>
#ifdef CONFIG_FSL_LS_PPA
#include <asm/arch/ppa.h>
#endif
#include <asm/arch/mmu.h>
#include <asm/arch/soc.h>
#include <hwconfig.h>
#include <ahci.h>
#include <mmc.h>
#include <scsi.h>
#include <fsl_esdhc.h>
#include <environment.h>
#include <fsl_mmdc.h>
#include <netdev.h>
#include <fsl_sec.h>
#include <exports.h>
#include <dm/device.h>
#include <version.h>

#include "../common/cpld.h"
#include "../common/phyinv.h"
#include "../common/board_phyinv.h"

DECLARE_GLOBAL_DATA_PTR;

enum { POWER = 0, WARM = 1, WDOG = 2, ABNORMAL = 3, 
		COLD = 4, MACSEC_SWITCH = 5, BUTTON = 6, DEFAULTDB = 7};

#define SIZEOF_UNITNAME sizeof(((PhyInv_t_page0*)(0))->unitName)
#define SIZEOF_PARTNUM sizeof(((PhyInv_t_page0*)(0))->partNum)

extern void init_board_debug (void);

#define GPIO1_DIR_REG  0x2300000
#define GPIO1_ODR_REG  0x2300004
#define GPIO1_DATA_REG 0x2300008
#define GPIO_RESET_MUX_INN 0x00000800
#define GPIO_RESET_MUX_SFP 0x00000100

#define GPIO_I2C_CLK_R 0x00010000

#define LED_ORANGE_BLINK (CPLD_STATUS_LED_RED|CPLD_STATUS_LED_GREEN |CPLD_STATUS_LED_BLINK)
#define LED_GREEN_BLINK (CPLD_STATUS_LED_GREEN | CPLD_STATUS_LED_BLINK)

/*four channed, channel 0: cpld(0x42), 1: rtc(0x68), 2:TMP1075(), 3:eeprom(0x54)*/
#define I2C_BUS_CPLD  1
#define I2C_BUS_RTC   2
#define I2C_BUS_TMP   3
#define I2C_BUS_INV   4

#define CONFIG_SYS_I2C_CPLD_ADDR 0x42
#define CONFIG_SYS_I2C_INV_ADDR  0x54

struct udevice* get_i2c_inv_dev(int ext)
{
	int ret = 0;
	struct udevice* i2cdev;
	static struct udevice* bus = 0;
	
	int chip_addr = CONFIG_SYS_I2C_INV_ADDR;

	if (ext)
		chip_addr = CONFIG_SYS_I2C_INV_ADDR + 1;
	
	if (!bus) {
		ret = i2c_get_chip_for_busnum(I2C_BUS_INV, chip_addr, 1, &i2cdev);
		if (ret != 0) {
			printf("%s failed to get i2c dev %d:0x%x, ret=%d\n",
				__func__, I2C_BUS_INV, chip_addr, ret);
			return 0;
		}
		bus = i2cdev->parent;
	}else {
		ret = i2c_get_chip(bus, chip_addr, 1, &i2cdev);
		if (ret) {
			debug("Cannot find I2C chip %02x on bus %d\n", chip_addr,
			      I2C_BUS_INV);
			return 0;
		}
	}
	
	return i2cdev;
}

struct udevice* get_i2c_cpld_dev(void)
{
	int ret = 0;
	struct udevice* i2cdev;
	
	ret = i2c_get_chip_for_busnum(I2C_BUS_CPLD, CONFIG_SYS_I2C_CPLD_ADDR, 1, &i2cdev);
	if (ret != 0) {
		printf("%s failed to get i2c dev %d:0x%x, ret=%d\n",
			__func__, I2C_BUS_CPLD, CONFIG_SYS_I2C_CPLD_ADDR,ret);
		return 0;
	}

	return i2cdev;
}

void i2c_busy_recovery(void)
{
	int i = 0;
	unsigned int val;
	unsigned int dirval;

	dirval = readl(GPIO1_DIR_REG);
	val = (dirval | htonl(GPIO_I2C_CLK_R));
	writel(val, GPIO1_DIR_REG);

	val = GPIO_RESET_MUX_INN;

	for (i = 0; i < 10; i++)
	{
		val &= ~GPIO_I2C_CLK_R; 
		writel(htonl(val), GPIO1_DATA_REG);
		udelay(5);
		val |= GPIO_I2C_CLK_R; 		
		writel(htonl(val), GPIO1_DATA_REG);
		udelay(5);
	}

	writel(dirval, GPIO1_DIR_REG);
}

int do_i2c_recovery (struct cmd_tbl * cmdtp, int flag, int argc, char * const argv[])
{
	i2c_busy_recovery();

	return 0;
}

U_BOOT_CMD (ri2c, 1, 1, do_i2c_recovery,"i2c nine clock", "no help");

int checkboard(void)
{
	struct udevice *i2cdev;
	char unitName[SIZEOF_UNITNAME + 1];
#ifdef CONFIG_BOARD_TYPES
	char partNumber[SIZEOF_PARTNUM + 1];
#endif

	i2cdev = get_i2c_inv_dev(0);
	if (i2cdev == 0) {
		return 0;
	}
	
	if ((dm_i2c_read (i2cdev,offsetof (PhyInv_t_page0, unitName),
		(u8*) unitName, SIZEOF_UNITNAME) == 0) &&
	  (unitName[0] != 0xff))
	{
		unitName[SIZEOF_UNITNAME] = '\0';
		printf ("Board: %s\n", unitName);
	}
	else
	{
		printf ("Board: %s\n", "unknown");
	}
	
#ifdef CONFIG_BOARD_TYPES
	if ((dm_i2c_read(i2cdev, offsetof (PhyInv_t_page0, partNum),
		(u8*) partNumber, SIZEOF_PARTNUM) == 0) &&
	  (partNumber[0] != 0xff))
	{
		if (strncmp(partNumber, "1078904920", 10) == 0)
		{
			//gd->board_type = ADVA_BOARD_GE104;
		}
		else
		{
			gd->board_type = 0;
		}
		
		partNumber[SIZEOF_PARTNUM] = '\0';
		printf ("PartNumber: %s\n", partNumber);
	}
	else
	{
		printf ("PartNumber: %s\n", "unknown");
	}
#endif

	return 0;
}


int dram_init(void)
{
	static const struct fsl_mmdc_info mparam = {
		0x04180000,	/* mdctl */
		0x00030035,	/* mdpdc */
		0x12554000,	/* mdotc */
		0xbabf7954,	/* mdcfg0 */
		0xdb328f64,	/* mdcfg1 */
		0x01ff00db,	/* mdcfg2 */
		0x00001680,	/* mdmisc */
		0x0f3c8000,	/* mdref */
		0x00002000,	/* mdrwd */
		0x00bf1023,	/* mdor */
		0x0000003f,	/* mdasp */
		0x0000022a,	/* mpodtctrl */
		0xa1390003,	/* mpzqhwctrl */
	};

	mmdc_init(&mparam);

	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
#if !defined(CONFIG_SPL) || defined(CONFIG_SPL_BUILD)
	/* This will break-before-make MMU for DDR */
	update_early_mmu_table();
#endif

	return 0;
}

int restart_cause_init(void)
{
	struct udevice *i2cdev;
	uint8_t val=0;
	uint8_t scratch;
	uint8_t led = LED_ORANGE_BLINK;
	
	i2cdev = get_i2c_cpld_dev();
	if (i2cdev == 0) {
		return -1;
	}
	
	/* Set blinking yellow status LED */
	dm_i2c_write(i2cdev, CPLD_STATUS_LED_REG, &led, 1);

	if (dm_i2c_read (i2cdev,CPLD_SCRATCH0_REG, &scratch, 1) != 0) 
		printf("read scratch register failed\n");
	else {
		/* Determine reset cause and set cpld scratch register */
		uint8_t cause = 0;

		if ((scratch > DEFAULTDB) && (scratch != 0xa5)) {
			cause = ABNORMAL;
		}else if (scratch == 0xa5)	{
			cause = WARM;
		}else
			cause = scratch;
		
		/* update the cpld scratch register */
		dm_i2c_reg_write (i2cdev, CPLD_SCRATCH0_REG, cause);

		val = dm_i2c_reg_read(i2cdev, CPLD_RESET0_CNTRL_REG);
		dm_i2c_reg_write(i2cdev, CPLD_RESET0_CNTRL_REG, (val & 0xd4));
	}

	return 0;
}



int board_early_init_f(void)
{
	fsl_lsch2_early_init_f();

	return 0;
}

int board_init(void)
{
	struct ccsr_cci400 *cci = (struct ccsr_cci400 *)(CONFIG_SYS_IMMR +
					CONFIG_SYS_CCI400_OFFSET);
	/*
	 * Set CCI-400 control override register to enable barrier
	 * transaction
	 */	 
	if (current_el() == 3)
		out_le32(&cci->ctrl_ord, CCI400_CTRLORD_EN_BARRIER);

#ifdef CONFIG_SYS_FSL_ERRATUM_A010315
	erratum_a010315();
#endif

#ifdef CONFIG_ENV_IS_NOWHERE
	gd->env_addr = (ulong)&default_environment[0];
#endif

#ifdef CONFIG_FSL_CAAM
	sec_init();
#endif

#ifdef CONFIG_FSL_LS_PPA
	ppa_init();
#endif

	restart_cause_init();
	
	return 0;
}

void adva_board_setup (void *fdt)
{
	struct udevice *i2cdev;
	uchar lStatusLed = LED_GREEN_BLINK;
	
	i2cdev = get_i2c_cpld_dev();
	if (i2cdev == 0) {
		return;
	}

	dm_i2c_reg_write(i2cdev, CPLD_STATUS_LED_REG, lStatusLed);
}

void ft_uboot_version(void *fdt)
{
	int nodeoffset = fdt_path_offset(fdt, "/");
	
	fdt_setprop(fdt, nodeoffset, "uboot_version", U_BOOT_VERSION_STRING, strlen(U_BOOT_VERSION_STRING)+1);
}

int ft_board_setup(void *blob, bd_t *bd)
{
	arch_fixup_fdt(blob);

	ft_cpu_setup(blob, bd);

	ft_uboot_version(blob);

	adva_board_setup(bd);

	return 0;
}

int board_reboot(int flag)
{
	struct udevice* i2cdev;
	uint8_t scratch = 0;
	uint8_t val = 0;
	
	udelay (50000);				/* wait 50 ms */

	disable_interrupts();

	i2cdev = get_i2c_cpld_dev();
	if (i2cdev == 0) {
		return -1;
	}

	if (flag == 1)
	{
		scratch = COLD;
		val = (CPLD_CARD_RESET_CNTRL_ENA | CPLD_CARD_RESET_CNTRL_COLD);
	}
	else if (flag == 0)
	{
		scratch = POWER;
		val = (CPLD_CARD_RESET_CNTRL_ENA | CPLD_CARD_RESET_CNTRL_POR);
	}
	
	dm_i2c_reg_write(i2cdev, CPLD_SCRATCH0_REG, scratch);
	
	udelay (50000);				/* wait 50 ms */
	
	dm_i2c_reg_write(i2cdev, CPLD_CARD_RESET_CNTRL_REG, val);
	
	udelay (2000000);				/* wait 2s */
	
	reset_cpu(1); /*cold reset cpu*/

	/*NOTREACHED*/
	return 0;
}

int last_stage_init (void)
{
	struct udevice* i2cdev;
	uint8_t lamptest = 1;
	uint8_t scratch = 0;
	
	i2cdev = get_i2c_cpld_dev();
	if (i2cdev == 0) {
		return -1;
	}

	/* Check for power-on reset */
	
	if (dm_i2c_read (i2cdev, CPLD_SCRATCH0_REG, &scratch, 1) != 0) {
		debug("last_stage_init: read cpld scratch reg failed\n");
		return 0;
	}
	
	if (scratch == POWER) {
		/* Set blinking yellow status LED */
		if (dm_i2c_write (i2cdev, CPLD_LAMP_TEST_REG, &lamptest, 1) != 0) {
			debug("last_stage_init: set lamptest register failed\n");
		}
		
		udelay (400000000);

		lamptest = 0;
		if (dm_i2c_write (i2cdev, CPLD_LAMP_TEST_REG, &lamptest, 1) != 0) {
			debug("last_stage_init: clear lamptest register failed\n");
		}
	}
	
	return 0;
}

static void setenv_ethaddr_eeprom(const char* ethaddr)
{
	uchar enetaddr[6];
	uchar* macAddrPtr;
	/* configuration based on dev kit EEPROM */
	
	macAddrPtr = phyinv_base_mac();
	memcpy(enetaddr, macAddrPtr, 6);
	
	if (is_valid_ethaddr(enetaddr))
		eth_env_set_enetaddr(ethaddr, enetaddr);
	else
		puts("Skipped ethaddr assignment due to invalid "
			"EMAC address in EEPROM\n");
}

#ifdef CONFIG_BOARD_LATE_INIT
int adva_board_late_init(void)
{
	uchar enetaddr[6];

	env_set_addr("setenv_ethaddr_eeprom", (void *)setenv_ethaddr_eeprom);

	/* if no ethaddr environment, get it from EEPROM */
	if (!eth_env_get_enetaddr("eth0addr", enetaddr))
		setenv_ethaddr_eeprom("eth0addr");

	if (!eth_env_get_enetaddr("eth1addr", enetaddr))
		setenv_ethaddr_eeprom("eth1addr");

#if defined(CONFIG_DEBUG_CMD)
	/* Initialize debug mode from environment setting */
	init_board_debug();
#endif
	
	return 0;
}
#endif

