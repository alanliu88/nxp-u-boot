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
#ifndef __CPLD_H__
#define __CPLD_H__

#include <common.h>

#ifdef CONFIG_BOARD_CPLD_HEADER

/* Include a board specific cpld header */
#include CONFIG_BOARD_CPLD_HEADER

#else
/* Use this common cpld header */

/*
 * CPLD I/O Macros
 */

#define CPLD_READ(reg) \
    ((volatile unsigned char *)CONFIG_SYS_CPLD_BASE)[reg]
#define CPLD_WRITE(reg, val) \
    ((volatile unsigned char *)CONFIG_SYS_CPLD_BASE)[reg] = (val)
#define CPLD_SET_BITS(reg, bits) \
    ((volatile unsigned char *)CONFIG_SYS_CPLD_BASE)[reg] |= (bits)
#define CPLD_CLR_BITS(reg, bits) \
    ((volatile unsigned char *)CONFIG_SYS_CPLD_BASE)[reg] &= ~(bits)
#define CPLD_GET_BITS(reg, bits) \
    ((volatile unsigned char *)CONFIG_SYS_CPLD_BASE)[reg] & (bits)
#define CPLD_CLRSET_BITS(reg, clear, set) \
    CPLD_WRITE((reg), (CPLD_READ(reg) & ~((u8)(clear))) | (set))

/*
 * CPLD Register Definitions
 */

#define CPLD_VERSION_REG                    0x00

#define CPLD_PCB_REV_REG                    0x01

#define CPLD_RESET0_CNTRL_REG               0x02
#define CPLD_RESET0_CNTRL_MAX9451_IEEE      (1 << 7)
#define CPLD_RESET0_CNTRL_MAX9451           (1 << 6)
#define CPLD_RESET0_CNTRL_DS310X            (1 << 5)
#define CPLD_RESET0_CNTRL_DS31408           (1 << 5)
#define CPLD_RESET0_CNTRL_DS31407           (1 << 5)
#define CPLD_RESET0_CNTRL_IIC               (1 << 4)
#define CPLD_RESET0_CNTRL_SWITCH            (1 << 3)
#define CPLD_RESET0_CNTRL_MGMTPHY           (1 << 3)
#define CPLD_RESET0_CNTRL_PHY               (1 << 2)
#define CPLD_RESET0_CNTRL_FPGA              (1 << 1)
#define CPLD_RESET0_CNTRL_NOR               (1 << 0)

#define CPLD_RESET1_CNTRL_REG               0x03
#define CPLD_RESET0_CNTRL_BITS              (1 << 7)
#define CPLD_RESET0_CNTRL_BITS_2            (1 << 6)
#define CPLD_RESET1_CNTRL_PHY              (1 << 6)

#define CPLD_FPGA_CNTRL_REG                 0x04
#define CPLD_FPGA_CNTRL_SW_CLOCK            (1 << 4)
#define CPLD_FPGA_CNTRL_CLOCK               (1 << 3)
#define CPLD_FPGA_CNTRL_PROG                (1 << 2)
#define CPLD_FPGA_CNTRL_RDWR                (1 << 1)
#define CPLD_FPGA_CNTRL_CSI                 (1 << 0)

#define CPLD_FPGA_STATUS_REG                0x05
#define CPLD_FPGA_STATUS_INIT               (1 << 2)
#define CPLD_FPGA_STATUS_BUSY               (1 << 1)
#define CPLD_FPGA_STATUS_DONE               (1 << 0)

#define CPLD_FPGA_DATA_REG                  0x06

#define CPLD_INT0_CNTRL_REG                 0x07
#define CPLD_INT1_CNTRL_REG                 0x08
#define CPLD_INT2_CNTRL_REG                 0x09
#define CPLD_INT3_CNTRL_REG                 0x0a
#define CPLD_INT4_CNTRL_REG                 0x0b
#define CPLD_INT5_CNTRL_REG                 0x0c

#define CPLD_INT_CNTRL_INT0                 1
#define CPLD_INT_CNTRL_INT1                 2
#define CPLD_INT_CNTRL_INT2                 3
#define CPLD_INT_CNTRL_INT3                 4
#define CPLD_INT_CNTRL_INT4                 5

#define CPLD_INT_STATUS0_REG                0x0d
#define CPLD_INT_STATUS0_DS310X             (1 << 4)
#define CPLD_INT_STATUS0_PHY1               (1 << 3)
#define CPLD_INT_STATUS0_PHY2               (1 << 2)
#define CPLD_INT_STATUS0_SWITCH             (1 << 1)
#define CPLD_INT_STATUS0_FPGA               (1 << 0)

#define CPLD_INT_STATUS1_REG                0x0e

#define CPLD_SCRATCH0_REG                   0x0f

#define CPLD_SCRATCH1_REG                   0x10

#define CPLD_TEST_REG                       0x11

#define CPLD_WRITE_PROTECT_REG              0x12
#define CPLD_WRITE_PROTECT_NOR              (1 << 0)

#define CPLD_UART_REG                       0x13
#define CPLD_UART_DSR                       (1 << 1)
#define CPLD_UART_DTR                       (1 << 0)

#define CPLD_SPI_SEL_REG                    0x14
#define CPLD_SPI_SEL_DS310X              (1 << 0)
#define CPLD_SPI_SEL_DS26521            (1 << 1)
#define CPLD_SPI_SEL_DS26521_2        (1 << 2)
#define CPLD_SPI_SEL_MACSEC_SRAM   (1 << 2)
#define CPLD_SPI_SEL_MACSEC_PHy       (1 << 3)

