#ifndef _BOARD_PHYINV_H_
#define _BOARD_PHYINV_H_
/*
 * Copyright (C) 2012 ADVA Optical Networking, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <malloc.h>
#include <i2c.h>
#include <i2c_eeprom.h>

#include "phyinv.h"
extern struct udevice* get_i2c_inv_dev(int ext);
/******************************************************************************
 * Read and verify physical inventory eeprom page 0
 *****************************************************************************/
static const unsigned char* phyinv_page0_buffer (void)
{
	struct udevice* dev = NULL;
    static unsigned char* phyinv = NULL;
    int count;

    for (count = 10; !phyinv && count; count--)
    {
        int size = sizeof(PhyInv_t_page0);
        
        dev = get_i2c_inv_dev(0);
        if (dev == 0) 
        {
            printf("%s failed to get inv i2c bus\n",__func__);
            return NULL;
        }

        udelay(10);

        phyinv = (unsigned char*) malloc (size);
        if (phyinv)
        {
            int ret;
            u_int32_t sum = 0;

            ret = dm_i2c_read (dev, 0, phyinv, size);
            if (ret == 0)
            {
                int i;

                for (i = 4; i < 256; i++)
                {
                    sum += phyinv[i];
                }
            }

            if ((ret != 0) || (sum != ntohl(*((u_int32_t*)phyinv))))
            {
                free (phyinv);
                phyinv = NULL;
            }
        }
    }

    return phyinv;
}

/******************************************************************************
 * Return pointer to base MAC address from physical inventory
 *****************************************************************************/
static inline unsigned char* phyinv_base_mac (void)
{
    static struct {
        union {
            unsigned short s[3];
            unsigned char c[6];
        } u;
    } macAddr = { .u.s = { 0, 0, 0 } };
    unsigned char* macPtr = macAddr.u.c;

    if (!macAddr.u.s[0] && !macAddr.u.s[1] && !macAddr.u.s[2])
    {
        const PhyInv_t_page0* page0;

        page0 = (PhyInv_t_page0*) phyinv_page0_buffer();
        if (page0)
        {
            memcpy (macPtr, page0->basePhyAddr, 6);
        }
        else
        {
            macPtr = NULL;
        }
    }

    return macPtr;
}

#endif
