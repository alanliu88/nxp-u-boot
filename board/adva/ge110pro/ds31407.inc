/*
 * ds31407.inc
 *
 * DS31407 register setup array
 *    derrived from DS31407 Startup.mfg
 *
 * Copyright (C) 2014 ADVA Optical Networking
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */

    /*MCPLL*/
    { 0x03DE, 0xF0 },
    { 0x03DF, 0x40 },
    { 0x03E0, 0x3F },
    { 0x03E1, 0x8F },
    { 0x03E2, 0x80 },
    { 0x03E3, 0x08 },
    { 0x03E4, 0x98 },
    { 0x03E5, 0x04 },
    { 0x03E5, 0x00 },

    /*Output APLL1*/
    { 0x0060,   0x01 },

    /*Enable Noise Shaping*/
    { 0x00D3, 0x03 },
    { 0x00D4, 0x40 },
    { 0x00D6, 0x3F },
    { 0x00D7, 0x0F },
    { 0x00D8, 0x87 },
    { 0x00D9, 0x08 },

    /* Clear DPLL external switch mode */
    { 0x0081, 0x20 },

    /*No D2PHLKTO*/
    { 0x0080, 0x01 },
    { 0x008A, 0x00 },
    { 0x0080, 0x00 },

    /*then, do the software configuration.*/
    /* Begin OC1 Register Bank
    OC1 AFBDIV: N1 = 80; N2 = 1; D = 1; Mode = 1
    The comment line above is required for DS31407 DK software configuration
    */
    { 0X00C0, 0X01 },
    { 0X00C1, 0XC0 },
    { 0X00C3, 0X80 },
    { 0X00C4, 0X80 },
    { 0X00C8, 0X01 },
    { 0X00C9, 0X00 },
    { 0X00CA, 0X00 },
    { 0X00CB, 0X00 },
    { 0X00D0, 0X8A },
    { 0X00D2, 0X0F },
    { 0X00D8, 0X02 },
    { 0X00E0, 0X00 },
    { 0X00E1, 0X00 },
    { 0X00E2, 0X00 },
    { 0X00E3, 0X00 },
    { 0X00E4, 0X00 },
    { 0X00E5, 0X00 },
    { 0X00E6, 0X00 },
    { 0X00E7, 0X00 },
    { 0X00E8, 0X00 },
    { 0X00E9, 0X50 },
    { 0X00F2, 0X2C },

    /* Begin OC4 Register Bank */
    { 0X00C0, 0X04 },
    { 0X00B0, 0X99 },
    { 0X00B1, 0X99 },
    { 0X00B2, 0X99 },
    { 0X00B3, 0X98 },
    { 0X00B4, 0X30 },
    { 0X00B8, 0X01 },
    { 0X00BC, 0XEF },
    { 0X00BD, 0X4B },
    { 0X00BE, 0XD2 },
    { 0X00C1, 0X30 },
    { 0X00C3, 0X80 },
    { 0X00C8, 0X17 },
    { 0X00C9, 0X00 },
    { 0X00CA, 0X00 },
    { 0X00CB, 0X00 },
    { 0X00CC, 0X17 },
    { 0X00CD, 0X00 },
    { 0X00CE, 0X00 },
    { 0X00CF, 0X00 },
    { 0X00D3, 0X28 },

    /* Begin PLL Re-lock Sequence */
    /* MCPLL re-lock sequence:*/
    { 0x03E5, 0x04 },
    { 0x03E5, 0x00 },

    /* APLL1 re-lock sequence:*/
    { 0X00C0 , 0X01 },
    { 0X00D0 , 0X0A },
    { 0X00D0 , 0X8A },
