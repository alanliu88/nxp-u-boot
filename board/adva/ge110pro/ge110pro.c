// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 * Copyright 2019 NXP
 */

#include <common.h>
#include <clock_legacy.h>
#include <command.h>
#include <fdt_support.h>
#include <i2c.h>
#include <init.h>
#include <net.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/arch/immap_ls102xa.h>
#include <asm/arch/clock.h>
#include <asm/arch/fsl_serdes.h>
#include <asm/arch/ls102xa_devdis.h>
#include <asm/arch/ls102xa_soc.h>
#include <hwconfig.h>
#include <mmc.h>
#include <fsl_csu.h>
#include <fsl_ifc.h>
#include <fsl_immap.h>
#include <netdev.h>
#include <fsl_mdio.h>
#include <tsec.h>
#include <fsl_devdis.h>
#include <spl.h>
#include <linux/delay.h>
//#include "../common/sleep.h"
#ifdef CONFIG_U_QE
#include <fsl_qe.h>
#endif
#include <fsl_validate.h>


DECLARE_GLOBAL_DATA_PTR;

#define VERSION_MASK		0x00FF
#define BANK_MASK		0x0001
#define CONFIG_RESET		0x1
#define INIT_RESET		0x1

#define CPLD_SET_MUX_SERDES	0x20
#define CPLD_SET_BOOT_BANK	0x40

#define BOOT_FROM_UPPER_BANK	0x0
#define BOOT_FROM_LOWER_BANK	0x1

#define LANEB_SATA		(0x01)
#define LANEB_SGMII1		(0x02)
#define LANEC_SGMII1		(0x04)
#define LANEC_PCIEX1		(0x08)
#define LANED_PCIEX2		(0x10)
#define LANED_SGMII2		(0x20)

#define MASK_LANE_B		0x1
#define MASK_LANE_C		0x2
#define MASK_LANE_D		0x4
#define MASK_SGMII		0x8

#define KEEP_STATUS		0x0
#define NEED_RESET		0x1

#define SOFT_MUX_ON_I2C3_IFC	0x2
#define SOFT_MUX_ON_CAN3_USB2	0x8
#define SOFT_MUX_ON_QE_LCD	0x10

int checkboard(void)
{
	puts("Board: LS1021ATWR\n");
#if !defined(CONFIG_QSPI_BOOT) && !defined(CONFIG_SD_BOOT_QSPI)
#endif

	return 0;
}

void ddrmc_init(void)
{
	struct ccsr_ddr *ddr = (struct ccsr_ddr *)CONFIG_SYS_FSL_DDR_ADDR;
	u32 temp_sdram_cfg, tmp;

	out_be32(&ddr->sdram_cfg, DDR_SDRAM_CFG);

	out_be32(&ddr->cs0_bnds, DDR_CS0_BNDS);
	out_be32(&ddr->cs0_config, DDR_CS0_CONFIG);

	out_be32(&ddr->timing_cfg_0, DDR_TIMING_CFG_0);
	out_be32(&ddr->timing_cfg_1, DDR_TIMING_CFG_1);
	out_be32(&ddr->timing_cfg_2, DDR_TIMING_CFG_2);
	out_be32(&ddr->timing_cfg_3, DDR_TIMING_CFG_3);
	out_be32(&ddr->timing_cfg_4, DDR_TIMING_CFG_4);
	out_be32(&ddr->timing_cfg_5, DDR_TIMING_CFG_5);

#ifdef CONFIG_DEEP_SLEEP
	if (is_warm_boot()) {
		out_be32(&ddr->sdram_cfg_2,
			 DDR_SDRAM_CFG_2 & ~SDRAM_CFG2_D_INIT);
		out_be32(&ddr->init_addr, CONFIG_SYS_SDRAM_BASE);
		out_be32(&ddr->init_ext_addr, (1 << 31));

		/* DRAM VRef will not be trained */
		out_be32(&ddr->ddr_cdr2,
			 DDR_DDR_CDR2 & ~DDR_CDR2_VREF_TRAIN_EN);
	} else
#endif
	{
		out_be32(&ddr->sdram_cfg_2, DDR_SDRAM_CFG_2);
		out_be32(&ddr->ddr_cdr2, DDR_DDR_CDR2);
	}

	out_be32(&ddr->sdram_mode, DDR_SDRAM_MODE);
	out_be32(&ddr->sdram_mode_2, DDR_SDRAM_MODE_2);

	out_be32(&ddr->sdram_interval, DDR_SDRAM_INTERVAL);

	out_be32(&ddr->ddr_wrlvl_cntl, DDR_DDR_WRLVL_CNTL);

	out_be32(&ddr->ddr_wrlvl_cntl_2, DDR_DDR_WRLVL_CNTL_2);
	out_be32(&ddr->ddr_wrlvl_cntl_3, DDR_DDR_WRLVL_CNTL_3);

	out_be32(&ddr->ddr_cdr1, DDR_DDR_CDR1);

	out_be32(&ddr->sdram_clk_cntl, DDR_SDRAM_CLK_CNTL);
	out_be32(&ddr->ddr_zq_cntl, DDR_DDR_ZQ_CNTL);

	out_be32(&ddr->cs0_config_2, DDR_CS0_CONFIG_2);

	/* DDR erratum A-009942 */
	tmp = in_be32(&ddr->debug[28]);
	out_be32(&ddr->debug[28], tmp | 0x0070006f);

	udelay(1);

#ifdef CONFIG_DEEP_SLEEP
	if (is_warm_boot()) {
		/* enter self-refresh */
		temp_sdram_cfg = in_be32(&ddr->sdram_cfg_2);
		temp_sdram_cfg |= SDRAM_CFG2_FRC_SR;
		out_be32(&ddr->sdram_cfg_2, temp_sdram_cfg);

		temp_sdram_cfg = (DDR_SDRAM_CFG_MEM_EN | SDRAM_CFG_BI);
	} else
#endif
		temp_sdram_cfg = (DDR_SDRAM_CFG_MEM_EN & ~SDRAM_CFG_BI);

	out_be32(&ddr->sdram_cfg, DDR_SDRAM_CFG | temp_sdram_cfg);

#ifdef CONFIG_DEEP_SLEEP
	if (is_warm_boot()) {
		/* exit self-refresh */
		temp_sdram_cfg = in_be32(&ddr->sdram_cfg_2);
		temp_sdram_cfg &= ~SDRAM_CFG2_FRC_SR;
		out_be32(&ddr->sdram_cfg_2, temp_sdram_cfg);
	}
#endif
}

