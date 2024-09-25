/*
 * Support for indirect PCI bridges.
 *
 * Copyright (C) 1998 Gabriel Paubert.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <common.h>


#include <asm/processor.h>
#include <asm/io.h>
#include <pcie.h>
#include "pex/mvPexRegs.h"
#include "pex/mvPex.h"

#define cfg_pcie_read(val, addr, type, op)	*val = op((type)(addr))
#define cfg_pcie_write(val, addr, type, op)	op((type *)(addr), (val))

#define in_8(off) (*((vu_char *)(off)))
#define in_le16(off) (*((vu_short *)(off)))
#define in_le32(off) (*((vu_long *)(off)))
#define out_8(off, val) (*((vu_char *)(off)) = ((vu_char)val))
#define out_le16(off, val) (*((vu_short *)(off)) = ((vu_short)val))
#define out_le32(off, val) (*((vu_long *)(off)) = ((vu_long)val))

#if defined(CONFIG_MARVELL)
int pcie_check (int dev)
{
	unsigned int bus_val = (dev >> 16) & 0xFF;
	unsigned int dev_val = (dev >> 11) & 0x1F;
	unsigned int dl_status = 0;
	if (((bus_val == 0) && (dev_val > 1)) || (bus_val > 0x7F))
	{
		return -1;
	}
	if ((bus_val != 0) || (dev_val != 0))
	{
		dl_status = MV_REG_READ(PEX_STATUS_REG(0));
		if ((dl_status & 0x1))
		{
			return -1;
		}
	}
	return 0;
}
#endif

#if defined(CONFIG_E500)
#define INDIRECT_PCIE_OP(rw, size, type, op, mask)                        \
static int                                                               \
indirect_pcie_##rw##_config_##size(struct pcie_controller *hose,               \
			      pcie_dev_t dev, int offset, type val)       \
{                                                                        \
	*(hose->cfg_addr) = dev | (offset & 0xfc) | 0x80000000 |((offset & 0xf00)<<16);          \
	sync();                                                          \
	cfg_pcie_##rw(val, hose->cfg_data + (offset & mask), type, op);       \
	return 0;                                                        \
}
#else
#if defined(CONFIG_MARVELL)
#define INDIRECT_PCIE_OP(rw, size, type, op, mask)			 \
static int								 \
indirect_pcie_##rw##_config_##size(struct pcie_controller *hose, 		 \
			      pcie_dev_t dev, int offset, type val)	 \
{									 \
	if (pcie_check(dev))   \
	{								\
		return -1;                              \
	}								\
	out_le32(hose->cfg_addr, dev | (offset & 0xfc) | 0x80000000 |((offset & 0xf00)<<16)); 	 \
	cfg_pcie_##rw(val, hose->cfg_data + (offset & mask), type, op);	 \
	return 0;    					 		 \
}
#else
#define INDIRECT_PCIE_OP(rw, size, type, op, mask)			 \
static int								 \
indirect_pcie_##rw##_config_##size(struct pcie_controller *hose, 		 \
			      pcie_dev_t dev, int offset, type val)	 \
{									 \
	out_le32(hose->cfg_addr, dev | (offset & 0xfc) | 0x80000000 |((offset & 0xf00)<<16)); 	 \
	cfg_pcie_##rw(val, hose->cfg_data + (offset & mask), type, op);	 \
	return 0;    					 		 \
}
#endif
#endif

INDIRECT_PCIE_OP(read, byte, unsigned char *, in_8, 3)
INDIRECT_PCIE_OP(read, word, unsigned short *, in_le16, 2)
INDIRECT_PCIE_OP(read, dword, unsigned int *, in_le32, 0)
INDIRECT_PCIE_OP(write, byte, unsigned char, out_8, 3)
INDIRECT_PCIE_OP(write, word, unsigned short, out_le16, 2)
INDIRECT_PCIE_OP(write, dword, unsigned int, out_le32, 0)

void pcie_setup_indirect(struct pcie_controller* hose, unsigned int cfg_addr, unsigned int cfg_data)
{
	pcie_set_ops(hose,
		    indirect_pcie_read_config_byte,
		    indirect_pcie_read_config_word,
		    indirect_pcie_read_config_dword,
		    indirect_pcie_write_config_byte,
		    indirect_pcie_write_config_word,
		    indirect_pcie_write_config_dword);

	hose->cfg_addr = (unsigned int *) cfg_addr;
	hose->cfg_data = (unsigned char *) cfg_data;
}


