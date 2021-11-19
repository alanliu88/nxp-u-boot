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
#include <command.h>

int mmcBootQuiet = 0;

static int
do_bootmmc (struct cmd_tbl * cmdtp, int flag, int argc, char * const argv[])
{
	int rc;

	const char* extraargs = env_get("extraargs");

	u8 bootstandby = (strstr(extraargs, "bootstandby") ? 1 : 0);
	
	rc = run_command_list("ext2load mmc 0:5  $loadaddr  active", -1, 0);
	if (rc == 0)
	{
		unsigned long config_loadaddr = 0;		
		config_loadaddr = env_get_hex("loadaddr", 0);
		if (config_loadaddr == 0)
		{
			printf("please set enviroment \"loadaddr\"");
			return 0;
		}
		
		u8* active_byte = (u8 *)config_loadaddr;
		int active = (bootstandby?	!(*active_byte - '0') : (*active_byte - '0'));
		if (active == 0)
		{
			rc = run_command_list("ext2load mmc 0:6 $kdiaddr $skdifile", -1, 0);
			if (rc == 0)
			{
				rc = run_command_list("ext2load mmc 0:6 $fdtaddr $fdtfile", -1, 0);
				if (rc == 0)
				{
					rc = run_command_list("run setbootargs; bootm $kdiaddr - $fdtaddr", -1, 0);
				}
			}
		}
		else
		{
			rc = run_command_list("ext2load mmc 0:b $kdiaddr $skdifile", -1, 0);
			if (rc == 0)
			{
				rc = run_command_list("ext2load mmc 0:b $fdtaddr $fdtfile", -1, 0);
				if (rc == 0)
				{
					rc = run_command_list("run setbootargs; bootm $kdiaddr - $fdtaddr", -1, 0);
				}
			}
		}
			
	}


    return rc;
}

U_BOOT_CMD (bootmmc, 1, 0, do_bootmmc, NULL, NULL);
