/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

//#define CONFIG_ARMV7_PSCI_1_0

#define CONFIG_ARMV7_SECURE_BASE	OCRAM_BASE_S_ADDR

#define CONFIG_SYS_FSL_CLK

#define CONFIG_SKIP_LOWLEVEL_INIT
/*#define CONFIG_DEEP_SLEEP*/

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 16 * 1024 * 1024)

#define CONFIG_SYS_INIT_RAM_ADDR	OCRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	OCRAM_SIZE

#define CONFIG_SYS_CLK_FREQ		100000000
#define CONFIG_DDR_CLK_FREQ		100000000

#define DDR_SDRAM_CFG			0x470c0008
#define DDR_CS0_BNDS			0x008000bf
#define DDR_CS0_CONFIG			0x80014202
#define DDR_TIMING_CFG_0		0x50550004
#define DDR_TIMING_CFG_1		0xbcb38c56
#define DDR_TIMING_CFG_2		0x0040d120
#define DDR_TIMING_CFG_3		0x010e1000
#define DDR_TIMING_CFG_4		0x00000001
#define DDR_TIMING_CFG_5		0x03401400
#define DDR_SDRAM_CFG_2			0x00401010
#define DDR_SDRAM_MODE			0x00061c60
#define DDR_SDRAM_MODE_2		0x00180000
#define DDR_SDRAM_INTERVAL		0x18600618
#define DDR_DDR_WRLVL_CNTL		0x8655f605
#define DDR_DDR_WRLVL_CNTL_2		0x05060607
#define DDR_DDR_WRLVL_CNTL_3		0x05050505
#define DDR_DDR_CDR1			0x80040000
#define DDR_DDR_CDR2			0x00000001
#define DDR_SDRAM_CLK_CNTL		0x02000000
#define DDR_DDR_ZQ_CNTL			0x89080600
#define DDR_CS0_CONFIG_2		0
#define DDR_SDRAM_CFG_MEM_EN		0x80000000
#define SDRAM_CFG2_D_INIT		0x00000010
#define DDR_CDR2_VREF_TRAIN_EN		0x00000080
#define SDRAM_CFG2_FRC_SR		0x80000000
#define SDRAM_CFG_BI			0x00000001

#ifdef CONFIG_RAMBOOT_PBL
#define CONFIG_SYS_FSL_PBL_PBI	board/ADVA/ge110pro/ls102xa_pbi.cfg
#endif


/*define card name for ge110*/
#define CONFIG_BOARD_TYPES				1
#define ADVA_BOARD_GE112				1
#define ADVA_BOARD_GE114				2
#define ADVA_BOARD_GE114S				3
#define ADVA_BOARD_T1804				4
#define ADVA_BOARD_T3204				5
#define ADVA_BOARD_GE114PH				6
#define ADVA_BOARD_GE114_PRO			7
#define ADVA_BOARD_GE114_PRO_MACSEC		8
#define ADVA_BOARD_GE114SH_PRO			9
#define ADVA_BOARD_GE114SH_PRO_MACSEC	10
#define ADVA_BOARD_GE112_PRO			11
#define ADVA_BOARD_MINI_CPE				12
#define ADVA_BOARD_GE114CSH_PRO_VM		13
#define ADVA_BOARD_GE114CH_PRO_VM		14
#define ADVA_BOARD_GE114H_PRO_VM		15
#define ADVA_BOARD_GE112_PRO_VM			16
#define ADVA_BOARD_GE114SH_PRO_VM		17

#define PHYS_SDRAM			0x80000000
#define PHYS_SDRAM_SIZE			(512 * 1024 * 1024)

#define CONFIG_SYS_DDR_SDRAM_BASE      0x80000000UL
#define CONFIG_SYS_SDRAM_BASE          CONFIG_SYS_DDR_SDRAM_BASE

#if !defined(CONFIG_SD_BOOT) && !defined(CONFIG_NAND_BOOT) && \
	!defined(CONFIG_QSPI_BOOT)
#define CONFIG_SYS_QE_FMAN_FW_IN_NOR
#endif

#define CONFIG_CHIP_SELECTS_PER_CTRL	4

/*
 * IFC Definitions
 */
