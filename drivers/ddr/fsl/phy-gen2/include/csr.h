/*
 * Copyright 2018 NXP
 * SPDX-License-Identifier:	GPL-2.0+        BSD-3-Clause
 *
 */

#ifndef _CSR_H_
#define _CSR_H_

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct csr {
	uint32_t addr;
	uint16_t data;
};

static const struct csr phy_csr[] = {
#ifdef CONFIG_FSL_PHY_GEN2_PHY_A2017_11
	{0x01005f,0x2ff},
	{0x01015f,0x2ff},
	{0x01105f,0x2ff},
	{0x01115f,0x2ff},
	{0x01205f,0x2ff},
	{0x01215f,0x2ff},
	{0x01305f,0x2ff},
	{0x01315f,0x2ff},
	{0x01405f,0x2ff},
	{0x01415f,0x2ff},
	{0x01505f,0x2ff},
	{0x01515f,0x2ff},
	{0x01605f,0x2ff},
	{0x01615f,0x2ff},
	{0x01705f,0x2ff},
	{0x01715f,0x2ff},
	{0x01805f,0x2ff},
	{0x01815f,0x2ff},
	{0x000055,0x3ff},
	{0x001055,0x3ff},
	{0x002055,0x3ff},
	{0x003055,0x3ff},
	{0x004055,0xff},
	{0x005055,0xff},
	{0x006055,0x3ff},
	{0x007055,0x3ff},
	{0x008055,0x3ff},
	{0x009055,0x3ff},
	{0x00a055,0x3ff},
	{0x00b055,0x3ff},
	{0x00200c5,0x19},
	{0x02002e,0x2},
	{0x020024,0x9},
	{0x02003a,0x2},
	{0x020056,0x3},
	{0x01004d,0x0},
	{0x01014d,0x0},
	{0x01104d,0x0},
	{0x01114d,0x0},
	{0x01204d,0x0},
	{0x01214d,0x0},
	{0x01304d,0x0},
	{0x01314d,0x0},
	{0x01404d,0x0},
	{0x01414d,0x0},
	{0x01504d,0x0},
	{0x01514d,0x0},
	{0x01604d,0x0},
	{0x01614d,0x0},
	{0x01704d,0x0},
	{0x01714d,0x0},
	{0x01804d,0x0},
	{0x01814d,0x0},
	{0x010049,0x618},
	{0x010149,0x618},
	{0x011049,0x618},
	{0x011149,0x618},
	{0x012049,0x618},
	{0x012149,0x618},
	{0x013049,0x618},
	{0x013149,0x618},
	{0x014049,0x618},
	{0x014149,0x618},
	{0x015049,0x618},
	{0x015149,0x618},
	{0x016049,0x618},
	{0x016149,0x618},
	{0x017049,0x618},
	{0x017149,0x618},
	{0x018049,0x618},
	{0x018149,0x618},
	{0x000043,0x3ff},
	{0x001043,0x3ff},
	{0x002043,0x3ff},
	{0x003043,0x3ff},
	{0x004043,0x3ff},
	{0x005043,0x3ff},
	{0x006043,0x3ff},
	{0x007043,0x3ff},
	{0x008043,0x3ff},
	{0x009043,0x3ff},
	{0x00a043,0x3ff},
	{0x00b043,0x3ff},
	{0x020018,0x1},
	{0x020075,0x2},
	{0x020050,0x0},
	{0x020008,0x320},
	{0x020088,0x9},
	{0x0200b2,0x208},
	{0x010043,0x5b1},
	{0x010143,0x5b1},
	{0x011043,0x5b1},
	{0x011143,0x5b1},
	{0x012043,0x5b1},
	{0x012143,0x5b1},
	{0x013043,0x5b1},
	{0x013143,0x5b1},
	{0x014043,0x5b1},
	{0x014143,0x5b1},
	{0x015043,0x5b1},
	{0x015143,0x5b1},
	{0x016043,0x5b1},
	{0x016143,0x5b1},
	{0x017043,0x5b1},
	{0x017143,0x5b1},
	{0x018043,0x5b1},
	{0x018143,0x5b1},
	{0x0200fa,0x1},
	{0x020019,0x4},
	{0x0200f0,0x5555},
	{0x0200f1,0x5555},
	{0x0200f2,0x5555},
	{0x0200f3,0x5555},
	{0x0200f4,0x5555},
	{0x0200f5,0x5555},
	{0x0200f6,0x5555},
	{0x0200f7,0xf000},
	{0x01004a,0x500},
	{0x01104a,0x500},
	{0x01204a,0x500},
	{0x01304a,0x500},
	{0x01404a,0x500},
	{0x01504a,0x500},
	{0x01604a,0x500},
	{0x01704a,0x500},
	{0x01804a,0x500},
	{0x020025,0x0},
	{0x02011b,0x7},
	{0x02002d,0x0},
	{0x020060,0x2},
#else
	{0x01005f,0x2ff},
	{0x01015f,0x2ff},
	{0x01105f,0x2ff},
	{0x01115f,0x2ff},
	{0x01205f,0x2ff},
	{0x01215f,0x2ff},
	{0x01305f,0x2ff},
	{0x01315f,0x2ff},
	{0x01405f,0x2ff},
	{0x01415f,0x2ff},
	{0x01505f,0x2ff},
	{0x01515f,0x2ff},
	{0x01605f,0x2ff},
	{0x01615f,0x2ff},
	{0x01705f,0x2ff},
	{0x01715f,0x2ff},
	{0x000055,0x3ff},
	{0x001055,0x3ff},
	{0x002055,0x3ff},
	{0x003055,0x3ff},
	{0x004055,0xff},
	{0x005055,0xff},
	{0x006055,0x3ff},
	{0x007055,0x3ff},
	{0x008055,0x3ff},
	{0x009055,0x3ff},
	{0x00a055,0x3ff},
	{0x00b055,0x3ff},
	{0x0200c5,0x19},
	{0x02002e,0x2},
	{0x020024,0x9},
	{0x02003a,0x2},
	{0x020056,0x3},
	{0x01004d,0x18},
	{0x01014d,0x18},
	{0x01104d,0x18},
	{0x01114d,0x18},
	{0x01204d,0x18},
	{0x01214d,0x18},
	{0x01304d,0x18},
	{0x01314d,0x18},
	{0x01404d,0x18},
	{0x01414d,0x18},
	{0x01504d,0x18},
	{0x01514d,0x18},
	{0x01604d,0x18},
	{0x01614d,0x18},
	{0x01704d,0x18},
	{0x01714d,0x18},
	{0x01804d,0x18},
	{0x01814d,0x18},
	{0x010049,0x618},
	{0x010149,0x618},
	{0x011049,0x618},
	{0x011149,0x618},
	{0x012049,0x618},
	{0x012149,0x618},
	{0x013049,0x618},
	{0x013149,0x618},
	{0x014049,0x618},
	{0x014149,0x618},
	{0x015049,0x618},
	{0x015149,0x618},
	{0x016049,0x618},
	{0x016149,0x618},
	{0x017049,0x618},
	{0x017149,0x618},
	{0x018049,0x618},
	{0x018149,0x618},
	{0x000043,0x3ff},
	{0x001043,0x3ff},
	{0x002043,0x3ff},
	{0x003043,0x3ff},
	{0x004043,0x3ff},
	{0x005043,0x3ff},
	{0x006043,0x3ff},
	{0x007043,0x3ff},
	{0x008043,0x3ff},
	{0x009043,0x3ff},
	{0x00a043,0x3ff},
	{0x00b043,0x3ff},
	{0x020018,0x1},
	{0x020075,0x2},
	{0x020050,0x0},
	{0x020008,0x320},
	{0x020088,0x9},
	{0x0200b2,0x288},
	{0x010043,0x5b1},
	{0x010143,0x5b1},
	{0x011043,0x5b1},
	{0x011143,0x5b1},
	{0x012043,0x5b1},
	{0x012143,0x5b1},
	{0x013043,0x5b1},
	{0x013143,0x5b1},
	{0x014043,0x5b1},
	{0x014143,0x5b1},
	{0x015043,0x5b1},
	{0x015143,0x5b1},
	{0x016043,0x5b1},
	{0x016143,0x5b1},
	{0x017043,0x5b1},
	{0x017143,0x5b1},
	{0x018043,0x5b1},
	{0x018143,0x5b1},
	{0x0200fa,0x1},
	{0x020019,0x4},
	{0x0200f0,0x5555},
	{0x0200f1,0x5555},
	{0x0200f2,0x5555},
	{0x0200f3,0x5555},
	{0x0200f4,0x5555},
	{0x0200f5,0x5555},
	{0x0200f6,0x5555},
	{0x0200f7,0xf000},
	{0x02000b,0x65},
	{0x02000c,0xc9},
	{0x02000d,0x7d1},
	{0x02000e,0x2c},
	{0x020025,0x0},
	{0x02002d,0x0},
	{0x020060,0x2}
#endif
};
#endif