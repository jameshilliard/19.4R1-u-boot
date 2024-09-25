/*
 * Copyright (c) 2011-2013, Juniper Networks, Inc.
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
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/gpl-2.0.html>.
 */

/*
 * ex4510 board configuration file
 */
#ifndef __CONFIG_H
#define __CONFIG_H

/* High Level Configuration Options */
#define CONFIG_BOOKE		1	/* BOOKE */
#define CONFIG_E500		1	/* BOOKE e500 family */
#define CONFIG_MPC85xx		1
#define CONFIG_P2020            1
#define CONFIG_P2020DS          1
#define CONFIG_EX45XX		1	/* ex45xx series */
#define CONFIG_EX4510		1	/* ex4510 board */
#define	CONFIG_PRODUCT_EXSERIES	1	/* EX series product */

#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE    0xfff80000
#endif

/* FIXME: define CONFIG_PRODUCTION once all dev is complete */
#undef CONFIG_PRODUCTION

#define CONFIG_FSL_ELBC
#define CONFIG_PCI				/* Enable PCI/PCIE */
#define CONFIG_PCI1
#define CONFIG_PCIE1
#define CONFIG_FSL_PCI_INIT	1	/* Use common FSL init code */
#define CONFIG_FSL_PCIE_RESET	1	/* need PCIe reset errata */
#define CONFIG_CMD_PCI

#define CONFIG_FSL_LAW		1	/* Use common FSL init code */

#define CONFIG_TSEC_ENET		/* tsec ethernet support */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_SYS_CLK_FREQ	100000000   /* sysclk */
#define	CONFIG_DDR_CLK_FREQ	100000000   /* ddrclk */

/*
 * These can be toggled for performance analysis, otherwise use default.
 */
#define CONFIG_L2_CACHE			/* toggle L2 cache 	*/
#define CONFIG_BTB			/* toggle branch predition */

#define CONFIG_ENABLE_36BIT_PHYS	1

/*
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 */
#define CONFIG_SYS_CCSRBAR_DEFAULT	0xff700000	/* CCSRBAR Default */
#define CONFIG_SYS_CCSRBAR		0xfef00000	/* relocated CCSRBAR */
/* physical addr of CCSRBAR */
#define CONFIG_SYS_CCSRBAR_PHYS		CONFIG_SYS_CCSRBAR
/* PQII uses CONFIG_SYS_IMMR */
#define CONFIG_SYS_IMMR			CONFIG_SYS_CCSRBAR


/* DDR Setup */
#undef  CFG_DRAM_TEST			/* memory test, takes time */
#define CONFIG_SYS_MEMTEST_START	0x10000000
#define CONFIG_SYS_MEMTEST_END		0x7fffffff

#define CONFIG_SYS_DDR_SDRAM_BASE	0x00000000
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_SDRAM_BASE

#define	CONFIG_DDR_ECC
#undef	CONFIG_ECC_INIT_VIA_DDRCONTROLLER
#define	CONFIG_MEM_INIT_VALUE		0xdeadbeef

/* DDR Setup */
#define CONFIG_VERY_BIG_RAM
/* 
 * force u-boot relocation address & stack to be within 1GB
 * of RAM so that u-boot and pre-maped kernel address can share the same
 * TLB entry.. 
 * [XXX:ex:- if RAM sz > 1GB and if u-boot relocates to top of the memory @ <2GB, 
 * loader will invalidate the TLB entry that maps 0-1GB where kernel will be
 * loaded. This will cause Instruction TLB error]
 */
/* relocate u-boot at the top of 1GB */
#define CONFIG_MEM_RELOCATE_TOP		(1024 << 20)
/* 
 * Assign fixed tlb index for mapping RAM.
 * Index should be > 4.
 * loader uses tlb1 entries[0-4] for pre-mapping the kernel
 */
#define CONFIG_RAM_TLB_INDEX		8

/*
 * When initializing flash, if we cannot find the manufacturer ID,
 * assume this is the AMD flash associated with the CDS board.
 * This allows booting from a promjet.
 */
#define CONFIG_ASSUME_AMD_FLASH
#define CONFIG_SYS_MAX_FLASH_BANKS      1	/* number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	128
#undef  CONFIG_SYS_FLASH_CHECKSUM
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms) */


#define CONFIG_CHIP_SELECTS_PER_CTRL	1

#undef CONFIG_SPD_EEPROM		/* Use SPD EEPROM for DDR setup*/
#define CONFIG_DDR_SPD
/* I2C addresses of SPD EEPROMs */
#define CONFIG_SYS_SPD_BUS_NUM	0		/* SPD EEPROM located on I2C bus 0 */

#undef CONFIG_CLOCKS_IN_MHZ

/*
 * Local Bus
 * Flash 0xff80_0000 - 0xffff_ffff 8M
 * EPLD  0xff00_0000 - 0xff00_ffff 64K
 */

#define CONFIG_SYS_EPLD_BASE		0xff000000
#define CONFIG_SYS_FLASH_BASE		0xff800000  /* start of FLASH 8M */
#define CONFIG_SYS_SDRAM_SIZE		2048	    /* DDR is 2G */