#if !defined(CONFIG_QSPI_BOOT) && !defined(CONFIG_SD_BOOT_QSPI)
#define CONFIG_FSL_IFC
#define CONFIG_SYS_FLASH_BASE		0x60000000
#define CONFIG_SYS_FLASH_BASE_PHYS	CONFIG_SYS_FLASH_BASE

#define CONFIG_SYS_NOR0_CSPR_EXT	(0x0)
#define CONFIG_SYS_NOR0_CSPR	(CSPR_PHYS_ADDR(CONFIG_SYS_FLASH_BASE_PHYS) | \
				CSPR_PORT_SIZE_16 | \
				CSPR_MSEL_NOR | \
				CSPR_V)
#define CONFIG_SYS_NOR_AMASK		IFC_AMASK(8 * 1024 * 1024)

/* NOR Flash Timing Params */
#define CONFIG_SYS_NOR_CSOR		(CSOR_NOR_ADM_SHIFT(4) | \
					CSOR_NOR_TRHZ_80)
#define CONFIG_SYS_NOR_FTIM0		(FTIM0_NOR_TACSE(0x4) | \
					FTIM0_NOR_TEADC(0x5) | \
					FTIM0_NOR_TAVDS(0x0) | \
					FTIM0_NOR_TEAHC(0x5))
#define CONFIG_SYS_NOR_FTIM1		(FTIM1_NOR_TACO(0x35) | \
					FTIM1_NOR_TRAD_NOR(0x1A) | \
					FTIM1_NOR_TSEQRAD_NOR(0x13))
#define CONFIG_SYS_NOR_FTIM2		(FTIM2_NOR_TCS(0x4) | \
					FTIM2_NOR_TCH(0x4) | \
					FTIM2_NOR_TWP(0x1c) | \
					FTIM2_NOR_TWPH(0x0e))
#define CONFIG_SYS_NOR_FTIM3		0

#define CONFIG_SYS_FLASH_QUIET_TEST
#define CONFIG_FLASH_SHOW_PROGRESS	45	/* count down from 45/5: 9..1 */

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	1024	/* sectors per device */
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms) */

#define CONFIG_SYS_FLASH_EMPTY_INFO
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE_PHYS }

#if defined(__LITTLE_ENDIAN)
#define CONFIG_CFI_FLASH_USE_WEAK_ACCESSORS
#define CONFIG_SYS_WRITE_SWAPPED_DATA
#endif

#endif

/* CS1 */
/* NAND Flash on IFC */
#define CONFIG_NAND_FSL_IFC
#define CONFIG_SYS_NAND_BASE		0x68000000
#define CONFIG_SYS_NAND_BASE_PHYS	CONFIG_SYS_NAND_BASE

#define CONFIG_SYS_NAND_CSPR_EXT	(0x0)
#define CONFIG_SYS_NAND_CSPR	(CSPR_PHYS_ADDR(CONFIG_SYS_NAND_BASE_PHYS) \
				| CSPR_PORT_SIZE_8	\
				| CSPR_MSEL_NAND	\
				| CSPR_V)
#define CONFIG_SYS_NAND_AMASK	IFC_AMASK(64*1024)
#define CONFIG_SYS_NAND_CSOR	(CSOR_NAND_ECC_ENC_EN	/* ECC on encode */ \
				| CSOR_NAND_ECC_DEC_EN	/* ECC on decode */ \
				| CSOR_NAND_ECC_MODE_4	/* 4-bit ECC */ \
				| CSOR_NAND_RAL_3	/* RAL = 3 Bytes */ \
				| CSOR_NAND_PGS_2K	/* Page Size = 2K */ \
				| CSOR_NAND_SPRZ_64	/* Spare size = 64 */ \
				| CSOR_NAND_PB(64))	/* 64 Pages Per Block */

#define CONFIG_SYS_NAND_ONFI_DETECTION

#define CONFIG_SYS_NAND_FTIM0		(FTIM0_NAND_TCCST(0x7) | \
					FTIM0_NAND_TWP(0x18)   | \
					FTIM0_NAND_TWCHT(0x7) | \
					FTIM0_NAND_TWH(0xa))
#define CONFIG_SYS_NAND_FTIM1		(FTIM1_NAND_TADLE(0x32) | \
					FTIM1_NAND_TWBE(0x39)  | \
					FTIM1_NAND_TRR(0xe)   | \
					FTIM1_NAND_TRP(0x18))
