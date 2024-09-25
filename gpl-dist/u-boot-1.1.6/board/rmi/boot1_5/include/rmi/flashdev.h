/***********************************************************************
Copyright 2003-2006 Raza Microelectronics, Inc.(RMI). All rights
reserved.
Use of this software shall be governed in all respects by the terms and
conditions of the RMI software license agreement ("SLA") that was
accepted by the user as a condition to opening the attached files.
Without limiting the foregoing, use of this software in source and
binary code forms, with or without modification, and subject to all
other SLA terms and conditions, is permitted.
Any transfer or redistribution of the source code, with or without
modification, IS PROHIBITED,unless specifically allowed by the SLA.
Any transfer or redistribution of the binary code, with or without
modification, is permitted, provided that the following condition is
met:
Redistributions in binary form must reproduce the above copyright
notice, the SLA, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution:
THIS SOFTWARE IS PROVIDED BY Raza Microelectronics, Inc. `AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL RMI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*****************************#RMI_3#***********************************/
#ifndef __FLASHDEV_H__
#define __FLASHDEV_H__
#include "rmi/board.h"


enum dev_type{
	NOR_FLASH = 0,
	NAND_FLASH = 1
};

struct flash_device
{
	char name[20];
	unsigned long iobase;
	int	chip_select;
	unsigned long flash_type;
	unsigned long size;
	/* some more flash info */
	unsigned long nr_blocks; // No of Blocks/Sectors
	unsigned int flash_id; /* page size without spare area */
	unsigned long nr_pages; /* no of pages per block */
	int (*reset) (struct flash_device *this_flash);
	int (*erase) (struct flash_device *this_flash, 
		      unsigned long address, 
		      unsigned long nBytes);
	int (*write) (struct flash_device *this_flash, 
		      unsigned long address, 
		      unsigned char *src,
		      unsigned long nBytes);
	int (*read) (struct flash_device *this_flash, 
		      unsigned long address, 
		      unsigned char *dst,
		      unsigned long nBytes);
	void (*iprint)(struct flash_device *this_flash);
	void *priv;
};

void rmi_cfi_flash_init(void);
void rmi_nand_flash_init(void);

#endif