#define	CONFIG_SYS_LBC_BASE		0xff000000
#define CONFIG_SYS_LBC_BASE_PHYS	CONFIG_SYS_LBC_BASE
#define CONFIG_SYS_FLASH_BASE_PHYS	CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_EPLD_BASE_PHYS	CONFIG_SYS_EPLD_BASE

/* Manual Setup of DDR params */

#define CONFIG_SYS_DDR3_CS0_BNDS		0x0000007F
#define CONFIG_SYS_DDR3_CS0_CONFIG		0x80014302 	
#define CONFIG_SYS_DDR3_CS0_CONFIG_2		0x00000000
#define CONFIG_SYS_DDR3_TIMING_0		0x00330104
#define CONFIG_SYS_DDR3_TIMING_1		0x6f6b4846
#define CONFIG_SYS_DDR3_TIMING_2		0x0fa8c0cf
#define CONFIG_SYS_DDR3_TIMING_3		0x00040000	
#define CONFIG_SYS_DDR3_TIMING_4		0x00220001
#define CONFIG_SYS_DDR3_TIMING_5		0x02401400
#define CONFIG_SYS_DDR3_MODE			0x00061421
#define CONFIG_SYS_DDR3_MODE_2			0x04000000
#define CONFIG_SYS_DDR3_MD_CNTL 		0x00000000
#define CONFIG_SYS_DDR3_INTERVAL		0x09300100
#define CONFIG_SYS_DDR3_DATA_INIT		0xdeadbeef
#define CONFIG_SYS_DDR3_CLK_CNTL		0x02800000
#define CONFIG_SYS_DDR3_INIT_ADDR		0x00000000
#define CONFIG_SYS_DDR3_INIT_EXT_ADDR		0x00000000
#define CONFIG_SYS_DDR3_ZQ_CNTL 		0x88000000
#define CONFIG_SYS_DDR3_WRLVL_CNTL		0x8645a607
#define CONFIG_SYS_DDR3_SR_CNTR 		0x00000000
#define CONFIG_SYS_DDR3_RCW_1			0x00050000
#define CONFIG_SYS_DDR3_RCW_2			0x00000000
#define CONFIG_SYS_SDRAM_CFG			0xC7000000
#define CONFIG_SYS_SDRAM_CFG_2			0x24401054
#define CONFIG_SYS_SDRAM_CFG_INITIAL		0x47000000
#define CONFIG_SYS_SDRAM_CFG_NO_ECC		0xf7010000
#define CONFIG_SYS_DDR3_IP_REV1 		0x00020403
#define CONFIG_SYS_DDR3_IP_REV2 		0x00000100
#define CONFIG_SYS_DDR3_ERR_INT_EN		0x00000000
#define CONFIG_SYS_DDR3_CDR1			0x00010077
#define CONFIG_SYS_DDR3_CDR2			0x00000000
#define SDRAM_CFG_MEM_EN			0x80000000
#define CONFIG_SYS_DDR3_ERR_DISABLE		0x0000000d
#define CONFIG_SYS_DDR3_ERR_SBE 		0x00ff0000
#define CONFIG_SYS_DDR3_DINIT			0x00000010

/*
 * FLASH on the Local Bus
 * One banks, 8M, using the CFI driver.
 * Boot from BR0/OR0 bank at 0xff80_0000
 *
 * BR0:
 *    BA (Base Address) = 0xff80_0000 = BR0[0:16] = 1111 1111 1000 0000 0
 *    XBA (Address Space eXtent) = BR0[17:18] = 00
 *    PS (Port Size) = 8 bits = BR0[19:20] = 01
 *    DECC = disable parity check = BR0[21:22] = 00
 *    WP = enable both read and write protect = BR0[23] = 0
 *    MSEL = use GPCM = BR0[24:26] = 000
 *    ATOM = not use atomic operate = BR0[28:29] = 00
 *    Valid = BR0[31] = 1
 *
 * 0      4      8     12    16     20    24     28
 * 1111 1111 1000 0000 0000 1000 0000 0001 = ff800801    BR0
 *
 * OR0:
 *    Addr Mask = 8M = OR0[0:16] = 1111 1111 1000 0000 0
 *    Reserved OR0[17:18] = 00
 *    BCTLD = buffer control not assert = OR0[19] = 1
 *    CSNT = chip select /CS and /WE negated = OR0[20] = 0
 *    ACS = address to CS at same time as address line = OR0[21:22] = 00
 *    XACS = extra address setup = OR0[23] = 0
 *    SCY = 7 clock wait states = OR0[24:27] = 0100
 *    SETA (External Transfer Ack) = OR0[28] = 0
 *    TRLX = use relaxed timing = OR0[29] = 0
 *    EHTR (Extended hold time on read access) = OR0[30] = 0
 *    EAD = use external address latch delay = OR0[31] = 0
 *
 * 0      4      8     12    16     20    24     28
 * 1111 1111 1000 0000 0001 0000 0100 0000 = ff801040    OR0
 */