int dram_init(void)
{
#if (!defined(CONFIG_SPL) || defined(CONFIG_SPL_BUILD))
	ddrmc_init();
#endif

	erratum_a008850_post();

	gd->ram_size = get_ram_size((void *)PHYS_SDRAM, PHYS_SDRAM_SIZE);

#if defined(CONFIG_DEEP_SLEEP) && !defined(CONFIG_SPL_BUILD)
	fsl_dp_resume();
#endif

	return 0;
}

int board_eth_init(struct bd_info *bis)
{
	return pci_eth_init(bis);
}

int board_early_init_f(void)
{
	struct ccsr_scfg *scfg = (struct ccsr_scfg *)CONFIG_SYS_FSL_SCFG_ADDR;

#ifdef CONFIG_TSEC_ENET
	/* clear BD & FR bits for BE BD's and frame data */
	clrbits_be32(&scfg->etsecdmamcr, SCFG_ETSECDMAMCR_LE_BD_FR);
	out_be32(&scfg->etsecmcr, SCFG_ETSECCMCR_GE2_CLK125);
#endif

#ifdef CONFIG_FSL_IFC
	init_early_memctl_regs();
#endif

	arch_soc_init();

#if defined(CONFIG_DEEP_SLEEP)
	if (is_warm_boot()) {
		timer_init();
		dram_init();
	}
#endif

	return 0;
}

#ifdef CONFIG_SPL_BUILD
void board_init_f(ulong dummy)
{
	void (*second_uboot)(void);

	/* Clear the BSS */
	memset(__bss_start, 0, __bss_end - __bss_start);

	get_clocks();

#if defined(CONFIG_DEEP_SLEEP)
	if (is_warm_boot())
		fsl_dp_disable_console();
#endif

	preloader_console_init();

	timer_init();
	dram_init();

	/* Allow OCRAM access permission as R/W */
#ifdef CONFIG_LAYERSCAPE_NS_ACCESS
	enable_layerscape_ns_access();
#endif

	/*
	 * if it is woken up from deep sleep, then jump to second
	 * stage uboot and continue executing without recopying
	 * it from SD since it has already been reserved in memeory
	 * in last boot.
	 */
	if (is_warm_boot()) {
		second_uboot = (void (*)(void))CONFIG_SYS_TEXT_BASE;
		second_uboot();
	}

	board_init_r(NULL, 0);
}
#endif

