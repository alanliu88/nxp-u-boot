/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __LX2_QDS_H
#define __LX2_QDS_H

#include "lx2160a_common.h"

/* Qixis */
#define QIXIS_XMAP_MASK			0x07
#define QIXIS_XMAP_SHIFT		5
#define QIXIS_RST_CTL_RESET_EN		0x30

#define QIXIS_LBMAP_DFLTBANK		0x00
#define QIXIS_LBMAP_ALTBANK		0x20
#define QIXIS_LBMAP_QSPI		0x00
#define QIXIS_RCW_SRC_QSPI		0xff
#define QIXIS_RST_CTL_RESET		0x31
#define QIXIS_RCFG_CTL_RECONFIG_IDLE	0x20
#define QIXIS_RCFG_CTL_RECONFIG_START	0x21
#define QIXIS_RCFG_CTL_WATCHDOG_ENBLE	0x08
#define QIXIS_LBMAP_MASK		0x0f
#define QIXIS_SDID_MASK				0x07
#define QIXIS_ESDHC_NO_ADAPTER			0x7


/* SYSCLK */
#define QIXIS_SYSCLK_100		0x0
#define QIXIS_SYSCLK_125		0x1
#define QIXIS_SYSCLK_133		0x2

/* DDRCLK */
#define QIXIS_DDRCLK_100		0x0
#define QIXIS_DDRCLK_125		0x1
#define QIXIS_DDRCLK_133		0x2

#define BRDCFG4_EMI1SEL_MASK		0x38
#define BRDCFG4_EMI1SEL_SHIFT		3
#define BRDCFG4_EMI2SEL_MASK		0x07
#define BRDCFG4_EMI2SEL_SHIFT		0

/*
 * MMC
 */
#ifdef CONFIG_MMC
#ifndef __ASSEMBLY__
u8 qixis_esdhc_detect_quirk(void);
#endif
#define CONFIG_ESDHC_DETECT_QUIRK  qixis_esdhc_detect_quirk()
#endif

/* MAC/PHY configuration */
#if defined(CONFIG_FSL_MC_ENET) && !defined(CONFIG_SPL_BUILD)
#define CONFIG_PHYLIB_10G
#define CONFIG_MII
#define CONFIG_ETHPRIME		"DPMAC1@xgmii"

#define CONFIG_PHY_AQUANTIA
#define AQ_PHY_ADDR1		0x00
#define AQ_PHY_ADDR2		0x01
#define AQ_PHY_ADDR3		0x02
#define AQ_PHY_ADDR4		0x03
#define AQR405_IRQ_MASK		0x36

#define CONFIG_PHY_CORTINA
#define CORTINA_NO_FW_UPLOAD
#define CORTINA_PHY_ADDR1	0x0

#define CONFIG_PHY_REALTEK
#define RGMII_PHY_ADDR1		0x01
#define RGMII_PHY_ADDR2		0x02

#define SGMII_CARD_PORT1_PHY_ADDR 0x1C
#define SGMII_CARD_PORT2_PHY_ADDR 0x1D
#define SGMII_CARD_PORT3_PHY_ADDR 0x1E
#define SGMII_CARD_PORT4_PHY_ADDR 0x1F

#endif

#include <asm/fsl_secure_boot.h>

#endif /* __LX2_QDS_H */