#define CONFIG_FLASH_BR_PRELIM  (BR_PHYS_ADDR(CONFIG_SYS_FLASH_BASE_PHYS) \
				 | BR_PS_8 | BR_V)
#define CONFIG_FLASH_OR_PRELIM  0xff806e65

#define CONFIG_EPLD_BR_PRELIM	(BR_PHYS_ADDR(CONFIG_SYS_EPLD_BASE_PHYS) \
				 | BR_PS_16 | BR_V)
#define CONFIG_EPLD_OR_PRELIM	0xffff0e45	

#define CONFIG_SYS_BR0_PRELIM	CONFIG_FLASH_BR_PRELIM	
#define CONFIG_SYS_OR0_PRELIM	CONFIG_FLASH_OR_PRELIM	

#define CONFIG_SYS_BR1_PRELIM	CONFIG_EPLD_BR_PRELIM 
#define CONFIG_SYS_OR1_PRELIM	CONFIG_EPLD_OR_PRELIM

#define CFG_FLASH_BANKS_LIST	{CONFIG_SYS_FLASH_BASE}
#undef  CFG_FLASH_CHECKSUM

#define CFG_MONITOR_BASE        CONFIG_SYS_TEXT_BASE       /* start of monitor */

#undef	CFG_FLASH_PROTECTION

#define	CONFIG_FLASH_CFI_DRIVER
#define	CONFIG_SYS_FLASH_CFI
#define	CONFIG_SYS_FLASH_EMPTY_INFO

#define CONFIG_SYS_FLASH_QUIET_TEST
#define CONFIG_FLASH_SHOW_PROGRESS  45	/* count down from 45/5: 9..1 */

#define CONFIG_SYS_INIT_RAM_LOCK    1
#define CONFIG_SYS_INIT_RAM_ADDR    0xfee00000	/* Initial RAM address */
#define CFG_INIT_RAM_ADDR	    0xfee00000	/* Initial RAM address */
#define CONFIG_SYS_INIT_RAM_END	    0x4000	/* End of used area in RAM */
#define CFG_INIT_RAM_END	    0x4000

#define POST_STOR_WORD		    CFG_INIT_RAM_ADDR

#define CONFIG_SYS_GBL_DATA_OFFSET      (CONFIG_SYS_INIT_RAM_END \
				    - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET   CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_LEN	    (512 * 1024)    /* Reserve 512kB for Mon */
#define CFG_MONITOR_LEN		    (512 * 1024)    /* Reserve 512kB for Mon */
#define	CONFIG_SYS_MALLOC_LEN	    (256 * 1024)    /* Reserved for malloc */
#define	CONFIG_SYS_MONITOR_BASE	    CONFIG_SYS_TEXT_BASE	    /* start of monitor */

/* Serial Port */
#define CONFIG_CONS_INDEX	    1
#undef	CONFIG_SERIAL_SOFTWARE_FIFO
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE 1
#define CONFIG_SYS_NS16550_CLK	    get_bus_freq(0)
#define CONFIG_SERIAL_MULTI	    1 /* Enable both serial ports */

#define CONFIG_SYS_NS16550_COM1	    (CONFIG_SYS_CCSRBAR + 0x4500)
#define CONFIG_SYS_NS16550_COM2	    (CONFIG_SYS_CCSRBAR + 0x4600)
#define CONFIG_SYS_CONSOLE_INFO_QUIET

#define CONFIG_SYS_BAUDRATE_TABLE   \
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400,115200}

/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER
#ifdef	CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2  "> "
#endif

/*
 * main board I2C
 */
#define CFG_I2C_CTRL_1	    0
#define CFG_I2C_CTRL_2	    1

/* I2C controller 1 */
#define CFG_I2C_C1_9548SW1		0x70	/* Mux */ /* U17 */
#define CFG_I2C_C1_9548SW1_ENV_0	0x29	/* W83782D_0 */
#define CFG_I2C_C1_9548SW1_ENV_1	0x2A	/* W83782D_1 */
#define CFG_I2C_C1_9548SW1_P1_PS	0x54	/* power supply */
#define CFG_I2C_C1_9548SW1_P2_PS	0x54	/* power supply */
#define CFG_I2C_C1_9548SW1_P3_AD5245_1	0x2C	/* front LCD */
#define CFG_I2C_C1_9548SW1_P3_AD5245_2	0x2D	/* back LCD */
#define CFG_I2C_C1_9548SW1_P4_SEEPROM	0x72	/* Renesas R5H30211 */
#define CFG_I2C_C1_9548SW1_P5_RTC	0x51	/* RTC M41T11M6E */
#define CFG_I2C_C1_9548SW1_P5_EEPROM	0x53	/* EEPROM AT24C02 */
#define CFG_I2C_C1_9548SW1_P5_FAN	0x54	/* FAN */
#define CFG_I2C_C1_9548SW1_P6_PWR1220	0x40	/* PWR1220 */
#define CFG_I2C_C1_9548SW1_P6_TEMP_0	0x19	/* NE1617A_0*/
#define CFG_I2C_C1_9548SW1_P6_TEMP_1	0x1A	/* NE1617A_1*/
#define CFG_I2C_C1_9548SW1_P6_ICS_0	0x6E	/* ICS9DB803 */
#define CFG_I2C_C1_9548SW1_P7_MUX_0	0x51	/* ADI MUX 0 */
#define CFG_I2C_C1_9548SW1_P7_MUX_1	0x52	/* ADI MUX 1 */
#define CFG_I2C_C1_9548SW1_P7_MUX_2	0x53	/* ADI MUX 2 */
#define CFG_I2C_C1_9548SW1_P7_MUX_3	0x54	/* ADI MUX 3 */
#define CFG_I2C_C1_9548SW1_P7_ICS_1	0x6E	/* ICS9DB803 */
#define CFG_I2C_C1_9548SW1_P7_PEX8618	0x38	/* PEX8618 */
#define CFG_I2C_C1_9548SW1_P7_MK1493	0x69	/* MK1493-03B */

