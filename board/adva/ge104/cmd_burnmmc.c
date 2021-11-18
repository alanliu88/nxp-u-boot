/*
 * Copyright (C) ADVA Optical Networking, Inc. 2014
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

#include <common.h>

#define BLOCK_SIZE  0xa00000

static int
do_burnmmc (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    int rc = 0;
	loff_t offset = 0;
	ulong rdlen = 0;

	ulong mmc_blk_off = 0;
	ulong mmc_blk_cnt = 0;

	env_set_hex("burnbsize", BLOCK_SIZE);
	
	do {
		offset += rdlen;
		env_set_hex("fileoffset", offset);
		rc = run_command_list("fatload usb 0:1 $loadaddr ge104-mmc.bin $burnbsize $fileoffset", -1, 0);
		rdlen = env_get_hex("filesize", 0);

		mmc_blk_cnt = rdlen/512;

		env_set_hex("mmcblkoff", mmc_blk_off);
		env_set_hex("mmcblkcnt", mmc_blk_cnt);
		
		rc = run_command_list("mmc write $fileaddr $mmcblkoff $mmcblkcnt", -1, 0);
		
		mmc_blk_off += rdlen/512;
		
	}while (rdlen == BLOCK_SIZE);


    return rc;
}

U_BOOT_CMD (burnmmc, 2, 0, do_burnmmc, NULL, NULL);