#define CONFIG_SYS_NAND_FTIM2		(FTIM2_NAND_TRAD(0xf) | \
					FTIM2_NAND_TREH(0xa) | \
					FTIM2_NAND_TWHRE(0x1e))
#define CONFIG_SYS_NAND_FTIM3           0x0

#define CONFIG_SYS_NAND_BASE_LIST	{CONFIG_SYS_NAND_BASE}
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_MTD_NAND_VERIFY_WRITE
#define CONFIG_CMD_NAND
#define CONFIG_CMD_NAND_TRIMFFS

#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)


/* CS2 */
/* FPGA */

#define CONFIG_SYS_FPGA_BASE	0x70000000
#define FPGA_BASE_PHYS	CONFIG_SYS_FPGA_BASE

#define CONFIG_SYS_FPGA_CSPR_EXT        (0x0)
#define CONFIG_SYS_FPGA_CSPR		(CSPR_PHYS_ADDR(FPGA_BASE_PHYS) \
					| CSPR_PORT_SIZE_16 \
					| CSPR_MSEL_GPCM \
					| CSPR_V)
#define CONFIG_SYS_FPGA_AMASK		IFC_AMASK(64*1024*1024)
#define CONFIG_SYS_FPGA_CSOR		(CSOR_NOR_ADM_SHIFT(4) | \
					CSOR_GPCM_TRHZ_80)

/* FPGA Timing parameters for IFC GPCM */
#define CONFIG_SYS_FPGA_FTIM0		(FTIM0_GPCM_TACSE(0xf) | \
					FTIM0_GPCM_TEADC(0xf) | \
					FTIM0_GPCM_TEAHC(0xf))
/*FTIM1: 0f003f00, to make "version" command works in linux shell before fpga 
image is loaded, disable RGETA and WGETA, set TACO to maxmium value, 
reset to correct value after fpga loaded*/					
#define CONFIG_SYS_FPGA_FTIM1		(FTIM1_GPCM_TACO(0xff) | \
					FTIM1_GPCM_TRAD(0x3f))
#define CONFIG_SYS_FPGA_FTIM2		(FTIM2_GPCM_TCS(0x2) | \
					FTIM2_GPCM_TCH(0x3) | \
					FTIM2_GPCM_TWP(0x3f))
#define CONFIG_SYS_FPGA_FTIM3           0x0

/* CS3 */
/* CPLD */

#define CONFIG_SYS_CPLD_BASE	0x78000000
#define CPLD_BASE_PHYS	CONFIG_SYS_CPLD_BASE

#define CONFIG_SYS_CPLD_CSPR_EXT        (0x0)
#define CONFIG_SYS_CPLD_CSPR		(CSPR_PHYS_ADDR(CPLD_BASE_PHYS) \
					| CSPR_PORT_SIZE_8 \
					| CSPR_MSEL_GPCM \
					| CSPR_V)
#define CONFIG_SYS_CPLD_AMASK		IFC_AMASK(64*1024)
#define CONFIG_SYS_CPLD_CSOR		(CSOR_NOR_ADM_SHIFT(4) | \
					CSOR_NOR_NOR_MODE_AVD_NOR | \
					CSOR_NOR_TRHZ_80)

/* CPLD Timing parameters for IFC GPCM */
#define CONFIG_SYS_CPLD_FTIM0		(FTIM0_GPCM_TACSE(0xf) | \
					FTIM0_GPCM_TEADC(0xf) | \
					FTIM0_GPCM_TEAHC(0xf))
#define CONFIG_SYS_CPLD_FTIM1		(FTIM1_GPCM_TACO(0xff) | \
					FTIM1_GPCM_TRAD(0x3f))
#define CONFIG_SYS_CPLD_FTIM2		(FTIM2_GPCM_TCS(0xf) | \
					FTIM2_GPCM_TCH(0xf) | \
					FTIM2_GPCM_TWP(0xff))
#define CONFIG_SYS_CPLD_FTIM3           0x0


