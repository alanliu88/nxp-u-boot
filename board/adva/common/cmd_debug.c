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
#include <u-boot/md5.h>

/******************************************************************************
 * The u-boot command parser normally allows access to all commands.  This
 * implements a "debug" mode which disables most commands by default and
 * requires a password to be entered to gain access to the full set of
 * commands.  A number of patches to the u-boot code base are also needed
 * in addition to this file.
 *****************************************************************************/
#ifdef CONFIG_DEBUG_CMD

DECLARE_GLOBAL_DATA_PTR;

/*
 * Holds the current mode:
 *    -1 = debug is disabled and can not be enabled,
 *     0 = debug is disabled and can be enabled with password,
 *     1 = debug is enabled
 */
static int board_debug;
extern int phyinv_fipsmode(int value);

bool __is_fips_board(unsigned long type)
{
	return false;
}

bool is_fips_board(unsigned long type)
	__attribute__((weak, alias("__is_fips_board")));


static void
to_ascii(char *str, unsigned char *data, size_t data_len)
{
    int i;
    char *p = str;

    for (i = 0; i < data_len; i++, p += 2)
    {
        sprintf(p, "%02x", data[i]);
    }
}

static int
check_passwd(const char *pw)
{
    unsigned char buf[16 + 32 + 1]; /* 16 salt, 32 passwd, 1 null */
    unsigned char result[16];
    const char *hash;
    const char *salt;

    if ((hash = env_get("hash")) == NULL)
    {
        hash = CONFIG_DEBUG_PASSWORD;
    }
    salt = hash;
    hash += 16;

    buf[0] = '\0';
    strncpy((char *)buf, salt, 16);
    strncpy((char *)buf + 16, pw, 32);

    md5(buf, strlen((char *)buf), result);
    to_ascii((char *)buf, result, 16);

    return memcmp(buf, hash, 32);
}

static void
set_passwd(const char *pw)
{
    unsigned char buf[16 + 32 + 1]; /* 16 salt, 32 passwd, 1 null */
    unsigned char result[16];
    unsigned int r;

    srand(get_timer(0));
    r = rand();

    md5((unsigned char *)&r, sizeof(unsigned int), result);
    to_ascii((char *)buf, result, 16);

    strncpy((char *)buf + 16, pw, 32);

    if ((r = strlen(pw)) > 32)
    {
        r = 32;
    }

    md5(buf, r + 16, result);
    to_ascii((char *)buf + 16, result, 16);

    env_set("hash", (char *)buf);
}

/*
 * Basic command set available with debug mode disabled.
 */
static const char* simple_cmds[] = {
    "?",
    "help",
    "boot",
    "date",
    "mtest",
    "phyinv",
    "reboot",
    "version",
    0,
};

/*
 * Command filter hook called within common/command.c
 */
cmd_tbl_t *
board_check_cmd (cmd_tbl_t *p, int help)
{
    char **cmds;

    //filter out command "fips" in none fips board
    if ((strcmp (p->name, "fips") == 0) && !is_fips_board(gd->board_type))
    {
        return 0;
    }


    if (board_debug > 0)
        return p;

    if (help == 2)
        return 0;

    if ((strcmp (p->name, "fips") == 0) && (help == 0))
        return p;

    if ((strcmp (p->name, "debug") == 0) && (help == 0))
        return p;

    for (cmds = (char**)simple_cmds; *cmds; cmds++)
    {
        if (strcmp (p->name, *cmds) == 0)
        {
            return p;
        }
    }
    return 0;
}

/*
 * Initialize debug mode from environment setting.
 * Should be called from misc_init_r.
 */
void
init_board_debug (void)
{
    const char *env_debug = env_get ("debug");

    board_debug = (env_debug == NULL) ? 0 :
                  (strcmp (env_debug,  "on") == 0) ?  1 :
                  (strcmp (env_debug, "off") == 0) ? -1 : 0;
}

/*
 * This is used to temporarily enable debug mode by unprotected commands which
 * need to run protected commands - like "boot" which performs "run bootcmd".
 */
int
set_board_debug (int val)
{
    int ret;

    ret = board_debug;
    board_debug = val;

    return ret;
}

/*
 * Debug command handler to enable or disable debug mode.
 *
 * If the board_debug variable was set to -1 because debug = off is in the
 * environment, then the debug command is disabled with no way to gain access
 * to the full command set.
 */
static int
do_debug (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    if (board_debug < 0)
    {
        return 0;
    }

    if ((argc == 2) && (strcmp (argv[1], "on") == 0))
    {
        int i;
        char c;
        char buf[32 + 1];

        c = getc();
        for (i = 0; (i < 32) && (c != '\n') && (c != '\r'); i++)
        {
            buf[i] = c;
            c = getc();
        }
        buf[i] = '\0';

        if (check_passwd(buf) == 0)
        {
            printf ("debug mode enabled\n");
            board_debug = 1;
        }
    }
    else if (board_debug && (argc == 2) && (strcmp (argv[1], "off") == 0))
    {
        printf ("debug mode disabled\n");
        board_debug = 0;
    }
    else if (board_debug && (argc == 3) && (strcmp (argv[1], "set") == 0))
    {
        set_passwd(argv[2]);
    }

    return 0;
}

U_BOOT_CMD (debug, 3, 0, do_debug, NULL, NULL);

static int
do_fips (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    int len = 0;
    char buf[256];

    memset(buf, 0, 256);

    //if (gd->fips_mode == 1)
    {
        printf("fips mode enabled !\n");
        return 0;
    }

    if ((argc == 2) && (strcmp (argv[1], "on") == 0))
    {
        puts ("Warning: U-Boot command line will be prohibited.\n");
        //len = readline_into_buffer ("Continue (yes/no)? ", buf, 0);
        if ((len > 0) && (strcmp (buf, "yes") == 0))
        {        
            board_debug = -1;
            
            phyinv_fipsmode(1);

            //set_default_env(0);
            //setenv("debug", "off");
            //saveenv();
        
            puts("\nfips mode enabled !\n");

            mdelay(1000);
            run_command_list("reboot cold", -1, 0);
        }
        else
        {
            puts ("Aborted\n");
            return 0;
        }
    }

    return 0;
}

U_BOOT_CMD (fips, 3, 0, do_fips, NULL, NULL);

#endif /* CONFIG_DEBUG_CMD */
