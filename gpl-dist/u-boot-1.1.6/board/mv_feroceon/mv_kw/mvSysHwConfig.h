/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/
/*******************************************************************************
* mvSysHwCfg.h - Marvell system HW configuration file
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCmvSysHwConfigh
#define __INCmvSysHwConfigh

/****************************************/
/* Soc supporetd Units definitions	*/
/****************************************/
#undef MV_MEM_OVER_PEX_WA

#define MV_INCLUDE_PEX
#define MV_INCLUDE_GIG_ETH
#define MV_INCLUDE_CESA
#define MV_INCLUDE_USB
#define MV_INCLUDE_TWSI
#define MV_INCLUDE_NAND
#define MV_INCLUDE_UART
#define MV_INCLUDE_SPI
#define MV_INCLUDE_TDM
#define MV_INCLUDE_XOR
#define MV_INCLUDE_TS
#define MV_INCLUDE_AUDIO
#define MV_INCLUDE_RTC
#define MV_INCLUDE_SATA
#define MV_INCLUDE_INTEG_SATA
#define MV_INCLUDE_CLK_PWR_CNTRL

/*********************************************/
/* Board Specific defines : On-Board devices */
/*********************************************/

/* DRAM ddim detection support */
#define MV_INC_BOARD_DDIM
/* On-Board NAND Flash support */
#define MV_INC_BOARD_NAND_FLASH
/* On-Board SPI Flash support */
#define MV_INC_BOARD_SPI_FLASH
/* On-Board RTC */
#define MV_INC_BOARD_RTC

/* PEX-PCI\PCI-PCI Bridge*/
#define PCI0_IF_PTP		0		/* no Bridge on pciIf0*/
#define PCI1_IF_PTP		0		/* no Bridge on pciIf1*/

/************************************************/
/* U-Boot Specific				*/
/************************************************/
#define MV_INCLUDE_MONT_EXT

#if defined(MV_INCLUDE_MONT_EXT)
#define MV_INCLUDE_MONT_MMU
#define MV_INCLUDE_MONT_MPU
#if defined(MV_INC_BOARD_NOR_FLASH)
#define MV_INCLUDE_MONT_FFS
#endif
#endif


/************************************************/
/* RD boards specifics 				*/
/************************************************/

#undef MV_INC_BOARD_DDIM

#ifndef MV_BOOTROM
#define MV_STATIC_DRAM_ON_BOARD
#endif

#if defined(RD_88F6281)
#define MV_INC_BOARD_QD_SWITCH
#endif


#if defined(MV98DX) /* xCat Boards */
#define MV_PRESTERA_SWITCH
#define MV_INCLUDE_SAGE
#define PRESTERA_CASCADE_STACK_PORTS
#endif

#if defined(EX2200) || defined(RPS)
//#undef MV_PRESTERA_SWITCH
//#undef MV_INCLUDE_SAGE
//#undef PRESTERA_CASCADE_STACK_PORTS
#undef MV_INCLUDE_CESA
#undef MV_INCLUDE_NAND
#undef MV_INCLUDE_TDM
#undef MV_INCLUDE_XOR
#undef MV_INCLUDE_TS
#undef MV_INCLUDE_AUDIO
#undef MV_INCLUDE_SATA
#undef MV_INCLUDE_INTEG_SATA
#undef MV_INCLUDE_CLK_PWR_CNTRL
#undef MV_INCLUDE_RTC
#endif

/* 
 *  System memory mapping 
 */

/* SDRAM: actual mapping is auto detected */
#define SDRAM_CS0_BASE  0x00000000
#define SDRAM_CS0_SIZE  _256M

#define SDRAM_CS1_BASE  0x10000000
#define SDRAM_CS1_SIZE  _256M

#define SDRAM_CS2_BASE  0x20000000
#define SDRAM_CS2_SIZE  _256M

#define SDRAM_CS3_BASE  0x30000000
#define SDRAM_CS3_SIZE  _256M

/* PEX */
#define PEX0_MEM_BASE 0x90000000
#define PEX0_MEM_SIZE _256M

#define PEX0_IO_BASE 0xf0000000
#define PEX0_IO_SIZE _16M

#define PEX1_MEM_BASE 0x98000000
#define PEX1_MEM_SIZE _128M

#define PEX1_IO_BASE 0xf2000000
#define PEX1_IO_SIZE _16M

/* PEX Work arround */
/* the target we will use for the workarround */
#define PEX_CONFIG_RW_WA_TARGET PEX0_MEM
/*a flag that indicates if we are going to use the 
size and base of the target we using for the workarround
window */
#define PEX_CONFIG_RW_WA_USE_ORIGINAL_WIN_VALUES 1
/* if the above flag is 0 then the following values
will be used for the workarround window base and size,
otherwise the following defines will be ignored */
#define PEX_CONFIG_RW_WA_BASE 0x50000000
#define PEX_CONFIG_RW_WA_SIZE _16M

/* Device: CS0 - NAND, CS1 - SPI, CS2 - Boot ROM, CS3 - Boot device */

#define DEVICE_CS0_BASE 0xf9000000
#define DEVICE_CS0_SIZE _8M