/* I2C controller 2 */
#define CFG_I2C_C2_9546SW1			    0x71    /* U19 */
#define CFG_I2C_C2_9546SW1_P0_9548SW2		    0x74    /* U51 */
#define CFG_I2C_C2_9546SW1_P0_9548SW2_P0_SFPP_0	    0x50
#define CFG_I2C_C2_9546SW1_P0_9548SW2_P1_SFPP_1	    0x50
#define CFG_I2C_C2_9546SW1_P0_9548SW2_P2_SFPP_2	    0x50
#define CFG_I2C_C2_9546SW1_P0_9548SW2_P3_SFPP_3	    0x50
#define CFG_I2C_C2_9546SW1_P0_9548SW2_P4_SFPP_4	    0x50
#define CFG_I2C_C2_9546SW1_P0_9548SW2_P5_SFPP_5	    0x50
#define CFG_I2C_C2_9546SW1_P0_9548SW2_P6_SFPP_6	    0x50
#define CFG_I2C_C2_9546SW1_P0_9548SW2_P7_SFPP_7	    0x50
#define CFG_I2C_C2_9546SW1_P1_9548SW3		    0x74    /* U53 */
#define CFG_I2C_C2_9546SW1_P1_9548SW3_P0_SFPP_8	    0x50
#define CFG_I2C_C2_9546SW1_P1_9548SW3_P1_SFPP_9	    0x50
#define CFG_I2C_C2_9546SW1_P1_9548SW3_P2_SFPP_10    0x50
#define CFG_I2C_C2_9546SW1_P1_9548SW3_P3_SFPP_11    0x50
#define CFG_I2C_C2_9546SW1_P1_9548SW3_P4_SFPP_12    0x50
#define CFG_I2C_C2_9546SW1_P1_9548SW3_P5_SFPP_13    0x50
#define CFG_I2C_C2_9546SW1_P1_9548SW3_P6_SFPP_14    0x50
#define CFG_I2C_C2_9546SW1_P1_9548SW3_P7_SFPP_15    0x50
#define CFG_I2C_C2_9546SW1_P2_9548SW4		    0x74    /* U52 */
#define CFG_I2C_C2_9546SW1_P2_9548SW4_P0_SFPP_16    0x50
#define CFG_I2C_C2_9546SW1_P2_9548SW4_P1_SFPP_17    0x50
#define CFG_I2C_C2_9546SW1_P2_9548SW4_P2_SFPP_18    0x50
#define CFG_I2C_C2_9546SW1_P2_9548SW4_P3_SFPP_19    0x50
#define CFG_I2C_C2_9546SW1_P2_9548SW4_P4_SFPP_20    0x50
#define CFG_I2C_C2_9546SW1_P2_9548SW4_P5_SFPP_21    0x50
#define CFG_I2C_C2_9546SW1_P2_9548SW4_P6_SFPP_22    0x50
#define CFG_I2C_C2_9546SW1_P2_9548SW4_P7_SFPP_23    0x50
#define CFG_I2C_C2_9546SW1_P3_9548SW5		    0x74    /* U67 */
#define CFG_I2C_C2_9546SW1_P3_9548SW5_P0_SFPP_24    0x50
#define CFG_I2C_C2_9546SW1_P3_9548SW5_P1_SFPP_25    0x50
#define CFG_I2C_C2_9546SW1_P3_9548SW5_P2_SFPP_26    0x50
#define CFG_I2C_C2_9546SW1_P3_9548SW5_P3_SFPP_27    0x50
#define CFG_I2C_C2_9546SW1_P3_9548SW5_P4_SFPP_28    0x50
#define CFG_I2C_C2_9546SW1_P3_9548SW5_P5_SFPP_29    0x50
#define CFG_I2C_C2_9546SW1_P3_9548SW5_P6_SFPP_30    0x50
#define CFG_I2C_C2_9546SW1_P3_9548SW5_P7_SFPP_31    0x50