#define CONFIG_SYS_CSPR0_EXT	CONFIG_SYS_NOR0_CSPR_EXT
#define CONFIG_SYS_CSPR0			CONFIG_SYS_NOR0_CSPR
#define CONFIG_SYS_AMASK0		CONFIG_SYS_NOR_AMASK
#define CONFIG_SYS_CSOR0		CONFIG_SYS_NOR_CSOR
#define CONFIG_SYS_CS0_FTIM0		CONFIG_SYS_NOR_FTIM0
#define CONFIG_SYS_CS0_FTIM1		CONFIG_SYS_NOR_FTIM1
#define CONFIG_SYS_CS0_FTIM2		CONFIG_SYS_NOR_FTIM2
#define CONFIG_SYS_CS0_FTIM3		CONFIG_SYS_NOR_FTIM3

#define CONFIG_SYS_CSPR1_EXT	CONFIG_SYS_NAND_CSPR_EXT
#define CONFIG_SYS_CSPR1			CONFIG_SYS_NAND_CSPR
#define CONFIG_SYS_AMASK1		CONFIG_SYS_NAND_AMASK
#define CONFIG_SYS_CSOR1		CONFIG_SYS_NAND_CSOR
#define CONFIG_SYS_CS1_FTIM0		CONFIG_SYS_NAND_FTIM0
#define CONFIG_SYS_CS1_FTIM1		CONFIG_SYS_NAND_FTIM1
#define CONFIG_SYS_CS1_FTIM2		CONFIG_SYS_NAND_FTIM2
#define CONFIG_SYS_CS1_FTIM3		CONFIG_SYS_NAND_FTIM3

#define CONFIG_SYS_CSPR2_EXT	CONFIG_SYS_FPGA_CSPR_EXT
#define CONFIG_SYS_CSPR2			CONFIG_SYS_FPGA_CSPR
#define CONFIG_SYS_AMASK2		CONFIG_SYS_FPGA_AMASK
#define CONFIG_SYS_CSOR2		CONFIG_SYS_FPGA_CSOR
#define CONFIG_SYS_CS2_FTIM0		CONFIG_SYS_FPGA_FTIM0
#define CONFIG_SYS_CS2_FTIM1		CONFIG_SYS_FPGA_FTIM1
#define CONFIG_SYS_CS2_FTIM2		CONFIG_SYS_FPGA_FTIM2
#define CONFIG_SYS_CS2_FTIM3		CONFIG_SYS_FPGA_FTIM3

#define CONFIG_SYS_CSPR3_EXT	CONFIG_SYS_CPLD_CSPR_EXT
#define CONFIG_SYS_CSPR3			CONFIG_SYS_CPLD_CSPR
#define CONFIG_SYS_AMASK3		CONFIG_SYS_CPLD_AMASK
#define CONFIG_SYS_CSOR3		CONFIG_SYS_CPLD_CSOR
#define CONFIG_SYS_CS3_FTIM0		CONFIG_SYS_CPLD_FTIM0
#define CONFIG_SYS_CS3_FTIM1		CONFIG_SYS_CPLD_FTIM1
#define CONFIG_SYS_CS3_FTIM2		CONFIG_SYS_CPLD_FTIM2
#define CONFIG_SYS_CS3_FTIM3		CONFIG_SYS_CPLD_FTIM3


/*
 * Serial Port
 */
#ifdef CONFIG_LPUART
#define CONFIG_LPUART_32B_REG
#else
#define CONFIG_SYS_NS16550_SERIAL
#ifndef CONFIG_DM_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#endif
#define CONFIG_SYS_NS16550_CLK		get_serial_clock()
#endif

/*
 * I2C
 */
/*#define CONFIG_SYS_I2C*/
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */
#define CONFIG_SYS_I2C_MXC_I2C3		/* enable I2C bus 3 */

#define CONFIG_SYS_I2C_CPLD_BUS_NUM	1
#define CONFIG_SYS_I2C_CPLD_PROG_ADDR	0x40
#define CONFIG_SYS_I2C_CPLD_ADDR	0x42


/* EEPROM */
/*#define CONFIG_ID_EEPROM*/
#define CONFIG_SYS_I2C_EEPROM_NXID
#define CONFIG_SYS_EEPROM_BUS_NUM		1
#define CONFIG_SYS_I2C_EEPROM_ADDR		0x53
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		1
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	3
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	5

/*
 * MMC
 */

