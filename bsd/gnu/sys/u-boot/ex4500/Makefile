# $Id: Makefile 584389 2013-06-04 05:34:41Z ib-builder $
#
# Copyright (c) 2012-2013, Juniper Networks, Inc.
# All rights reserved.
#

UPDATE_DEPENDFILE = NO

all: build_uboot.ex4500

build_uboot.ex4500: _BUILD_UBOOT_45XX_USE

_BUILD_UBOOT_45XX_USE: .META
XMAKE_META_FILE = build_uboot.ex4500.meta

.include "../Makefile.inc"