#define CFG_I2C_C2_9546SW2			    0x77    /* U20 */
#define CFG_I2C_C2_9546SW2_P0_9548SW6		    0x74    /* U68 */
#define CFG_I2C_C2_9546SW2_P0_9548SW6_P0_SFPP_32    0x50
#define CFG_I2C_C2_9546SW2_P0_9548SW6_P1_SFPP_33    0x50
#define CFG_I2C_C2_9546SW2_P0_9548SW6_P2_SFPP_34    0x50
#define CFG_I2C_C2_9546SW2_P0_9548SW6_P3_SFPP_35    0x50
#define CFG_I2C_C2_9546SW2_P0_9548SW6_P4_SFPP_36    0x50
#define CFG_I2C_C2_9546SW2_P0_9548SW6_P5_SFPP_37    0x50
#define CFG_I2C_C2_9546SW2_P0_9548SW6_P6_SFPP_38    0x50
#define CFG_I2C_C2_9546SW2_P0_9548SW6_P7_SFPP_39    0x50
#define CFG_I2C_C2_9546SW2_P1_M2_I2C		    0x53
#define CFG_I2C_C2_9546SW2_P2_M1_80_I2C		    0x53
#define CFG_I2C_C2_9546SW2_P3_M1_40_I2C		    0x53

#define CFG_I2C_C2_9506EXP1			    0x21    /* U16 */
#define CFG_I2C_C2_9546SW1_P0_9506EXP2		    0x24    /* U55 */
#define CFG_I2C_C2_9546SW1_P1_9506EXP3		    0x20    /* U65 */
#define CFG_I2C_C2_9546SW1_P1_9506EXP4		    0x22    /* U66 */
#define CFG_I2C_C2_9546SW1_P1_9506EXP5		    0x24    /* U64 */
#define CFG_I2C_C2_9546SW1_P2_9506EXP6		    0x20    /* U207 */
#define CFG_I2C_C2_9546SW1_P2_9506EXP7		    0x22    /* U210 */
#define CFG_I2C_C2_9546SW1_P2_9506EXP8		    0x24    /* U208 */

/* I2C */
#define CONFIG_FSL_I2C		/* Use FSL common I2C driver */
#define CONFIG_HARD_I2C		/* I2C with hardware support */
#undef	CONFIG_SOFT_I2C		/* I2C bit-banged */
#define CONFIG_LAST_STAGE_INIT  /* init 2nd I2C controller */
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_I2C_SPEED		100000	/* I2C speed and slave address */
#define CONFIG_SYS_I2C_EEPROM_ADDR 	CFG_I2C_C1_9548SW1_P4_SEEPROM	
#define CONFIG_SYS_I2C_SLAVE		0x7F
#define CONFIG_SYS_I2C_OFFSET		0x3000
#define CONFIG_SYS_I2C2_OFFSET		0x3100
#define CFG_I2C_NOPROBES		{0x69}  /* Don't probe these addrs */
#define CONFIG_I2C_MUX

#define CONFIG_FSL_I2C_CUSTOM_DFSR	0x10
#define CONFIG_FSL_I2C_CUSTOM_FDR	0x0E
/*
 * M1-SFP+ I2C
 */
/* rev 2 */
#define CFG_I2C_M1_SFPP_9548SW1			0x70	/* U8 */
#define CFG_I2C_M1_SFPP_9548SW1_P0_SFPP_1	0x50
#define CFG_I2C_M1_SFPP_9548SW1_P1_SFPP_0	0x50
#define CFG_I2C_M1_SFPP_9548SW1_P2_SFPP_3	0x50
#define CFG_I2C_M1_SFPP_9548SW1_P3_SFPP_2	0x50
#define CFG_I2C_M1_SFPP_9548SW1_P4_9506EXP1	0x20	/* U7 */
#define CFG_I2C_M1_SFPP_9548SW1_P5_TCPU		0x60
#define CFG_I2C_M1_SFPP_9548SW1_P5_LM75		0x48
#define CFG_I2C_M1_SFPP_9548SW1_P6_SEEPROM	0x72

/* rev 1 */
#define CFG_I2C_M1_SFPP_SEEPROM_REV1		0x52
#define CFG_I2C_M1_SFPP_9548SW1_P5_ADT7410_REV1	0x48

/*
 * M2-LBK I2C
 */
/* rev 2 */
#define CFG_I2C_M2_LBK_9548SW1			0x70	/* U3 */
#define CFG_I2C_M2_LBK_9548SW1_P0_9506EXP1	0x20	/* U5 */
#define CFG_I2C_M2_LBK_9548SW1_P1_SEEPROM	0x72
#define CFG_I2C_M2_LBK_9548SW1_P2_TCPU		0x60
#define CFG_I2C_M2_LBK_9548SW1_P2_LM75		0x48

/* rev 1 */
#define CFG_I2C_M2_LBK_9543SW1_REV1		0x70	/* U3 */
#define CFG_I2C_M2_LBK_9543SW1_P0_9506EXP1_REV1	0x20	/* U5 */
#define CFG_I2C_M2_LBK_9543SW1_P1_TCPU_REV1	0x60
#define CFG_I2C_M2_LBK_9548SW1_P0_ADT7410_REV1	0x48
#define CFG_I2C_M2_LBK_SEEPROM_REV1		0x52

