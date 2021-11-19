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
#include <linux/stringify.h>

int ubiBootQuiet = 0;

static int
do_bootubi (struct cmd_tbl * cmdtp, int flag, int argc, char * const argv[])
{
    int rc;

    const char* ubiskdi = env_get("ubiskdi");
    const char* ubifdt  = env_get("ubifdt");
    const char* ubibootverbose = env_get("ubibootverbose");
    const char* extraargs = env_get("extraargs");

    u8 bootstandby = (strstr(extraargs, "bootstandby") ? 1 : 0);
	
    if (ubiskdi == NULL)
        env_set("ubiskdi", CONFIG_SKDIFILE);

    if (ubifdt == NULL)
        env_set("ubifdt", CONFIG_FDTFILE);

    if (ubibootverbose == NULL)
        ubiBootQuiet = 1;

    puts("\nMounting boot filesystem ...\n");

    rc = run_command_list("ubi part ubi", -1, 0);
    if (rc == 0)
    {
        rc = run_command_list("ubifsmount ubi:common", -1, 0);
        if (rc == 0)
        {
            rc = run_command_list("ubifsload " __stringify(CONFIG_LOADADDR) " active", -1, 0);
            if (rc == 0)
            {
                u8* active_byte = (u8 *)CONFIG_LOADADDR;
                rc = run_command_list("ubifsumount", -1, 0);
                if (rc == 0)
                {
                    int active = (bootstandby?  !(*active_byte - '0') : (*active_byte - '0'));
                    char *mnt_cmd = (active == 0) ?  "ubifsmount ubi:boot0" :
                                    (active == 1) ?  "ubifsmount ubi:boot1" : NULL;

                    if (mnt_cmd == NULL)
                    {
                        printf("Unable to determine active partition group\n");
                        rc = 1;
                    }
                    else
                    {
                        rc = run_command_list(mnt_cmd, -1, 0);
                        if (rc == 0)
                        {
                            rc = run_command_list("ubifsload $kdiaddr $ubiskdi", -1, 0);
                            if (rc == 0)
                            {
                                rc = run_command_list("ubifsload $fdtaddr $ubifdt", -1, 0);
                                if (rc == 0)
                                {
                                    rc = run_command_list("run setbootargs; bootm $kdiaddr - $fdtaddr", -1, 0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    ubiBootQuiet = 0;

    return rc;
}

U_BOOT_CMD (bootubi, 1, 0, do_bootubi, NULL, NULL);
