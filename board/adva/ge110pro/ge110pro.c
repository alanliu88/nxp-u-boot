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
#ifdef CONFIG_U_QE
#include <fsl_qe.h>
#endif
#include <fsl_validate.h>
#include <dm/device.h>
#include <spi.h>
#include <miiphy.h>
#include <fdt_support.h>

#include "../common/cpld.h"
#include "../common/phyinv.h"
#include "../common/board_phyinv.h"

DECLARE_GLOBAL_DATA_PTR;

enum { POWER = 0, WARM = 1, WDOG = 2, ABNORMAL = 3, 
		COLD = 4, MACSEC_SWITCH = 5, BUTTON = 6, DEFAULTDB = 7};


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

/******************************************************************************
 * Display the board name
 * GE114Pro, GE114Pro MACsec, GE114SHPro, GE114SHPro MACsec, GE112Pro, Mini CPE, etc
 *****************************************************************************/
#define SIZEOF_UNITNAME sizeof(((PhyInv_t_page0*)(0))->unitName)
#define SIZEOF_PARTNUM sizeof(((PhyInv_t_page0*)(0))->partNum)
int checkboard(void)
{
    struct udevice *i2cdev;
    char unitName[SIZEOF_UNITNAME + 1];
#ifdef CONFIG_BOARD_TYPES
    char partNumber[SIZEOF_PARTNUM + 1];
#endif
    uint offset;
    char testEndian[]="\x11\x22\x33\x44";

    printf("%s Endian Mode\n",(((*((unsigned long*)testEndian))==0x11223344)?"Big":"Little"));
    printf("TestEndian is %lx [%x][%x][%x][%x]\n",*((unsigned long*)testEndian),testEndian[0],testEndian[1],testEndian[2],testEndian[3]);

    i2cdev = get_i2c_inv_dev(0);
    if (i2cdev == 0) {
        return 0;
    }

    offset = offsetof (PhyInv_t_page0, unitName);
    if ((dm_i2c_read (i2cdev,offset,(u8*) unitName, SIZEOF_UNITNAME) == 0) &&
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
    offset = offsetof (PhyInv_t_page0, partNum);
    if ((dm_i2c_read(i2cdev, offset, (u8*)partNumber, SIZEOF_PARTNUM) == 0) && 
        (partNumber[0] != 0xff))
    {
        if ((strncmp(partNumber, "1078904720", 10) == 0) ||
            (strncmp(partNumber, "1078904721", 10) == 0))
        {
            gd->board_type = ADVA_BOARD_GE114_PRO;
        }
        else if((strncmp(partNumber, "1078904722", 10) == 0) ||
                (strncmp(partNumber, "1078904723", 10) == 0))
        {
            gd->board_type = ADVA_BOARD_GE114_PRO_MACSEC;
        }
        else if(strncmp(partNumber, "1078904724", 10) == 0)
        {
            gd->board_type = ADVA_BOARD_GE114SH_PRO;
        }
        else if(strncmp(partNumber, "1078904725", 10) == 0)
        {
            gd->board_type = ADVA_BOARD_GE114SH_PRO_MACSEC;
        }
        else if((strncmp(partNumber, "1078904701", 10) == 0) ||
                (strncmp(partNumber, "1078904702", 10) == 0) ||
                (strncmp(partNumber, "1078904703", 10) == 0) ||
                (strncmp(partNumber, "1078904705", 10) == 0) ||
                (strncmp(partNumber, "1078904840", 10) == 0))
        {
            gd->board_type = ADVA_BOARD_GE112_PRO;
        }
        else if((strncmp(partNumber, "1013904704", 10) == 0) ||
                    (strncmp(partNumber, "1078904704", 10) == 0))
        {
            gd->board_type = ADVA_BOARD_MINI_CPE;
        }
        else if((strncmp(partNumber, "1078904754", 10) == 0) ||
                (strncmp(partNumber, "1078904755", 10) == 0))
        {
            gd->board_type = ADVA_BOARD_GE114CSH_PRO_VM;
        }
        else if(strncmp(partNumber, "1078904757", 10) == 0)
        {
            gd->board_type = ADVA_BOARD_GE114SH_PRO_VM;
        }
        else if((strncmp(partNumber, "1078904752", 10) == 0) ||
                (strncmp(partNumber, "1078904753", 10) == 0))
        {
            gd->board_type = ADVA_BOARD_GE114CH_PRO_VM;
        }
        else if((strncmp(partNumber, "1078904750", 10) == 0) ||
                (strncmp(partNumber, "1078904751", 10) == 0))
        {
            gd->board_type = ADVA_BOARD_GE114H_PRO_VM;
        }
        else if ((strncmp(partNumber, "1078904850", 10) == 0) ||
                 (strncmp(partNumber, "1078904851", 10) == 0) ||
                 (strncmp(partNumber, "1078904858", 10) == 0) ||
                 (strncmp(partNumber, "1078904859", 10) == 0))
        {
            gd->board_type = ADVA_BOARD_GE112_PRO_VM;
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
	//ddrmc_init();
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

	if (((gd->board_type == ADVA_BOARD_GE114SH_PRO) ||
	    (gd->board_type == ADVA_BOARD_GE114SH_PRO_MACSEC) ||
	    (gd->board_type == ADVA_BOARD_GE114CSH_PRO_VM)) &&
	    (( CPLD_READ(CPLD_SCRATCH0_REG) == POWER) ||
		 ( CPLD_READ(CPLD_SCRATCH0_REG) == COLD) ||
		 ( CPLD_READ(CPLD_SCRATCH0_REG) == MACSEC_SWITCH))) 
	{
		
		extern int setup_ds31407 (void);
		setup_ds31407();
	}

	return 0;
}
#endif
#if defined(CONFIG_LAST_STAGE_INIT)
int last_stage_init (void)
{
	if ((gd->board_type == ADVA_BOARD_GE114CSH_PRO_VM)  ||
	    (gd->board_type == ADVA_BOARD_GE114CH_PRO_VM)   ||
	    (gd->board_type == ADVA_BOARD_GE114H_PRO_VM)    ||
	    (gd->board_type == ADVA_BOARD_GE114SH_PRO_VM)   ||
	    (gd->board_type == ADVA_BOARD_GE112_PRO_VM) ) {
		if (CPLD_READ(CPLD_SCRATCH0_REG) != 1) {
			printf("SWITCH [RESET]\n"); 

			/* Reset and Take Out of Reset the L2Switch */
			CPLD_WRITE (CPLD_RESET1_CNTRL_REG, 0xff);
			udelay (11000);
			CPLD_WRITE (CPLD_RESET1_CNTRL_REG, 0xbf);
			udelay (2000000);

			/* Configure L2Switch to CPU interface */
#if defined(DEFAULT_MII_NAME)
			static char *devname = DEFAULT_MII_NAME;
#else
			static char *devname = CONFIG_TSEC1_NAME;
#endif
			// Write data - value
			if (miiphy_write(devname, 0x10, 0x1, 0xc03d) != 0)			{
			    printf("Error: unable to write L2Switch(0x10) SMI reg(0x1) Value(0xc003) \n");
			}
			
			// Write address - 4:0 RegAddr, 9:5 DevAddr, 11:10 SMIOp  
			if (miiphy_write(devname, 0x10, 0x0, 0x96c1) != 0)			{
			    printf("Error: unable to write L2Switch(0x10) SMI reg(0x0) Value(0x96c1) \n");
			}
			printf("SWITCH [READY]\n");
		}

		/* Configure CPU ETH interface*/
		writel(0x7105, 0x2d10504);
		writel(0x1058, 0x2d10020);

		printf("eTSEC1 [READY]\n");

	}
		
	return 0;
}
#endif


#if defined(CONFIG_MISC_INIT_R)
int misc_init_r(void)
{
    uint8_t *macAddrPtr;

#ifdef CONFIG_FSL_DEVICE_DISABLE
	device_disable(devdis_tbl, ARRAY_SIZE(devdis_tbl));
#endif
    if ((gd->board_type == ADVA_BOARD_GE114CSH_PRO_VM) ||
        (gd->board_type == ADVA_BOARD_GE114CH_PRO_VM)  ||
        (gd->board_type == ADVA_BOARD_GE114H_PRO_VM)   ||
        (gd->board_type == ADVA_BOARD_GE114SH_PRO_VM)  ||
        (gd->board_type == ADVA_BOARD_GE112_PRO_VM) )
    {
        if (env_set("VM_PLATFORM", "1") != 0)
        {
            puts("Error setting VM_PLATFORM\n");
        }
    }   
  
    /* Set MAC addresses used by Linux.  The values from the ethaddr and
    * eth1addr environment variables will be copied into the device tree.
    */
    macAddrPtr = phyinv_base_mac();
    if (macAddrPtr)
    {   
    	char macStr[20];

    	sprintf(macStr, "%02x:%02x:%02x:%02x:%02x:%02x",
    		macAddrPtr[0], macAddrPtr[1], macAddrPtr[2],
    		macAddrPtr[3], macAddrPtr[4], macAddrPtr[5]);


    	/* Set eth0 mac address */
    	if (env_set("ethaddr", macStr) != 0)
    	{
    		puts("Error setting ethaddr\n");
    	}

    	/*
    	* The eth1 interface is connected to the FPGA and doesn't need an
    	* externally visible MAC address.  Set the locally administered bit
    	* and leave the rest 00's.
    	*/
    	if (env_set("eth1addr", "02:00:00:00:00:00") != 0)
    	{
    		puts("Error setting ethaddr\n");
    	}
    }

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

/******************************************************************************
 * Initialize ds31407 clock chip
 *****************************************************************************/
 
#define CPLD_SET_BITS(reg, bits) \
	 ((volatile unsigned char *)CONFIG_SYS_CPLD_BASE)[reg] |= (bits)
#define CPLD_CLR_BITS(reg, bits) \
	 ((volatile unsigned char *)CONFIG_SYS_CPLD_BASE)[reg] &= ~(bits)
int cpu_dspi_claim_bus(uint bus, uint cs)
{
	CPLD_SET_BITS (0x14, (1 << cs));
	return 0;
}

void cpu_dspi_release_bus(uint bus, uint cs)
{
	CPLD_CLR_BITS (0x14, (1 << cs));
}

static struct {
	unsigned int reg;
	unsigned char val;
} ds31407_init_table[] = {
#include "ds31407.inc"
	{ 0xffff, 0xff }
};

int setup_ds31407 (void)
{
	struct udevice *spidev;
	struct spi_slave *slave;
	unsigned int bus = 1;
	unsigned int cs = 0;
	unsigned int mode = SPI_MODE_0;
	int ret = -1;
	int tmp;
	int i;
	unsigned char dout[3];
	unsigned char din[3];

	ret = 0;
	dout[0] = 0x80; // read
	dout[1] = 0x00; // single
		
	/* Reset the chip */
	CPLD_SET_BITS (CPLD_RESET0_CNTRL_REG, CPLD_RESET0_CNTRL_DS310X);
	udelay (1);
	CPLD_CLR_BITS (CPLD_RESET0_CNTRL_REG, CPLD_RESET0_CNTRL_DS310X);
	udelay (150);

	ret = spi_get_bus_and_cs(bus, cs, 1000000, mode, "spi_generic_drv", "spidev@0", &spidev, &slave);
	if (ret)
	{
		printf("spi get bus %d cs %d failed!\n", bus, cs);
		return ret;
	}
	ret = spi_claim_bus(slave);
	if (ret)
	{
		printf("claim spi bus failed!\n");
		return 0;
	}
	
	tmp = spi_xfer(slave, 24, dout, din, SPI_XFER_BEGIN | SPI_XFER_END);
	if (tmp) {
		ret = -EIO;
	}else if ((din[2] != 0xaf)) {
		printf ("spi_xfer data in:%x %x %x, hope 0xaf !!\n... ", din[0],din[1],din[2]);
		ret = -ENOENT;
	}
	spi_release_bus(slave);

	for (i = 0; !ret && (ds31407_init_table[i].reg != 0xFFFF); i++)
	{
		dout[0] = (ds31407_init_table[i].reg >> 7) & 0x7F;
		dout[1] = (ds31407_init_table[i].reg << 1) & 0xFE;
		dout[2] = ds31407_init_table[i].val;

		spi_claim_bus(slave);

		if (spi_xfer(slave, 24, dout, NULL, SPI_XFER_BEGIN | SPI_XFER_END) != 0)
		{
			ret = -1;
		}

		udelay(100);
		spi_release_bus(slave);
	}

	return ret;
}

/******************************************************************************
 * Flat Device Tree setup for the board.
 * Called when prepairing to boot Linux kernel for patching the device tree.
 *****************************************************************************/
#if defined(CONFIG_OF_BOARD_SETUP)
void ft_fix_spi(void *fdt)
{
	int node;
	const char *dspi_path, *sspi_path;

	node = fdt_path_offset(fdt, "/aliases");
	if (node < 0)
		return;

	dspi_path = fdt_getprop(fdt, node, "dspi", NULL);
	if (!dspi_path) {
		debug("No alias for %s\n", "dspi");
		return;
	}
	
	sspi_path = fdt_getprop(fdt, node, "sspi", NULL);
	if (!sspi_path) {
		debug("No alias for %s\n", "sspi");
		return;
	}

	do_fixup_by_path(fdt, dspi_path, "status", "disabled", 9, 0);
	do_fixup_by_path(fdt, sspi_path, "status", "okay", 5,0);

}

int ft_board_setup (void *fdt, struct bd_info *bd)
{
	unsigned char np_fixed_link[] = {0,0,0,2,0,0,0,1,0,0,0x03,0xe8,0,0,0,0,0,0,0,0};
	ft_cpu_setup(fdt, bd);

	/*
	* Add console baudrate to bootargs if not specified
	*/
	int nodeoffset = fdt_path_offset(fdt, "/chosen");
	if (nodeoffset >= 0)
	{
		const char* path;
		const char* substr;

		path = fdt_getprop(fdt, nodeoffset, "bootargs", NULL);
		if ((path != NULL) && ((substr = strstr(path, "console=ttyS0"))))
		{
			substr += 13; /* length of "console=ttyO0" */
			if (*substr != ',')
			{
				size_t len;
				char* newpath;
				uint32_t baud;
				const char* baudstr;
				int err;

				baudstr = env_get("baudrate");
				baud = (baudstr) ? simple_strtol(baudstr, NULL, 10) : 9600;
				if ((baud < 300) || (baud > 115200))
				{
					baud = 9600;
				}

				len = strlen(path);
				newpath = malloc(len + 25);

				strcpy(newpath, path);
				sprintf(newpath + (substr - path), ",%dn8", baud);
				if (*substr)
				{
					strcat(newpath, substr);
				}

				err = fdt_setprop(fdt, nodeoffset, "bootargs", newpath, strlen(newpath)+1);
				if (err < 0)
				{
					printf("WARNING: could not set bootargs %s.\n", fdt_strerror(err));
				}

				free(newpath);
			}
		}
	}
	if ((gd->board_type == ADVA_BOARD_GE112_PRO) ||
          (gd->board_type == ADVA_BOARD_MINI_CPE))
	{
		do_fixup_by_compat(fdt, "fsl,rt-etsec2", "fixed-link",&np_fixed_link,20,0);
	}

	if ((gd->board_type == ADVA_BOARD_GE114SH_PRO) ||
	    (gd->board_type == ADVA_BOARD_GE114SH_PRO_MACSEC) ||
	    (gd->board_type == ADVA_BOARD_GE114_PRO_MACSEC) ||
	    (gd->board_type == ADVA_BOARD_GE114CSH_PRO_VM) ||
	    (gd->board_type == ADVA_BOARD_GE114CH_PRO_VM))
	{
		struct ccsr_gur __iomem *gur = (void *)(CONFIG_SYS_FSL_GUTS_ADDR);
		unsigned int svr;

		svr = in_be32(&gur->svr);
		if ((svr & 0x000000ff) <= 0x10) 
		{
			ft_fix_spi(fdt);
		}
	}

    if( (gd->board_type == ADVA_BOARD_GE114CSH_PRO_VM) ||
        (gd->board_type == ADVA_BOARD_GE114CH_PRO_VM) ||
        (gd->board_type == ADVA_BOARD_GE114H_PRO_VM) ||
        (gd->board_type == ADVA_BOARD_GE114SH_PRO_VM) ||
        (gd->board_type == ADVA_BOARD_GE112_PRO_VM))
    {
        int nodeoffset = fdt_path_offset(fdt, "/");
        fdt_setprop(fdt, nodeoffset, "model", "LS102XGE110PROVME", 18);
    }

	/* Now about to boot the kernel so set status LED blinking green. */
	CPLD_CLRSET_BITS (CPLD_STATUS_LED_REG, CPLD_STATUS_LED_RED,
		CPLD_STATUS_LED_GREEN | CPLD_STATUS_LED_BLINK);

	return 0;
}
#endif

void reset_phy(void)
{
    if ((gd->board_type != ADVA_BOARD_GE114CSH_PRO_VM) &&
        (gd->board_type != ADVA_BOARD_GE114CH_PRO_VM) &&
        (gd->board_type != ADVA_BOARD_GE114H_PRO_VM) &&
        (gd->board_type != ADVA_BOARD_GE114SH_PRO_VM) &&
        (gd->board_type != ADVA_BOARD_GE112_PRO_VM) )
    {
        /* Reset the chip */
        CPLD_SET_BITS (CPLD_RESET1_CNTRL_REG, CPLD_RESET1_CNTRL_PHY);
        udelay (1);
        CPLD_CLR_BITS (CPLD_RESET1_CNTRL_REG, CPLD_RESET1_CNTRL_PHY);
        udelay (10);
    }
}

void _reset_cpu(ulong addr)
{
    CPLD_WRITE(0x30, 9);
}

void  board_detail(void)
{
    printf("%-12s= 0x%08x\n", "nor base", CONFIG_SYS_FLASH_BASE);
    printf("%-12s= 0x%08x\n", "nand base", CONFIG_SYS_NAND_BASE);
    printf("%-12s= 0x%08x\n", "fpga base",CONFIG_SYS_FPGA_BASE);
    printf("%-12s= 0x%08x\n", "cpld base",CONFIG_SYS_CPLD_BASE);
}

int board_reboot(int flag)
{
	if (flag == 1)
	{
  		CPLD_WRITE(CPLD_CARD_RESET_CNTRL_REG,
			CPLD_CARD_RESET_CNTRL_COLD);
		udelay(200);
		CPLD_WRITE(CPLD_CARD_RESET_CNTRL_REG,
			CPLD_CARD_RESET_CNTRL_COLD | CPLD_CARD_RESET_CNTRL_ENA);
		udelay(5000000);
 	}
	else
	{
 		CPLD_WRITE(CPLD_CARD_RESET_CNTRL_REG,
			CPLD_CARD_RESET_CNTRL_POR);
		udelay(200);
		CPLD_WRITE(CPLD_CARD_RESET_CNTRL_REG,
			CPLD_CARD_RESET_CNTRL_POR | CPLD_CARD_RESET_CNTRL_ENA);
		udelay(5000000);
	}

	/*NOTREACHED*/
	return 0;
}