/*
 * M2-LBK I2C
 */
#define CFG_I2C_M2_OPTIC_9543SW1_REV1		0x70
#define CFG_I2C_M2_OPTIC_SEEPROM_REV1		0x62

/*
 * M1/M2 misc
 */
#define CFG_I2C_C2_9546SW2_M1_40_CHAN		3
#define CFG_I2C_C2_9546SW2_M1_80_CHAN		2
#define CFG_I2C_C2_9546SW2_M2_CHAN		1

/* M1/M2 test circuit CPU channel */
#define CFG_I2C_C2_9546SW2_M1_CPU_CHAN		5
#define CFG_I2C_C2_9546SW2_M2_CPU_CHAN		2

#define CFG_EEPROM_PAGE_WRITE_BITS		3
#define CFG_EEPROM_PAGE_WRITE_DELAY_MS		10
#define CFG_EEPROM_MAC_OFFSET			56

/*
 * main board EEPROM platform ID
 */
#define	I2C_ID_JUNIPER_EX4510_32_F	0x0B39	/* KONA 32F */
#define	I2C_ID_JUNIPER_EX4510_32_T	0x0B3A	/* KONA 32T */
/*
 * These defines are here just from a code compilation 
 * prespective, once the code has been properly seperated out,
 * we will remove thee defines
 */
#define I2C_ID_JUNIPER_EX4500_40F	0x095a  /* Java Tsunami T40 */
#define I2C_ID_JUNIPER_EX4500_20F	0x095b  /* Java Tsunami T20 */

/*
 * M1/M2 card EEPROM platform ID
 */
#define I2C_ID_TSUNAMI_UPLINK_SFPPLUS_4PORT_PIC	    0x02FF
#define I2C_ID_TSUNAMI_M2_LOOPBACK_PIC		    0x02D2
#define I2C_ID_EX4500_UPLINK_XFP_4PORT_PIC	    0x0A1D
#define I2C_ID_EX4500_UPLINK_M2_OPTICAL_PIC	    0x0A1E
#define I2C_ID_EX4500_UPLINK_M2_LEGACY_2PORT_PIC    0x0A1F

/*
 * Power supply EEPROM platform ID
 */
#define I2C_ID_EX4500F_PWR_1050		0x043C  /* 1050W */

/* i2c eeprom/board types */
#define CONFIG_BOARD_TYPES	1

#define CFG_EX4500_T40_INDEX	0
#define CFG_EX4500_T20_INDEX	0
#define CFG_EX4500_M1_40_INDEX	1
#define CFG_EX4500_M1_80_INDEX	2
#define CFG_EX4500_M2_INDEX	3
#define CFG_EX4500_LAST_CARD	4

/*
 * As of now, comment this code out.
 * This may not be required on EX4510, but
 * put it in place till the final testing is
 * done and later remove it completely if
 * not required
 */
#if 0
/*
 * General PCI
 * Addresses are mapped 1-1.
 */
#define CFG_PCI1_MEM_BASE       0x80000000
#define CFG_PCI1_MEM_PHYS       CFG_PCI1_MEM_BASE
#define CFG_PCI1_MEM_SIZE       0x10000000      /* 256M */
#define CFG_PCI1_IO_BASE        0x00000000
#define CFG_PCI1_IO_PHYS        0xe2000000
#define CFG_PCI1_IO_SIZE        0x00100000      /* 1M */

/* General PCI new defines */
#define CONFIG_SYS_PCI_VIRT	0x80000000
#define CONFIG_SYS_PCI_PHYS	0x80000000

#define CONFIG_SYS_PCI1_MEM_VIRT    0x80000000
#define CONFIG_SYS_PCI1_MEM_BUS	    0x80000000
#define CONFIG_SYS_PCI1_MEM_PHYS    0x80000000
#define CONFIG_SYS_PCI1_MEM_SIZE    0x10000000	/* 256 */
#define CONFIG_SYS_PCI1_IO_VIRT	    0xe2000000
#define CONFIG_SYS_PCI1_IO_BUS	    0x00000000
#define CONFIG_SYS_PCI1_IO_PHYS	    0xe2000000
#define CONFIG_SYS_PCI1_IO_SIZE	    0x00100000  /* 1M */
#endif

/*
 * PCI Express
 */
#define CONFIG_PCIE		    1
#define CFG_PCIE_MEM_BASE	    0x90000000
#define CFG_PCIE_MEM_PHYS	    CFG_PCIE_MEM_BASE
#define CFG_PCIE_MEM_SIZE	    0x40000000	/* 1G */

