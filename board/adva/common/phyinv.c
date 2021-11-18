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
#include "phyinv.h"
#include <common.h>

/* GLOBALS */

DECLARE_GLOBAL_DATA_PTR;

int
_computeChecksum (struct phyInv_t *phyInv, bool silent)
{
    unsigned long chksumOnEntryPage0;
    unsigned long chksumOnEntryPage1;
    unsigned long chksumComputedPage0;
    unsigned long chksumComputedPage1;
    unsigned char *phyInvCharPtr;
    unsigned int i;
    unsigned long checksum = 0;
    /* save the check sum, to allow a validate function */
    chksumOnEntryPage0 = phyInv->p0->checksumChar[3];
    chksumOnEntryPage0 += phyInv->p0->checksumChar[2] << 8;
    chksumOnEntryPage0 += phyInv->p0->checksumChar[1] << 16;
    chksumOnEntryPage0 += phyInv->p0->checksumChar[0] << 24;
    chksumOnEntryPage1 = phyInv->p1->checksumChar[3];
    chksumOnEntryPage1 += phyInv->p1->checksumChar[2] << 8;
    chksumOnEntryPage1 += phyInv->p1->checksumChar[1] << 16;
    chksumOnEntryPage1 += phyInv->p1->checksumChar[0] << 24;

    /* Page 0 */
    phyInvCharPtr =
        (unsigned char *) phyInv->p0 + sizeof (phyInv->p0->checksumChar);

    for (i = sizeof (phyInv->p0->checksumChar); i < sizeof (PhyInv_t_page0);
            i++)
    {

        checksum += *phyInvCharPtr++;
    }
    chksumComputedPage0 = checksum;
    if (!silent)
    {
        printf ("chksumOnEntryPage0  = %#4.4lx\n", chksumOnEntryPage0);
        printf ("chksumComputedPage0 = %#4.4lx\n", chksumComputedPage0);
    }
    phyInv->p0->checksumChar[3] = (unsigned char) (checksum & 0x000000ff);
    phyInv->p0->checksumChar[2] =
        (unsigned char) ((checksum & 0x0000ff00) >> 8);
    phyInv->p0->checksumChar[1] =
        (unsigned char) ((checksum & 0x00ff0000) >> 16);
    phyInv->p0->checksumChar[0] =
        (unsigned char) ((checksum & 0xff000000) >> 24);

    /* Page 1 */
    phyInvCharPtr =
        (unsigned char *) phyInv->p1 + sizeof (phyInv->p1->checksumChar);

    checksum = 0;
    for (i = sizeof (phyInv->p1->checksumChar); i < sizeof (PhyInv_t_page1);
            i++)
    {

        checksum += *phyInvCharPtr++;
    }
    chksumComputedPage1 = checksum;
    phyInv->p1->checksumChar[3] = (unsigned char) (checksum & 0x000000ff);
    phyInv->p1->checksumChar[2] =
        (unsigned char) ((checksum & 0x0000ff00) >> 8);
    phyInv->p1->checksumChar[1] =
        (unsigned char) ((checksum & 0x00ff0000) >> 16);
    phyInv->p1->checksumChar[0] =
        (unsigned char) ((checksum & 0xff000000) >> 24);

    if (!silent)
    {
        printf ("chksumOnEntryPage1  = %#4.4lx\n", chksumOnEntryPage1);
        printf ("chksumComputedPage1 = %#4.4lx\n", chksumComputedPage1);
    }
    
    if (chksumComputedPage0 != chksumOnEntryPage0)
        return 1;
    if (chksumComputedPage1 != chksumOnEntryPage1)
        return 1;
    
    return 0;
}

int
computeChecksum (struct phyInv_t *phyInv)
{
    return _computeChecksum (phyInv, false);
}

#if 0
/* This isn't exactly equivalent to BSD strlcpy but it's close enough */
static char *
strlcpy (char *dest, const char *src, size_t len)
{

    if (len > 0)
    {
        strncpy (dest, src, len - 1);
        dest[len - 1] = '\0';
    }
    return dest;
}
#endif

unsigned int
dumpPhyInvHex (struct phyInv_t *startPtr)
{
    char buf[sizeof (PhyInv_t_page1)];
    int i;
    memcpy (buf, startPtr->p0, 256);
    printf ("\n-------page 0---------\n");
    for (i = 0; i < 256; i++)
    {
        if ((i % 16 == 0 && i / 16) || i == 0)
            printf ("\n %#2.2x= ", i);
        printf ("%2.2x ", (u8) buf[i]);
    }
    printf ("\n-------page 1---------\n");
    memcpy (buf, startPtr->p1, 256);
    for (i = 0; i < 256; i++)
    {
        if ((i % 16 == 0 && i / 16) || i == 0)
            printf ("\n %#2.2x= ", i);
        printf ("%2.2x ", (u8) buf[i]);
    }
    putc ('\n');
    return 0;
}


