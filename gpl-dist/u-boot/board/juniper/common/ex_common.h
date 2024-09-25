/*
 * $Id$
 *
 * ex_common.h - platform code common for all the EX platforms
 *
 * Copyright (c) 2010-2013, Juniper Networks, Inc.
 * All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>
 */

#ifndef __EX_COMMON__H__
#define __EX_COMMON__H__

#define MBR_BUF_SIZE		512
#define MAX_NUM_SLICES		4
#define IS_NUMERAL(num) 	(num >= 0 && num <= 9 ? TRUE : FALSE)
#define NBASE_DECIMAL		10
#define BOOTSEQ_DISABLE_ENV	"boot.btsq.disable"

#define NUM_DISK_PART		    4
#define DISK_PART_SIZE		    0x10
#define DISK_PART_TBL_OFFSET	    0x1be
#define DISK_PART_MAGIC_OFFSET	    0x1fe
#define DISK_PART_MAGIC		    0xAA55
#define ACTIVE_FLAG		    0x80

#if !defined(CONFIG_EX62XX)
/*--------------- USB OHCI/EHCI ----------------*/
#define swap_ulong(x) \
  ({ unsigned long x_ = (unsigned long)x; \
   (unsigned long)( \
	((x_ & 0x000000FFUL) << 24) | \
	((x_ & 0x0000FF00UL) <<  8) | \
	((x_ & 0x00FF0000UL) >>  8) | \
	((x_ & 0xFF000000UL) >> 24) ); \
   })

#undef  readl
#define readl(a)    swap_ulong(*((vu_long *)(a)))
#undef  writel
#define writel(a, b)    (*((vu_long *)(b)) = swap_ulong((vu_long)a))
#endif

typedef struct disk_part {
    uint8_t	boot_flag;	    /* active				*/
    uint8_t	head;		    /* start head			*/
    uint8_t	sector;		    /* start sector			*/
    uint8_t	cylinder;	    /* start cylinder			*/
    uint8_t	part_type;	    /* type of partition.		*/
    uint8_t	end_head;	    /* end of head			*/
    uint8_t	end_sector;	    /* end of sector			*/
    uint8_t	end_cylinder;	    /* end of cylinder			*/
    uint8_t	start_sec[4];	    /* start sector from 0		*/
    uint8_t	size_sec[4];	    /* number of sectors in partition	*/
} disk_part_t;

typedef int (* disk_read_vector) (int dev, int lba, int totcnt, char *buf);

#if defined(CONFIG_EX62XX)
extern void e500_watchdog_enable(void);
extern void e500_watchdog_disable(void);
extern void e500_watchdog_reset(void);
#endif

int atoh(char *string);
int atod(char *string);
int twos_complement(uint8_t temp);
int twos_complement_16(uint16_t temp);

#endif /* __EX_COMMON__H__ */