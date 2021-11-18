#
# (C) Copyright 2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

MINIMAL=

ifdef CONFIG_SPL_BUILD 
MINIMAL=y
endif

ifdef CONFIG_SPL_INIT_MINIMAL
MINIMAL=y
endif

ifdef MINIMAL
# necessary to create built-in.o
obj- := __dummy__.o
else
obj-y  +=  board_phyinv.o cmd_debug.o cmd_phyinv.o cmd_reboot.o phyinv.o
obj-y  += cmd_bootmmc.o lptm21.o cpld_burn.o i2c_cpld.o
obj-$(CONFIG_ADVA_CMD_BOOTUBI) += cmd_bootubi.o
obj-${CONFIG_ADVA_DS31407} += cmd_ds31xxx.o
endif

