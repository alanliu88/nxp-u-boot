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
#ifndef __SPI_COMMON_H__
#define __SPI_COMMON_H__

#include <common.h>

/*
 * SPI COMMON Macros
 */
#ifdef CONFIG_FSL_DSPI
#define SPI_BUS     1
#else
#define SPI_BUS     0
#endif
#define SPI_MODE    SPI_MODE_0

#define BIT_LEN     24
#define MAX_REG     0x3fff

#define WRITE       0x00
#define READ        0x80
#define SINGLE      0x00
#define BURST       0x01

#define SPI_SPEED_1MHZ  1000000
#define SPI_SPEED_2MHZ  2000000
#define SPI_SPEED_3MHZ  3000000
#define SPI_SPEED_4MHZ  4000000
#define SPI_SPEED_5MHZ  5000000
#define SPI_SPEED_6MHZ  6000000

#endif /* __SPI_COMMON_H__ */

