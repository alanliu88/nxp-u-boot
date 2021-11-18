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


#ifndef _CPLD_H_
#define _CPLD_H_

#include "lptm21.h"

int32_t cpld_info_short_print(struct udevice* dev);
void cpld_watchdog_enable(const uint8_t onoff);
void cpld_flashupdate_enable(const uint8_t onoff);
int32_t cpld_update(struct udevice* dev, const flash_sector_t sector);
int32_t cpld_update_all(struct udevice* dev);
flash_sector_t userentryidx2flash_sector(const uint8_t userentryidx);

#endif