/* New defines for PCIE */
#define CONFIG_SYS_PCIE_VIRT	    0x80000000
#define CONFIG_SYS_PCIE_PHYS	    0x80000000
#define CONFIG_SYS_PCIE1_MEM_VIRT   0x80000000
#define CONFIG_SYS_PCIE1_MEM_BUS    0x80000000
#define CONFIG_SYS_PCIE1_MEM_PHYS   0x80000000
#define CONFIG_SYS_PCIE1_MEM_SIZE   0x40000000	/* 1G */
#define CONFIG_SYS_PCIE1_IO_VIRT    0xe2000000
#define CONFIG_SYS_PCIE1_IO_BUS	    0x00000000
#define CONFIG_SYS_PCIE1_IO_PHYS    0xe2000000
#define CONFIG_SYS_PCIE1_IO_SIZE    0x00100000	/*   1M */

#if defined(CONFIG_PCI)

#define CONFIG_PCI_PNP                  /* do pci plug-and-play */
#undef CONFIG_85XX_PCI2

#undef CONFIG_PCI_SCAN_SHOW             /* show pci devices on startup */
#define CFG_PCI_SUBSYS_VENDORID 0x1957  /* Freescale */

#endif  /* CONFIG_PCI */


#if defined(CONFIG_TSEC_ENET)

#define CONFIG_NET_MULTI    1

#define CONFIG_MII		1	/* MII PHY management */
#define CONFIG_MII_DEFAULT_TSEC	1	/* Allow unregistered phys */
#define CONFIG_TSEC1	1
#define CONFIG_TSEC1_NAME	"eTSEC1"
#define CONFIG_TSEC2	1
#define CONFIG_TSEC2_NAME	"eTSEC2"
#define CONFIG_TSEC3	1
#define CONFIG_TSEC3_NAME	"eTSEC3"

#define CONFIG_SYS_TBIPA_VALUE		31 /* avoid conflict with eTSEC4 paddr */

#define TSEC1_PHY_ADDR          1
#define TSEC2_PHY_ADDR          31
#define TSEC3_PHY_ADDR          31

#define TSEC1_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC2_FLAGS		(TSEC_GIGABIT | TSEC_SGMII)
#define TSEC3_FLAGS		(TSEC_GIGABIT | TSEC_SGMII)

/* all three TSECs do not share the same smi interface */
#define TSEC1_PHYIDX            0
#define TSEC2_PHYIDX            1
#define TSEC3_PHYIDX            2

#define CONFIG_ETHPRIME		"eTSEC1"

#define CONFIG_PHY_GIGE		1	/* Include GbE speed/duplex detection */
#endif	/* CONFIG_TSEC_ENET */

/*
 * Environment
 */
#define CONFIG_FLASH_SIZE		0x800000 
#define CFG_FLASH_SIZE		0x800000 
#define CONFIG_ENV_IS_IN_FLASH		1
#define CFG_ENV_IS_IN_FLASH		1
#define CONFIG_ENV_ADDR			CONFIG_SYS_FLASH_BASE
#define CFG_ENV_OFFSET			0
#define CFG_ENV_SECTOR			0
#define CONFIG_ENV_SIZE			0x10000
#define CFG_ENV_SIZE			0x10000
#define CONFIG_ENV_SECT_SIZE		0x10000 /* 64K (one sector) */
#define CFG_OPQ_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + 0x10000) /* for RE nv env */
#define CFG_OPQ_ENV_OFFSET		0x10000
#define CFG_OPQ_ENV_SECT_SIZE		0x10000  /* 64K(1x64K sector) for RE nv env */
#define CFG_UPGRADE_ADDR		(CONFIG_SYS_FLASH_BASE + 0XA0000)
#define CFG_UPGRADE_SECT_SIZE		0x10000  /* 64K(1x64K sector) for upgrade */

#define CONFIG_LOADS_ECHO		1	/* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1	/* allow baudrate change */

#define CONFIG_API
#define CONFIG_DOS_PARTITION

#include <config_cmd_default.h>

#define CONFIG_CMD_IRQ
#define CONFIG_CMD_PING
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MII
#define CONFIG_CMD_ELF
#define CONFIG_CMD_SETEXPR
#define CONFIG_CMD_NET
#define CONFIG_CMD_EEPROM
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1
#define CMD_SAVEENV	
#define CONFIG_CMD_FLASH

/*
 * USB
 */
#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_FSL
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_USB_SHOW_NO_PROGRESS
#define USB_WRITE_READ

#define USB_OHCI_VEND_ID                0x1131
#define USB_EHCI_VEND_ID                0x1131
#define USB_OHCI_DEV_ID			0x1561
#define USB_EHCI_DEV_ID			0x1562

#define CONFIG_START_USB

#undef CONFIG_WATCHDOG			/* watchdog disabled */
#define CONFIG_EPLD_WATCHDOG            /* EPLD watchdog */
#define CONFIG_WATCHDOG_EXCEPTION

/*
 * PIC
 */
/* MPC8548 12 External Interrupts 0x1200 - 0x120B */
#define IRQ0_INT_VEC            0x1200
#define IRQ1_INT_VEC            0x1201
#define IRQ2_INT_VEC            0x1202
#define IRQ3_INT_VEC            0x1203
#define IRQ4_INT_VEC            0x1204
#define IRQ5_INT_VEC            0x1205
#define IRQ6_INT_VEC            0x1206
#define IRQ7_INT_VEC            0x1207
#define IRQ8_INT_VEC            0x1208
#define IRQ9_INT_VEC            0x1209
#define IRQ10_INT_VEC            0x120A
#define IRQ11_INT_VEC            0x120B