/* SPI */
#if defined(CONFIG_QSPI_BOOT) || defined(CONFIG_SD_BOOT_QSPI)
/* QSPI */
#define QSPI0_AMBA_BASE			0x40000000
#define FSL_QSPI_FLASH_SIZE		(1 << 24)
#define FSL_QSPI_FLASH_NUM		2

/* DSPI */
#endif

/* DM SPI */
//#if defined(CONFIG_FSL_DSPI) || defined(CONFIG_FSL_QSPI)
//#define CONFIG_DM_SPI_FLASH
//#endif

/*
 * eTSEC
 */

#ifdef CONFIG_TSEC_ENET
#define CONFIG_MII_DEFAULT_TSEC		1
#define CONFIG_TSEC1			1
#define CONFIG_TSEC1_NAME		"eTSEC1"


#define TSEC1_PHY_ADDR			1
#define TSEC2_PHY_ADDR			0
#define TSEC3_PHY_ADDR			0

#define TSEC1_FLAGS			(TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC2_FLAGS			(TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC3_FLAGS			(TSEC_GIGABIT | TSEC_REDUCED)

#define TSEC1_PHYIDX			0
#define TSEC2_PHYIDX			0
#define TSEC3_PHYIDX			0

#define CONFIG_ETHPRIME			"eTSEC1"

#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2
#endif

#define CONFIG_CMDLINE_TAG

#define CONFIG_PEN_ADDR_BIG_ENDIAN
#define CONFIG_LAYERSCAPE_NS_ACCESS
#define CONFIG_SMP_PEN_ADDR		0x01ee0200
#define COUNTER_FREQUENCY		12500000

#define CONFIG_HWCONFIG
#define HWCONFIG_BUFFER_SIZE		256

#define CONFIG_FSL_DEVICE_DISABLE

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
	func(USB, usb, 0)


/*
 * Miscellaneous configurable options
 */

#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		0x9fffffff

#define CONFIG_SYS_LOAD_ADDR		0x82000000

#define CONFIG_LS102XA_STREAM_ID

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_MONITOR_BASE CONFIG_SPL_TEXT_BASE
#else
#define CONFIG_SYS_MONITOR_BASE CONFIG_SYS_TEXT_BASE    /* start of monitor */
#endif

#define CONFIG_SYS_QE_FW_ADDR     0x60940000

/*
 * Environment
 */
//#define CONFIG_ENV_OVERWRITE

//#define CONFIG_ENV_IS_IN_FLASH
//#define CONFIG_ENV_ADDR               (CONFIG_SYS_FLASH_BASE + 0x2000)
//#define CONFIG_ENV_SECT_SIZE          (8 * 1024) /* one 8k boot sector for env */
//#define CONFIG_ENV_SIZE               (8 * 1024)  /* actual env size */
//#define CONFIG_ENV_ADDR_REDUND        (CONFIG_ENV_ADDR + CONFIG_ENV_SECT_SIZE)
//#define CONFIG_ENV_SIZE_REDUND        CONFIG_ENV_SIZE
//#define CONFIG_ENV_FORCE_READ_FLASH
//#define CONFIG_ENV_OVERWRITE
//#define CONFIG_SUPPRESS_ENV_WARNING


#include <asm/fsl_secure_boot.h>
#define CONFIG_SYS_BOOTM_LEN	(64 << 20) /* Increase max gunzip size */


#define CONFIG_HOSTNAME                 "ge110pro"
#define _CONFIG_TFTPDIR                  "ge110pro"
#define _CONFIG_SKDIFILE                 "fsp150.skdi"
#define _CONFIG_FDTFILE                  "ge110pro.dtb"
#define _CONFIG_RCWFILE                 "rcw.bin"
#define _CONFIG_NORFILE                 "ge110pro-nor.bin"
#define _CONFIG_NANDFILE                 "ge110pro-nand.bin"
#define _CONFIG_FPGAFILE                 "ge110pro.bit"
#define _CONFIG_UBOOTFILE                "u-boot-ge110pro.bin"
#define _CONFIG_POSTFILE                 "post-ge110pro.bin"