unsigned int
dumpPhyInv (struct phyInv_t *startPtr)
{
    char buf[sizeof (PhyInv_t_page1)];
    printf ("size of phyInv is page0:%#lx+page1:%#lx\n", sizeof (PhyInv_t_page0),
            sizeof (PhyInv_t_page1));

    printf ("Signature Checkcount Page 0          = %2.2x%2.2x%2.2x%2.2x\n",
            startPtr->p0->checksumChar[0],
            startPtr->p0->checksumChar[1],
            startPtr->p0->checksumChar[2], startPtr->p0->checksumChar[3]);
    printf ("Signature Checkcount Page 1          = %2.2x%2.2x%2.2x%2.2x\n",
            startPtr->p1->checksumChar[0],
            startPtr->p1->checksumChar[1],
            startPtr->p1->checksumChar[2], startPtr->p1->checksumChar[3]);
    strlcpy (buf, startPtr->p0->fmtVer, sizeof (startPtr->p0->fmtVer) + 1);
    printf ("fmtver - Format Version              = %s\n", buf);
    strlcpy (buf, startPtr->p0->unitName, sizeof (startPtr->p0->unitName) + 1);
    printf ("uname  - Unit Name (Card Type)       = %s\n", buf);
    strlcpy (buf, startPtr->p0->cleiCode, sizeof (startPtr->p0->cleiCode) + 1);
    printf ("clei   - CLEI Code                   = %s\n", buf);
    strlcpy (buf, startPtr->p0->serNum, sizeof (startPtr->p0->serNum) + 1);
    printf ("sn     - Serial Number               = %s\n", buf);
    strlcpy (buf, startPtr->p0->partNum, sizeof (startPtr->p0->partNum) + 1);
    printf ("pn     - PartNumber                  = %s\n", buf);
    printf
        ("addr   - Base PHY Addr               = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
         startPtr->p0->basePhyAddr[0], startPtr->p0->basePhyAddr[1],
         startPtr->p0->basePhyAddr[2], startPtr->p0->basePhyAddr[3],
         startPtr->p0->basePhyAddr[4], startPtr->p0->basePhyAddr[5]);
    printf ("nummac - Number of MAC Addresses     = %2.2u\n",
            (startPtr->p0->numMacAddr[0] << 8) + startPtr->p0->numMacAddr[1]);
    strlcpy (buf, startPtr->p0->hwVer, sizeof (startPtr->p0->hwVer) + 1);
    printf ("hwver  - Hardware Version            = %s\n", buf);
    strlcpy (buf, startPtr->p0->compatId, sizeof (startPtr->p0->compatId) + 1);
    printf ("compid - SW/HW Compatibility Id      = %s\n", buf);
    strlcpy (buf, startPtr->p0->mfg, sizeof (startPtr->p0->mfg) + 1);
    printf ("mfg    - Manufacturer                = %s\n", buf);
    strlcpy (buf, startPtr->p0->mfgSite, sizeof (startPtr->p0->mfgSite) + 1);
    printf ("mfgsite - Manufacture Site           = %s\n", buf);
    strlcpy (buf, startPtr->p0->mfgDate, sizeof (startPtr->p0->mfgDate) + 1);
    printf ("mfgdate - Manufacture Date           = %s\n", buf);
    printf ("fipsmode - fips mode switch          = %d\n", startPtr->p0->fipsMode);
    strlcpy (buf, startPtr->p1->testSite, sizeof (startPtr->p1->testSite) + 1);
    printf ("testsite - Test Site                 = %s\n", buf);
    strlcpy (buf, startPtr->p1->testDate, sizeof (startPtr->p1->testDate) + 1);
    printf ("testdate - Test Date                 = %s\n", buf);
    strlcpy (buf, startPtr->p1->burnInTestDate,
            sizeof (startPtr->p1->burnInTestDate) + 1);
    printf ("burnintestdate - Burn In Date        = %s\n", buf);
    strlcpy (buf, startPtr->p1->finalTestDate,
            sizeof (startPtr->p1->finalTestDate) + 1);
    printf ("finaltestdate  - Final Test Date     = %s\n", buf);
    strlcpy (buf, startPtr->p1->productionTestArea,
            sizeof (startPtr->p1->productionTestArea) + 1);
    printf ("productiontestarea  -                = %s\n", buf);
    return 0;

}
