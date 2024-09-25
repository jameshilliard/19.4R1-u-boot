#
# Copyright 2004 Freescale Semiconductor.
#
# Copyright (c) 2006-2010, Juniper Networks, Inc.
# All rights reserved.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

#
# ex3242 board
#
sinclude $(OBJTREE)/board/$(BOARDDIR)/config.tmp

ifeq ($(CONFIG_UPGRADE),y) 
	TEXT_BASE = 0xFFE80000
else
	TEXT_BASE = 0xFFF80000
endif

PLATFORM_CPPFLAGS += -DCONFIG_E500=1
PLATFORM_CPPFLAGS += -DCONFIG_MPC85xx=1
PLATFORM_CPPFLAGS += -DCONFIG_MPC8544=1
PLATFORM_CPPFLAGS += -DCONFIG_EX3242=1