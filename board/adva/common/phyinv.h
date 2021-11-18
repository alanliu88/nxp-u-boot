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
#ifndef __PHYINV_H__
#define __PHYINV_H__

typedef struct phyInv_t_page0
{
    /* 0x00 */
    unsigned char checksumChar[4];
    /* start of checksum area page 0*/
    /* 0x04 */
    char fmtVer[8];
    /* 0x0C */
    char unitName[16];
    /* 0x1C */
    char cleiCode[16];
    /* 0x2C */
    char serNum[32];
    /* 0x4C */
    char partNum[32];
    /* 0x6C */
    unsigned char basePhyAddr[6];
    /* 0x72 */
    unsigned char numMacAddr[2];
    /* 0x74 */
    char hwVer[16];
    /* 0x84 */
    char compatId[16];
    /* 0x94 */
    char mfg[16];
    /* 0xA4 */
    char mfgSite[16];
    /* 0xB4 */
    char mfgDate[16];
    /* 0xC4 */
	unsigned char fipsMode;
	/* 0xC5*/
    char unused[0x100 - 0xC5];
    /*end of shecksum area */
} PhyInv_t_page0;

typedef struct phyInv_t_page1
{
    /* 0x00 */
    unsigned char checksumChar[4];
    /* 0x04 */
    /* start of checksum area page 1*/
    char testSite[16];
    /* 0x14 */
    char testDate[16];
    /* 0x24 */
    char burnInTestDate[16];
    /* 0x34 */
    char finalTestDate[16];
    /* 0x44 */
    char productionTestArea[16];
    /* 0x54 */
    char unused[0x100 - 0x54];
    /*end of shecksum area */
} PhyInv_t_page1;


typedef struct phyInv_t
{
    PhyInv_t_page0 *p0;
    PhyInv_t_page1 *p1;
} PhyInv_t;

extern unsigned int dumpPhyInv (struct phyInv_t *startPtr);
extern unsigned int dumpPhyInvHex (struct phyInv_t *startP);
extern int computeChecksum (struct phyInv_t *phyInv);

#endif /* __PHYINV_H__ */