#define DEVICE_SPI_BASE 0xf8000000
#define DEVICE_CS1_BASE DEVICE_SPI_BASE
#define DEVICE_CS1_SIZE _16M

#define DEVICE_CS2_BASE 0xf4000000
#define DEVICE_CS2_SIZE _1M

#define DEVICE_CS3_BASE BOOTDEV_CS_BASE
#define DEVICE_CS3_SIZE BOOTDEV_CS_SIZE

#if !defined(MV_BOOTROM) && defined(MV_NAND_BOOT)
#define CFG_NAND_BASE 	BOOTDEV_CS_BASE
#else
#define CFG_NAND_BASE 	DEVICE_CS0_BASE
#endif

/* Internal registers: size is defined in Controllerenvironment */
#define INTER_REGS_BASE	0xF1000000


#define CRYPT_ENG_BASE	0xFB000000
#define CRYPT_ENG_SIZE	_64K

#define SAGE_UNIT_BASE	0xF4000000
#define SAGE_UNIT_SIZE	_64M

#if defined (MV_INCLUDE_PEX)
#define PCI_IF0_MEM0_BASE 	PEX0_MEM_BASE
#define PCI_IF0_MEM0_SIZE 	PEX0_MEM_SIZE
#define PCI_IF0_IO_BASE 	PEX0_IO_BASE
#define PCI_IF0_IO_SIZE 	PEX0_IO_SIZE
#endif

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE

#define PCI_ARBITER_CTRL    /* Use/unuse the Marvell integrated PCI arbiter	*/
#undef	PCI_ARBITER_BOARD	/* Use/unuse the PCI arbiter on board			*/

/* Check macro validity */
#if defined(PCI_ARBITER_CTRL) && defined (PCI_ARBITER_BOARD)
	#error "Please select either integrated PCI arbiter or board arbiter"
#endif

/* Board clock detection */
#if defined(MV98DX) /* for xCat Boards */
#define TCLK_AUTO_DETECT    /* Use Tclk auto detection 		*/
#else
#undef TCLK_AUTO_DETECT    
#endif
#define SYSCLK_AUTO_DETECT /* Use SysClk auto detection 	*/
#define PCLCK_AUTO_DETECT  /* Use PClk auto detection */
#define L2CLK_AUTO_DETECT  /* Use L2 Clk auto detection */

/************* Switch driver configuration ********************/
#define PRESTERA_CACHED

/************* Ethernet driver configuration ********************/

/*#define ETH_JUMBO_SUPPORT*/
/* HW cache coherency configuration */
#define DMA_RAM_COHER	    NO_COHERENCY
#define DRAM_COHERENCY      MV_CACHE_COHER_SW
//#define ETHER_DRAM_COHER    MV_UNCACHED 
#define ETHER_DRAM_COHER    DRAM_COHERENCY /* MV_UNCACHED */
#define INTEG_SRAM_COHER    MV_UNCACHED  /* Where integrated SRAM available */

#define ETH_DESCR_IN_SDRAM
#undef  ETH_DESCR_IN_SRAM

#if (ETHER_DRAM_COHER == MV_CACHE_COHER_HW_WB)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_HW_WB"
#elif (ETHER_DRAM_COHER == MV_CACHE_COHER_HW_WT)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_HW_WT"
#elif (ETHER_DRAM_COHER == MV_CACHE_COHER_SW)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_SW"
#elif (ETHER_DRAM_COHER == MV_UNCACHED)
#   define ETH_SDRAM_CONFIG_STR      "MV_UNCACHED"
#else
#   error "Unexpected ETHER_DRAM_COHER value"
#endif /* ETHER_DRAM_COHER */

/*********** Idma default configuration ***********/
#define UBOOT_CNTRL_DMA_DV     (ICCLR_DST_BURST_LIM_8BYTE | \
				ICCLR_SRC_INC | \
				ICCLR_DST_INC | \
				ICCLR_SRC_BURST_LIM_8BYTE | \
				ICCLR_NON_CHAIN_MODE | \
				ICCLR_BLOCK_MODE )