/*
 * Ports
 */
#define NUM_EXT_PORTS        52

/*
 * LCD + LED
 */
#define CONFIG_SHOW_ACTIVITY
#define CFG_LCD_NUM 1

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory	*/
#define CONFIG_CMDLINE_EDITING		/* Command-line editing */
#define CONFIG_SYS_LOAD_ADDR	0x2000000	/* default load address */
#define CONFIG_SYS_PROMPT		"=> "	/* Monitor Command Prompt */
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE	1024	/* Console I/O Buffer Size */
#else
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size */
#endif
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE \
		+ sizeof(CONFIG_SYS_PROMPT) + 16)	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16		/* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size */
#define CONFIG_SYS_HZ		1000	/* decrementer freq: 1ms ticks */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 16 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(16 << 20)	/* Initial Memory map for Linux*/

/* Cache Configuration */
#define CFG_DCACHE_SIZE 32768
#define CFG_CACHELINE_SIZE      32
#define CONFIG_SYS_CACHELINE_SIZE      32
#if defined(CONFIG_CMD_KGDB)
#define CFG_CACHELINE_SHIFT     5       /*log base 2 of the above value*/
#endif

#define CONFIG_BOARD_EARLY_INIT_F	1	/* call board_pre_init */
#define CONFIG_BOARD_EARLY_INIT_R	1	/* call board_early_init_r function */
#define CONFIG_MISC_INIT_R			1	/* call misc_init_r() */

/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define BOOTFLAG_COLD	0x01		/* Normal Power-On: Boot from FLASH */
#define BOOTFLAG_WARM	0x02		/* Software reboot */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

/*
 * Environment Configuration
 */

#define CONFIG_ETHADDR   00:E0:0C:00:00:3C
#define CONFIG_HAS_ETH1
#define CONFIG_ETH1ADDR  00:E0:0C:00:00:3D
#define CONFIG_HAS_ETH2
#define CONFIG_ETH2ADDR  00:E0:0C:00:00:3E
#define CONFIG_HAS_ETH3
#define CONFIG_ETH3ADDR  00:E0:0C:00:00:3F


/* default location for tftp and bootm */
#define CONFIG_LOADADDR		0x100000
#define CONFIG_BOOTDELAY	1	/* -1 disables auto-boot */
#undef  CONFIG_BOOTARGS		/* the boot command will set bootargs */
#define CONFIG_BAUDRATE		9600

#define CONFIG_AUTOBOOT_KEYED	/* use key strings to stop autoboot */

/*
 * Use ctrl-c to stop booting and enter u-boot command mode.
 * ctrl-c is common but not too easy to interrupt the booting by accident.
 */
#define CONFIG_AUTOBOOT_STOP_STR	"\x03"
#define CONFIG_SILENT_CONSOLE		1

#define CONFIG_EXTRA_ENV_SETTINGS                                    \
   "hw.uart.console=mm:0xfef04500\0"

#if defined(CONFIG_UPGRADE)
#define CONFIG_BOOTCOMMAND  \
  "bootelf 0xffe00000"
#else
#define CONFIG_BOOTCOMMAND  \
  "bootelf 0xfff00000"
#endif

#define CFG_UPGRADE_BOOT_STATE_OFFSET	0xA0000 /* from beginning of flash */
#define CFG_UPGRADE_BOOT_SECTOR_SIZE	0x10000  /* 64k */

#define CFG_IMG_HDR_OFFSET	0x30 /* from image address */
#define CFG_CHKSUM_OFFSET	0x100 /* from image address */

#define CONFIG_UBOOT_UPGRADE		1
#define CFG_UPGRADE_LOADER_BANK1	0xFFB00000
#define CFG_UPGRADE_LOADER_BANK0	0xFFF00000
#define CFG_UPGRADE_UBOOT_BANK1		0xFFB80000
#define CFG_UPGRADE_UBOOT_BANK0		0xFFF80000
/* check for ative partition and swizzle flash bank */
#define CONFIG_FLASH_SWIZZLE
#define CONFIG_BOOT_STATE		/* maintain boot/upgrade states */

#define	CONFIG_COMMANDS		(CONFIG_CMD_DFL \
				| CFG_CMD_PCI \
				| CFG_CMD_PING \
				| CFG_CMD_I2C \
				| CFG_CMD_MII \
				| CFG_CMD_USB \
				| CFG_CMD_CACHE \
				| CFG_CMD_ELF \
				| CFG_CMD_EEPROM \
				| CFG_CMD_BSP)

#define CONFIG_BOARD_RESET  1		/* board specific reset function */

#define CFG_VERSION_MAJOR       1
#define CFG_VERSION_MINOR       0
#define CFG_VERSION_PATCH       0

#endif	/* __CONFIG_H */