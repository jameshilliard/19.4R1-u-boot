/*
 * Copyright (c) 2007-2010, Juniper Networks, Inc.
 * All rights reserved.
 *
 * Copyright 2006 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
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

#include <common.h>

#ifdef CONFIG_SRX3000
#ifdef CONFIG_HARD_I2C

#include <command.h>
#include <i2c.h>		/* Functional interface */ 

#include <asm/io.h>
#include "fsl_i2c.h"	/* HW definitions */

#define I2C_TIMEOUT	(CFG_HZ / 4)
fsl_i2c_t * mpc_i2c = (fsl_i2c_t*)(CFG_CCSRBAR + CFG_I2C_OFFSET);

DECLARE_GLOBAL_DATA_PTR;

static int current_controller = 1;

int current_i2c_controller (void)
{
	return current_controller;
}

void i2c_controller (int controller)
{
	mpc_i2c = (controller == CFG_I2C_CTRL_2) ? 
					((fsl_i2c_t*)(CFG_CCSRBAR + CFG_I2C2_OFFSET)) :
					((fsl_i2c_t*)(CFG_CCSRBAR + CFG_I2C_OFFSET));
	current_controller = (controller == CFG_I2C_CTRL_2) ? 
					CFG_I2C_CTRL_2 : CFG_I2C_CTRL_1;
}

void
i2c_init (int speed, int slaveadd)
{
	int i;
	
	for (i=0; i<2; i++)
	{
		i2c_controller(i==0?CFG_I2C_CTRL_2:CFG_I2C_CTRL_1);
		
		/* stop I2C controller */
		writeb(0x0, &I2C->cr);

		udelay(5);
	   
		/* set clock */
		writeb(0x2c, &I2C->fdr);

		/* set default filter */
		writeb(0x10, &I2C->dfsrr);

		/* write slave address */
		writeb(slaveadd, &I2C->adr);

		/* clear status register */
		writeb(0x0, &I2C->sr);

		/* start I2C controller */
		writeb(I2C_CR_MEN, &I2C->cr);
	}
}

static __inline__ int
i2c_wait4bus (void)
{
	ulong timeval = get_timer(0);

	while (readb(&I2C->sr) & I2C_SR_MBB) {
		if (get_timer(timeval) > I2C_TIMEOUT) {
			return -1;
		}
	}

	return 0;
}

static __inline__ int
i2c_wait (int write)
{
	u32 csr;
	ulong timeval = get_timer(0);

	do {
		csr = readb(&I2C->sr);
		if (!(csr & I2C_SR_MIF))
			continue;

		writeb(0x0, &I2C->sr);

		if (csr & I2C_SR_MAL) {
			debug("i2c_wait: MAL\n");
			return -1;
		}

		if (!(csr & I2C_SR_MCF))	{
			debug("i2c_wait: unfinished\n");
			return -1;
		}

		if (write == I2C_WRITE && (csr & I2C_SR_RXAK)) {
			debug("i2c_wait: No RXACK\n");
			return -1;
		}

		return 0;
	} while (get_timer (timeval) < I2C_TIMEOUT);

	debug("i2c_wait: timed out\n");
	return -1;
}

static __inline__ int
i2c_write_addr (u8 dev, u8 dir, int rsta)
{
	writeb(I2C_CR_MEN | I2C_CR_MSTA | I2C_CR_MTX
		   | (rsta ? I2C_CR_RSTA : 0),
		   &I2C->cr);

	writeb((dev << 1) | dir, &I2C->dr);

	if (i2c_wait(I2C_WRITE) < 0)
		return 0;

	return 1;
}

static __inline__ int
__i2c_write (u8 *data, int length)
{
	int i;

	writeb(I2C_CR_MEN | I2C_CR_MSTA | I2C_CR_MTX,
		   &I2C->cr);

	for (i = 0; i < length; i++) {
		writeb(data[i], &I2C->dr);

		if (i2c_wait(I2C_WRITE) < 0)
			break;
	}

	return i;
}

static __inline__ int
__i2c_read (u8 *data, int length)
{
	int i;

	writeb(I2C_CR_MEN | I2C_CR_MSTA | ((length == 1) ? I2C_CR_TXAK : 0),
		   &I2C->cr);

	/* dummy read */
	readb(&I2C->dr);

	for (i = 0; i < length; i++) {
		if (i2c_wait(I2C_READ) < 0)
			break;

		/* Generate ack on last next to last byte */
		if (i == length - 2)
			writeb(I2C_CR_MEN | I2C_CR_MSTA | I2C_CR_TXAK,
				   &I2C->cr);

		/* Generate stop on last byte */
		if (i == length - 1)
			writeb(I2C_CR_MEN | I2C_CR_TXAK, &I2C->cr);

		data[i] = readb(&I2C->dr);
	}

	return i;
}

int
i2c_read (u8 dev, uint addr, int alen, u8 *data, int length)
{
	int i = -1;
	u8 *a = (u8*)&addr;

	if (i2c_wait4bus() >= 0
		&& i2c_write_addr(dev, I2C_WRITE, 0) != 0
		&& __i2c_write(&a[4 - alen], alen) == alen)
		i = 0;
	
	if (length && i2c_write_addr(dev, I2C_READ, 1) != 0) {
		i = __i2c_read(data, length);
	}

	writeb(I2C_CR_MEN, &I2C->cr);

	if (i == length)
		return 0;

	return -1;
}

int
i2c_write (u8 dev, uint addr, int alen, u8 *data, int length)
{
	int i = -1;
	u8 *a = (u8*)&addr;

	if (i2c_wait4bus() >= 0
		&& i2c_write_addr(dev, I2C_WRITE, 0) != 0
		&& __i2c_write(&a[4 - alen], alen) == alen) {
		i = __i2c_write(data, length);
	}

	writeb(I2C_CR_MEN, &I2C->cr);

	if (i == length)
		return 0;

	return -1;
}

int
i2c_probe (uchar chip)
{
	int tmp;

	/*
	 * Try to read the first location of the chip.  The underlying
	 * driver doesn't appear to support sending just the chip address
	 * and looking for an <ACK> back.
	 */
	udelay(10000);

	return i2c_read(chip, 0, 1, (uchar *)&tmp, 1);
}

int
i2c_probe_java (uint i2c_ctrl, uchar sw_addr, uchar ch_index, uchar chip)
{
	int tmp, ret=1;
	unsigned char ch;
	/*
	 * Try to read the first location of the chip.  The underlying
	 * driver doesn't appear to support sending just the chip address
	 * and looking for an <ACK> back.
	 */
	udelay(10000);

	i2c_controller(i2c_ctrl);
	ch= 1 << ch_index;
	i2c_write(sw_addr, 0, 0, &ch, 1); /* enable the coresponding channel on the swtich*/

	ret= i2c_read(chip, 0, 0, (uchar *)&tmp, 1);
	
	ch=0x0;
	i2c_write(sw_addr, 0, 0, &ch, 1);	/*disable the coresponding channel on the switch*/
	return (ret);
}

uchar
i2c_reg_read (uchar i2c_addr, uchar reg)
{
	uchar buf[1];

	if (i2c_read(i2c_addr, reg, 1, buf, 1) == -1) {
		buf[0] = -1;
	}

	return buf[0];
}

int
i2c_reg_write (uchar i2c_addr, uchar reg, uchar val)
{
	return i2c_write(i2c_addr, (uchar)(reg & 0x3f), 1, &val, 1);
}

#endif /* CONFIG_HARD_I2C */
#endif /* CONFIG_SRX3000 */