#define _CONFIG_DCN_INIT_CMD             ""
#define CONFIG_SYS_AUTOLOAD             "no"
#define CONFIG_LOADADDR			0x82000000

  /*
   * Partitions:
   *
   * This partition map mirrors what's defined in the device tree.
   * U-Boot uses this map, Linux uses the map defined in the device tree.
   * The two maps need to match.
   *
   * blksize  blks        name        start          end      size
   * -------  ----   ---------   ----------   ----------   -------
   *       8     1         rcw   0x00000000 - 0x00001FFF      (8K)
   *       8     1        env1   0x00002000 - 0x00003FFF      (8K)
   *       8     1        env2   0x00004000 - 0x00005FFF      (8K)
   *       8     1       spare   0x00006000 - 0x00007FFF      (8K)
   *       8     4         dtb   0x00008000 - 0x0000FFFF     (32K)
   *      64    51        bkdi   0x00010000 - 0x0033FFFF   (3264K)
   *      64     2        post   0x00340000 - 0x0035FFFF    (128K)
   *      64    10       uboot   0x00360000 - 0x003FFFFF    (640K)
   */

#define MTDPARTS_DEFAULT                "mtdparts="                 \
                                          "physmap-flash.0:"      \
                                              "8k(rcw),"         \
                                              "8k(env1),"         \
                                              "8k(env2),"       \
                                              "8k(spare),"       \
                                              "32k(dtb),"         \
                                              "3264k(bkdi),"      \
                                              "128k(post),"       \
                                              "640k(uboot);"      \
                                              "physmap-nand.0:"       \
                                                  "-(ubi)"
#define MTDIDS_DEFAULT                  "nor0=physmap-flash.0,nand0=physmap-nand.0"
  
#ifdef CONFIG_SYS_USE_UBI
  #define CONFIG_SYS_UBI                "ubi.mtd=ubi"   /* This name must match the name
                                                             defined in the device tree */
  #define CONFIG_BOOTARGS               CONFIG_SYS_UBI " panic=5"
#endif

/* Definitions of the NOR flash partitions used by u-boot macros */
#define NORFLASH_UBOOT_ADDR         0x60360000
#define NORFLASH_UBOOT_SIZE           0x000A0000 /*640K*/

#define NORFLASH_RCW_ADDR             0x60000000
#define NORFLASH_RCW_SIZE              0x2000      /* one 8k env block */

#define NORFLASH_ENV_ADDR             0x60002000
#define NORFLASH_ENV_SIZE             0x4000      /* two 8k env blocks */

#define NORFLASH_FDT_ADDR             0x60008000
#define NORFLASH_FDT_SIZE             0x8000     /* four 8K sectors*/

#define NORFLASH_BKDI_ADDR            0x60010000
#define NORFLASH_BKDI_SIZE            0x00330000

#define NORFLASH_POST_ADDR            0x60340000
#define NORFLASH_POST_SIZE            0x00020000

#define NORFLASH_MTPS_ADDR            NORFLASH_POST_ADDR
#define NORFLASH_MTPS_SIZE            NORFLASH_POST_SIZE

#define NORFLASH_NOR_ADDR              NORFLASH_RCW_ADDR
#define NORFLASH_NOR_SIZE              0x400000

#define CONFIG_EXTRA_ENV_SETTINGS         \
        "mtdparts="  MTDPARTS_DEFAULT "\0"  \
        "mtdids="    MTDIDS_DEFAULT   "\0"  \
        "serverdir=" _CONFIG_TFTPDIR   "\0"  \
        "skdifile="  _CONFIG_SKDIFILE  "\0"  \
        "fdtfile="   _CONFIG_FDTFILE   "\0"  \
        "rcwfile="	 _CONFIG_RCWFILE   "\0"	\
        "norfile="   _CONFIG_NORFILE   "\0"  \
        "nandfile="  _CONFIG_NANDFILE  "\0"  \
        "fpgafile="  _CONFIG_FPGAFILE  "\0"  \
        "ubootfile=" _CONFIG_UBOOTFILE "\0"  \
        "postfile="  _CONFIG_POSTFILE  "\0"  \
        "ubiskdi="   _CONFIG_SKDIFILE  "\0"  \
        "ubifdt="    _CONFIG_FDTFILE   "\0"  \
        BASE_ENV_SETTINGS