#define CPLD_STATUS_REG                     0x15
#define CPLD_STATUS_MAX9451_LOCK            (1 << 2)
#define CPLD_STATUS_PSU_PRESENT             (1 << 1)
#define CPLD_STATUS_PSU_POWER_GOOD          (1 << 0)

#define CPLD_LAMP_TEST_REG                  0x16
#define CPLD_LAMP_TEST_ENABLE               (1 << 0)

#define CPLD_STATUS_LED_REG                 0x17
#define CPLD_STATUS_LED_RED                 (1 << 2)
#define CPLD_STATUS_LED_GREEN               (1 << 1)
#define CPLD_STATUS_LED_BLINK               (1 << 0)

#define CPLD_NETWORK_LED_REG                0x18
#define CPLD_NETWORK0_LED_RED               (1 << 2)
#define CPLD_NETWORK0_LED_GREEN             (1 << 1)
#define CPLD_NETWORK0_LED_BLINK             (1 << 0)

#define CPLD_ACCESS_LED_REG0                0x19
#define CPLD_ACCESS0_LED_RED                (1 << 2)
#define CPLD_ACCESS0_LED_GREEN              (1 << 1)
#define CPLD_ACCESS0_LED_BLINK              (1 << 0)

#define CPLD_ACCESS_LED_REG1                0x1a

#define CPLD_ACCESS_LED_REG2                0x1b

#define CPLD_PORT_STATUS_LED_REG            0x1c
#define CPLD_PORT_STATUS_ACCESS_MODE        (1 << 7)
#define CPLD_PORT_STATUS_ACCESS_YELLOW      (1 << 6)
#define CPLD_PORT_STATUS_ACCESS_GREEN       (1 << 5)
#define CPLD_PORT_STATUS_ACCESS_BLINK       (1 << 4)
#define CPLD_PORT_STATUS_NETWORK_MODE       (1 << 3)
#define CPLD_PORT_STATUS_NETWORK_YELLOW     (1 << 2)
#define CPLD_PORT_STATUS_NETWORK_GREEN      (1 << 1)
#define CPLD_PORT_STATUS_NETWORK_BLINK      (1 << 0)

#define CPLD_SFP_CNTRL_REG                  0x1d
#define CPLD_SFP_CNTRL_ACCESS0_EN           (1 << 2)
#define CPLD_SFP_CNTRL_NETWORK0_EN          (1 << 0)

#define CPLD_SFP_STATUS0_REG                0x1e
#define CPLD_SFP_NETWORK0_PRESENT           (1 << 2)
#define CPLD_SFP_NETWORK0_TX_FALT           (1 << 1)
#define CPLD_SFP_NETWORK0_RX_LOSS           (1 << 0)

#define CPLD_SFP_STATUS1_REG                0x1f
#define CPLD_SFP_ACCESS0_PRESENT            (1 << 2)
#define CPLD_SFP_ACCESS0_TX_FALT            (1 << 1)
#define CPLD_SFP_ACCESS0_RX_LOSS            (1 << 0)

#define CPLD_RESET2_CNTRL_REG               0x2e
#define CPLD_RESET2_CNTRL_PWR_IIC           (1 << 4)
#define CPLD_RESET2_CNTRL_PTP3              (1 << 3)
#define CPLD_RESET2_CNTRL_PTP2              (1 << 2)
#define CPLD_RESET2_CNTRL_PTP1              (1 << 1)
#define CPLD_RESET2_CNTRL_PTP0              (1 << 0)

#define CPLD_WATCHDOG_WDCR                  0xfb
#define CPLD_WATCHDOG_WDCR_ENA              (1 << 0)
#define CPLD_WATCHDOG_WDCR_PERM             (1 << 1)

#define CPLD_WATCHDOG_WDICR                 0xfc

#define CPLD_WATCHDOG_WDCCR                 0xfd

#define CPLD_WATCHDOG_WDSR                  0xfe
#define CPLD_WATCHDOG_WDSR_VAL1             0x6c
#define CPLD_WATCHDOG_WDSR_VAL2             0x39

#define CPLD_RESET_CAUSE_REG                0xff
#define CPLD_RESET_CAUSE_BIT7               (1 << 7)
#define CPLD_RESET_CAUSE_BIT6               (1 << 6)
#define CPLD_RESET_CAUSE_BIT5               (1 << 5)
#define CPLD_RESET_CAUSE_BIT4               (1 << 4)
#define CPLD_RESET_CAUSE_BIT3               (1 << 3)
#define CPLD_RESET_CAUSE_WATCHDOG           (1 << 2)
#define CPLD_RESET_CAUSE_PWRESET            (1 << 1)
#define CPLD_RESET_CAUSE_HRESET             (1 << 0)

/* 
 * for GE114PRO cards
*/
#define CPLD_CARD_RESET_CNTRL_REG           0x30
#define CPLD_CARD_RESET_CNTRL_ENA           (1 << 3)
#define CPLD_CARD_RESET_CNTRL_POWER         5
#define CPLD_CARD_RESET_CNTRL_POR           (1 << 2)
#define CPLD_CARD_RESET_CNTRL_COLD          (1 << 1)
#define CPLD_CARD_RESET_CNTRL_WARM          (1 << 0)

#endif /* BOARD_CPLD */
#endif /* __CPLD_H__ */
