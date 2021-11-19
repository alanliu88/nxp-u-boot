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
#include <common.h>
#include <command.h>
#include <i2c.h>
#include <linux/ctype.h>
#include <cli.h>
#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#include "cpld.h"

DECLARE_GLOBAL_DATA_PTR;

extern int set_board_debug (int val);

void _board_reboot(unsigned long type)
{
	;
}

void board_reboot(unsigned long type)
	__attribute__((weak, alias("_board_reboot")));

/*******************************************************************/
/* reboot - Reboot the system */
/*******************************************************************/
static int
do_reboot (struct cmd_tbl * cmdtp, int flag, int argc, char * const argv[])
{
	if (argc == 2)
	{
		if (strcmp (argv[1], "power") == 0)
		{
			puts ("Power cycling the board.\n");
			board_reboot(0);
			puts ("failed\n");
		}
		else if (strcmp (argv[1], "cold") == 0)
		{
			puts("Hard resetting the board.\n");
			board_reboot(1);
			puts ("failed\n");
		}
	}
	else
	{
		do_reset (NULL, 0, 0, NULL);
	}

	return 0;
}

U_BOOT_CMD (reboot, 2, 0, do_reboot,
	"reset the system",
	"[ cold | power ]\n"
);

/*******************************************************************/
/* boot - boot the system */
/*******************************************************************/
#if !defined(CONFIG_CMD_BOOTD)
int do_bootd (struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int rcode = 0;

	while (argc >= 2)
	{
		int len;
		char buf[CONFIG_SYS_CBSIZE];
		char *p;

		buf[0] = '\0';
		for (p = argv[1]; *p; p++)
		{
			*p = tolower(*p);
		}
		if (strcmp (argv[1], "factorydefaults") == 0)
		{
			puts ("Warning: all saved configuration will be erased.\n");
			len = cli_readline_into_buffer ("Continue (yes/no)? ", buf, 5);
			if ((len > 0) && (strcmp (buf, "yes") == 0))
			{
				buf[0] = '\0';
				env_get_f("extraargs", buf, sizeof(buf));
				strncat (buf, " factorydb", sizeof(buf));
				env_set("extraargs", buf);
			}
			else
			{
				puts ("Aborted\n");
				return 0;
			}
		}
		else if (strcmp (argv[1], "standby") == 0)
		{
			puts ("Warning: the standby software will become the active software.\n");
			len = cli_readline_into_buffer ("Continue (yes/no)? ", buf, 5);
			if ((len > 0) && (strcmp (buf, "yes") == 0))
			{
				buf[0] = '\0';
				env_get_f ("extraargs", buf, sizeof(buf));
				strncat (buf, " bootstandby", sizeof(buf));
				env_set ("extraargs", buf);
			}
			else
			{
				puts ("Aborted\n");
				return 0;
			}
		}
		else
		{
			cmd_usage (cmdtp);
			return 1;
		}
		argv++;
		argc--;
	}

#ifdef CONFIG_DEBUG_CMD
	{
		int cur_value = set_board_debug(1);
#endif

	const char* bootcmd = CONFIG_BOOTCOMMAND;

#ifndef CONFIG_SYS_HUSH_PARSER
		if (run_command (bootcmd, flag) < 0)
			rcode = 1;
#else
		if (parse_string_outer (bootcmd,
			FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) != 0)
			rcode = 1;
#endif
#ifdef CONFIG_DEBUG_CMD
		set_board_debug(cur_value);
	}
#endif
	return rcode;
}

U_BOOT_CMD(
	boot, 3, 0, do_bootd,
	"boot the system",
	"[ factoryDefaults ] [ standby ]\n"
);
#endif
