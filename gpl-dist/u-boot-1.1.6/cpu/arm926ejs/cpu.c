/*
 * Copyright (c) 2009-2011, Juniper Networks, Inc.
 * All rights reserved.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <arm926ejs.h>

#ifdef CONFIG_USE_IRQ
DECLARE_GLOBAL_DATA_PTR;
#endif

/* read co-processor 15, register #1 (control register) */
#if defined(CONFIG_EX2200) || \
    defined(CONFIG_EX3300) || \
    defined(CONFIG_RPS200)
unsigned long read_p15_c1 (void)
#else
static unsigned long read_p15_c1 (void)
#endif
{
	unsigned long value;

	__asm__ __volatile__(
		"mrc	p15, 0, %0, c1, c0, 0   @ read control reg\n"
		: "=r" (value)
		:
		: "memory");
#ifdef CONFIG_MARVELL
	__asm__ __volatile__("nop;nop;nop;nop;nop;nop;nop");
#endif

#ifdef MMU_DEBUG
	printf ("p15/c1 is = %08lx\n", value);
#endif
	return value;
}

/* write to co-processor 15, register #1 (control register) */
static void write_p15_c1 (unsigned long value)
{
#ifdef MMU_DEBUG
	printf ("write %08lx to p15/c1\n", value);
#endif
	__asm__ __volatile__(
		"mcr	p15, 0, %0, c1, c0, 0   @ write it back\n"
		:
		: "r" (value)
		: "memory");

	read_p15_c1 ();
}

static void cp_delay (void)
{
	volatile int i;

	/* copro seems to need some delay between reading and writing */
	for (i = 0; i < 100; i++);
}

/* See also ARM926EJ-S Technical Reference Manual */
#define C1_MMU		(1<<0)		/* mmu off/on */
#define C1_ALIGN	(1<<1)		/* alignment faults off/on */
#define C1_DC		(1<<2)		/* dcache off/on */

#define C1_BIG_ENDIAN	(1<<7)		/* big endian off/on */
#define C1_SYS_PROT	(1<<8)		/* system protection */
#define C1_ROM_PROT	(1<<9)		/* ROM protection */
#define C1_IC		(1<<12)		/* icache off/on */
#define C1_HIGH_VECTORS	(1<<13)		/* location of vectors: low/high addresses */


int cpu_init (void)
{
	/*
	 * setup up stacks if necessary
	 */
#ifdef CONFIG_USE_IRQ
#if defined(CONFIG_EX2200) || \
    defined(CONFIG_EX3300) || \
    defined(CONFIG_RPS200)
	IRQ_STACK_START = CFG_PT_BASE - 4;
#else
	IRQ_STACK_START = _armboot_start - CFG_MALLOC_LEN - CFG_GBL_DATA_SIZE - 4;
#endif
	FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;
#endif
	return 0;
}

int cleanup_before_linux (void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */

	unsigned long i;

	disable_interrupts ();

	/* turn off I/D-cache */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (i));
#ifdef CONFIG_MARVELL
	__asm__ __volatile__("nop;nop;nop;nop;nop;nop;nop");
#endif
	i &= ~(C1_DC | C1_IC);
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (i));

#ifdef CONFIG_MARVELL
	if (((i >> 4)&0xfff) == 0x926)
	{
		/* flush I/D-cache */
		i = 0;
		asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (i));
	}
	else
	{
		/* flush I/D-cache */
		i = 0;
		asm ("mcr p15, 0, %0, c7, c5, 0": :"r" (i));
		asm ("mcr p15, 0, %0, c7, c6, 0": :"r" (i));
	}
#else
	/* flush I/D-cache */
	i = 0;
	asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (i));
#endif

	return (0);
}

int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef CONFIG_MARVELL
	extern void reset_cpu (ulong addr);
#endif
	disable_interrupts ();
	reset_cpu (0);
	/*NOTREACHED*/
	return (0);
}

void icache_enable (void)
{
	ulong reg;

	reg = read_p15_c1 ();		/* get control reg. */
	cp_delay ();
	write_p15_c1 (reg | C1_IC);
}

void icache_disable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	write_p15_c1 (reg & ~C1_IC);
}

#if defined(CONFIG_EX2200) || \
    defined(CONFIG_EX3300) || \
    defined(CONFIG_RPS200)
void dcache_enable (void)
{
	ulong reg;

	reg = read_p15_c1 ();		/* get control reg. */
	cp_delay ();
	write_p15_c1 (reg | C1_DC);
}
#endif

void dcache_disable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	reg &= ~C1_DC;
	write_p15_c1 (reg);

}
int dcache_status (void)
{
	return (read_p15_c1 () & C1_DC) != 0;
}


int icache_status (void)
{
	return (read_p15_c1 () & C1_IC) != 0;
}