#ifdef CONFIG_DEEP_SLEEP
/* program the regulator (MC34VR500) to support deep sleep */
void ls1twr_program_regulator(void)
{
	u8 i2c_device_id;

#define LS1TWR_I2C_BUS_MC34VR500	1
#define MC34VR500_ADDR			0x8
#define MC34VR500_DEVICEID		0x4
#define MC34VR500_DEVICEID_MASK		0x0f
#if CONFIG_IS_ENABLED(DM_I2C)
	struct udevice *dev;
	int ret;

	ret = i2c_get_chip_for_busnum(LS1TWR_I2C_BUS_MC34VR500, MC34VR500_ADDR,
				      1, &dev);
	if (ret) {
		printf("%s: Cannot find udev for a bus %d\n", __func__,
		       LS1TWR_I2C_BUS_MC34VR500);
		return;
	}
	i2c_device_id = dm_i2c_reg_read(dev, 0x0) &
					MC34VR500_DEVICEID_MASK;
	if (i2c_device_id != MC34VR500_DEVICEID) {
		printf("The regulator (MC34VR500) does not exist. The device does not support deep sleep.\n");
		return;
	}

	dm_i2c_reg_write(dev, 0x31, 0x4);
	dm_i2c_reg_write(dev, 0x4d, 0x4);
	dm_i2c_reg_write(dev, 0x6d, 0x38);
	dm_i2c_reg_write(dev, 0x6f, 0x37);
	dm_i2c_reg_write(dev, 0x71, 0x30);
#else
	unsigned int i2c_bus;
	i2c_bus = i2c_get_bus_num();
	i2c_set_bus_num(LS1TWR_I2C_BUS_MC34VR500);
	i2c_device_id = i2c_reg_read(MC34VR500_ADDR, 0x0) &
					MC34VR500_DEVICEID_MASK;
	if (i2c_device_id != MC34VR500_DEVICEID) {
		printf("The regulator (MC34VR500) does not exist. The device does not support deep sleep.\n");
		return;
	}

	i2c_reg_write(MC34VR500_ADDR, 0x31, 0x4);
	i2c_reg_write(MC34VR500_ADDR, 0x4d, 0x4);
	i2c_reg_write(MC34VR500_ADDR, 0x6d, 0x38);
	i2c_reg_write(MC34VR500_ADDR, 0x6f, 0x37);
	i2c_reg_write(MC34VR500_ADDR, 0x71, 0x30);

	i2c_set_bus_num(i2c_bus);
#endif
}
#endif

int board_init(void)
{
#ifdef CONFIG_SYS_FSL_ERRATUM_A010315
	erratum_a010315();
#endif

#ifndef CONFIG_SYS_FSL_NO_SERDES
	fsl_serdes_init();
#if !defined(CONFIG_QSPI_BOOT) && !defined(CONFIG_SD_BOOT_QSPI)
#endif
#endif

	ls102xa_smmu_stream_id_init();

#ifdef CONFIG_U_QE
	u_qe_init();
#endif

#ifdef CONFIG_DEEP_SLEEP
	ls1twr_program_regulator();
#endif
	return 0;
}

#if defined(CONFIG_SPL_BUILD)
void spl_board_init(void)
{
	ls102xa_smmu_stream_id_init();
}
#endif

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#ifdef CONFIG_CHAIN_OF_TRUST
	fsl_setenv_chain_of_trust();
#endif

	return 0;
}
#endif

#if defined(CONFIG_MISC_INIT_R)
int misc_init_r(void)
{
#ifdef CONFIG_FSL_DEVICE_DISABLE
	device_disable(devdis_tbl, ARRAY_SIZE(devdis_tbl));
#endif
	return 0;
}
#endif

#if defined(CONFIG_DEEP_SLEEP)
void board_sleep_prepare(void)
{
#ifdef CONFIG_LAYERSCAPE_NS_ACCESS
	enable_layerscape_ns_access();
#endif
}
#endif

int ft_board_setup(void *blob, struct bd_info *bd)
{
	ft_cpu_setup(blob, bd);

#ifdef CONFIG_PCI
	ft_pci_setup(blob, bd);
#endif

	return 0;
}

u8 flash_read8(void *addr)
{
	return __raw_readb(addr + 1);
}

void flash_write16(u16 val, void *addr)
{
	u16 shftval = (((val >> 8) & 0xff) | ((val << 8) & 0xff00));

	__raw_writew(shftval, addr);
}

u16 flash_read16(void *addr)
{
	u16 val = __raw_readw(addr);

	return (((val) >> 8) & 0x00ff) | (((val) << 8) & 0xff00);
}

