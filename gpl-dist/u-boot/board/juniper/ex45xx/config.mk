#
# Copyright 2004 Freescale Semiconductor.
#
# Copyright (c) 2011-2013, Juniper Networks, Inc.
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
# ex45xx board
#
TEXT_BASE = 0xFFF80000

ifeq ($(CONFIG_UPGRADE),y) 
	CONFIG_SYS_TEXT_BASE = 0xFFE80000
	RESET_VECTOR_ADDRESS = 0xFFEFFFFC
else
	CONFIG_SYS_TEXT_BASE = 0xFFF80000
	RESET_VECTOR_ADDRESS = 0xFFFFFFFC
endif