/* CPU address decode table. Note that table entry number must match its    */
/* winNum enumerator. For example, table entry '4' must describe Deivce CS0 */
/* winNum which is represent by DEVICE_CS0 enumerator (4).                  */
#define MV_CPU_IF_ADDR_WIN_MAP_TBL	{				\
/*     base low      base high     size       	   WinNum     enable */ \
	{{SDRAM_CS0_BASE ,    0,      SDRAM_CS0_SIZE} ,0xFFFFFFFF,DIS}, \
	{{SDRAM_CS1_BASE ,    0,      SDRAM_CS1_SIZE} ,0xFFFFFFFF,DIS}, \
	{{SDRAM_CS2_BASE ,    0,      SDRAM_CS2_SIZE} ,0xFFFFFFFF,DIS}, \
	{{SDRAM_CS3_BASE ,    0,      SDRAM_CS3_SIZE} ,0xFFFFFFFF,DIS}, \
	{{PEX0_MEM_BASE  ,    0,      PEX0_MEM_SIZE } ,0x0       ,EN},	\
	{{PEX0_IO_BASE   ,    0,      PEX0_IO_SIZE  } ,0x2       ,EN},	\
	{{INTER_REGS_BASE,    0,      INTER_REGS_SIZE},0x8       ,EN},	\
	{{DEVICE_CS0_BASE,    0,      DEVICE_CS0_SIZE},0x1	 ,EN},	\
	{{DEVICE_CS1_BASE,    0,      DEVICE_CS1_SIZE},0x3	 ,EN},	\
	{{DEVICE_CS2_BASE,    0,      DEVICE_CS2_SIZE},0x5	 ,EN},	\
	{{DEVICE_CS3_BASE,    0,      DEVICE_CS3_SIZE},0x4       ,EN},	\
	{{CRYPT_ENG_BASE,     0,      CRYPT_ENG_SIZE} ,0x7  	 ,EN},	\
	{{SAGE_UNIT_BASE,     0,      SAGE_UNIT_SIZE} ,0x6  	 ,EN},	\
    /* Table terminator */\
    {{TBL_TERM, TBL_TERM, TBL_TERM}, TBL_TERM,TBL_TERM}			\
};

#define MV_CACHEABLE(address) ((address) | 0x80000000)

/* includes */
#define _1K         0x00000400
#define _4K         0x00001000
#define _8K         0x00002000
#define _16K        0x00004000
#define _32K        0x00008000
#define _64K        0x00010000
#define _128K       0x00020000
#define _256K       0x00040000
#define _512K       0x00080000

#define _1M         0x00100000
#define _2M         0x00200000
#define _4M         0x00400000
#define _8M         0x00800000
#define _16M        0x01000000
#define _32M        0x02000000
#define _64M        0x04000000
#define _128M       0x08000000
#define _256M       0x10000000
#define _512M       0x20000000

#define _1G         0x40000000
#define _2G         0x80000000


#if defined(MV_BOOTSIZE_256K)

#define BOOTDEV_CS_SIZE _256K

#elif defined(MV_BOOTSIZE_512K)

#define BOOTDEV_CS_SIZE _512K

#elif defined(MV_BOOTSIZE_4M)

#define BOOTDEV_CS_SIZE _4M

#elif defined(MV_BOOTSIZE_8M)

#define BOOTDEV_CS_SIZE _8M

#elif defined(MV_BOOTSIZE_16M)

#define BOOTDEV_CS_SIZE _16M

#elif defined(MV_BOOTSIZE_32M)

#define BOOTDEV_CS_SIZE _32M

#elif defined(MV_BOOTSIZE_64M)

#define BOOTDEV_CS_SIZE _64M

#elif defined(MV_NAND_BOOT)

#define BOOTDEV_CS_SIZE _512K

#else

#define Error MV_BOOTSIZE undefined

#endif                                               

#define BOOTDEV_CS_BASE	((0xFFFFFFFF - BOOTDEV_CS_SIZE) + 1)

/* We use the following registers to store DRAM interface pre configuration   */
/* auto-detection results													  */
/* IMPORTANT: We are using mask register for that purpose. Before writing     */
/* to units mask register, make sure main maks register is set to disable     */
/* all interrupts.                                                            */
#define DRAM_BUF_REG0	0x30810	/* sdram bank 0 size	        */  
#define DRAM_BUF_REG1	0x30820	/* sdram config			*/
#define DRAM_BUF_REG2   0x30830	/* sdram mode 			*/
#define DRAM_BUF_REG3	0x60bb0	/* dunit control low 	        */          
#define DRAM_BUF_REG4	0x60a90	/* sdram address control        */
#define DRAM_BUF_REG5	0x60a94	/* sdram timing control low     */
#define DRAM_BUF_REG6	0x60a98	/* sdram timing control high    */
#define DRAM_BUF_REG7	0x60a9c	/* sdram ODT control low        */
#define DRAM_BUF_REG8	0x60b90	/* sdram ODT control high       */
#define DRAM_BUF_REG9	0x60b94	/* sdram Dunit ODT control      */
#define DRAM_BUF_REG10	0x60b98	/* sdram Extended Mode		*/
#define DRAM_BUF_REG11	0x60b9c	/* sdram Ddr2 Time Low Reg      */
#define DRAM_BUF_REG12	0x60bb4	/* sdram Ddr2 Time High Reg     */
#define DRAM_BUF_REG13	0x60ab0	/* dunit Ctrl High        	*/
#define DRAM_BUF_REG14	0x60ab4	/* sdram second DIMM exist      */

/* Following the pre-configuration registers default values restored after    */
/* auto-detection is done                                                     */
#define DRAM_BUF_REG_DV    	0

#define ETH_DEF_TXQ    		0
#define ETH_DEF_RXQ    		0
#define MV_ETH_TX_Q_NUM		    1
#define MV_ETH_RX_Q_NUM		    1
#define ETH_NUM_OF_RX_DESCR     64
#define ETH_NUM_OF_TX_DESCR     ETH_NUM_OF_RX_DESCR*2

#define MV_CESA_MAX_BUF_SIZE	1600

#endif /* __INCmvSysHwConfigh */