#define BASE_ENV_SETTINGS                                                    \
        "netretry=no\0"                                                        \
        "kdiaddr=0x82000000\0"                                                 \
        "fdtaddr=0x81FF0000\0"                                                 \
        "cpldaddr=" __stringify(CONFIG_SYS_CPLD_BASE) "\0"                     \
        "fpgaaddr=" __stringify(CONFIG_SYS_FPGA_BASE) "\0"                     \
        "ethinit=if test 0$ethinitdone -ne 1; then "                           \
                    _CONFIG_DCN_INIT_CMD "; "                                   \
                    "ethinitdone=1; "                                          \
                "fi; "                                                         \
                "if test _$ipconfig = _static -o 0$dhcpdone -eq 1; then "      \
                    "exit; "                                                   \
                "fi; "                                                         \
                "if dhcp; then "                                               \
                    "dhcpdone=1; "                                             \
                "fi\0"                                                         \
        "clearenv=if pro off " __stringify(NORFLASH_ENV_ADDR)                  \
                          " +" __stringify(NORFLASH_ENV_SIZE) "; "             \
                  "then "                                                      \
                      "era " __stringify(NORFLASH_ENV_ADDR)                    \
                        " +" __stringify(NORFLASH_ENV_SIZE) "; "               \
                      "pro on " __stringify(NORFLASH_ENV_ADDR)                 \
                           " +" __stringify(NORFLASH_ENV_SIZE) "; "            \
                  "fi\0"                                                       \
        "_updateimg=if run ethinit && tftp $loadaddr $updatefile; then "       \
                       "pro off $updateaddr +$updatesize; "                    \
                       "era $updateaddr +$updatesize; "                        \
                       "cp.b $loadaddr $updateaddr $filesize; "                \
                       "pro on $updateaddr +$updatesize; "                     \
                       "cmp.b $loadaddr $updateaddr $filesize; "               \
                  "fi\0"                                                       \
        "update_rcw=updatefile=$serverdir/$rcwfile; "                      \
                    "updateaddr=" __stringify(NORFLASH_RCW_ADDR) "; "        \
                    "updatesize=" __stringify(NORFLASH_RCW_SIZE) "; "        \
                    "run _updateimg\0"                                         \
        "update_uboot=updatefile=$serverdir/$ubootfile; "                      \
                    "updateaddr=" __stringify(NORFLASH_UBOOT_ADDR) "; "        \
                    "updatesize=" __stringify(NORFLASH_UBOOT_SIZE) "; "        \
                    "run _updateimg\0"                                         \
        "update_post=updatefile=$serverdir/$postfile; "                        \
                    "updateaddr=" __stringify(NORFLASH_POST_ADDR) "; "         \
                    "updatesize=" __stringify(NORFLASH_POST_SIZE) "; "         \
                    "run _updateimg\0"                                         \
        "update_fdt=updatefile=$serverdir/$fdtfile; " 					   \
                    "updateaddr=" __stringify(NORFLASH_FDT_ADDR) "; "		   \
                    "updatesize=" __stringify(NORFLASH_FDT_SIZE) "; "		   \
                    "run _updateimg\0"							\
        "update_nor=updatefile=$serverdir/$norfile; " 					   \
                    "updateaddr=" __stringify(NORFLASH_NOR_ADDR) "; "		   \
                    "updatesize=" __stringify(NORFLASH_NOR_SIZE) "; "		   \
                    "run _updateimg\0"							\
        "update_nand=if run ethinit && tftp $loadaddr $serverdir/$nandfile; "  \
                    "then "                                                    \
                        "nand erase.chip; "                                    \
                        "nand write.trimffs $loadaddr 0 $filesize; "           \
                    "fi\0"                                                     \
        "load_fpga=if run ethinit && tftp $serverdir/$fpgafile; then "         \
                      "fpga loadb 0 $fileaddr $filesize; "                     \
                  "fi\0"                                                       \
        "setbootargs=setenv bootargs " CONFIG_BOOTARGS " $extraargs "          \
                    "craft=$baudrate\0"                                        \
        "boot_tftp=if run ethinit && "                                         \
                      "tftp $kdiaddr $serverdir/$skdifile && "                 \
                      "tftp $fdtaddr $serverdir/$fdtfile; "                    \
                  "then "                                                      \
                      "run setbootargs; bootm $kdiaddr - $fdtaddr; "           \
                  "fi\0"                                                       \
        "boot_ubi=run setbootargs; bootubi\0"    \
        "bootcmd=run boot_ubi"

#endif
