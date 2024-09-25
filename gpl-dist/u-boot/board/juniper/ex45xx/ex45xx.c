/*
 * Copyright (c) 2011-2013, Juniper Networks, Inc.
 * All rights reserved.
 *
 * Copyright 2004 Freescale Semiconductor.
 *
 * (C) Copyright 2002 Scott McNutt <smcnutt@artesyncp.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <config.h>
#include <asm/types.h>
#include <asm/fsl_i2c.h>
#include <asm/io.h>
#include <i2c.h>
#include "cmd_ex45xx.h"
#include "epld.h"
#include "epld_watchdog.h"
#include "led.h"
#include "lcd.h"
#include <tsec.h>
#include <net.h>
#include <pci.h>
#include <asm/processor.h>
#include <asm/immap_85xx.h>
#include <asm/fsl_ddr_sdram.h>
#include <asm/mmu.h>
#include <asm/fsl_law.h>
#include <spd.h>
#include <spd_sdram.h>
#include <usb.h>
#include "ex45xx_local.h"
#include <asm/fsl_pci.h>
#if defined(CONFIG_POST)
#include <post.h>
#endif

#define	EX45XX_DEBUG
#undef debug
#ifdef	EX45XX_DEBUG
#define debug(fmt,args...)	printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif

DECLARE_GLOBAL_DATA_PTR;

extern phys_size_t fixed_sdram (void);
extern void ddr_enable_ecc(unsigned int);
extern uint read_phy_reg_private(struct tsec_private *priv, uint regnum);
extern void write_phy_reg_private(struct tsec_private *priv, uint regnum, uint value);
#ifdef CONFIG_EX4500
extern int ether_tsec_reinit (struct eth_device *dev);
#endif
int secure_eeprom_read (unsigned dev_addr, unsigned offset, 
    uchar *buffer, unsigned cnt);

struct pci_info {
    u16     cfg;
};

/* The cfg field is a bit mask in which each bit represents the value of
 * cfg_IO_ports[] signal and the bit is set if the interface would be
 * enabled based on the value of cfg_IO_ports[] signal
 *
 * On MPC86xx/PQ3 based systems:
 *   we extract cfg_IO_ports from GUTS register PORDEVSR
 *
 * cfg_IO_ports only exist on systems w/PCIe (we set cfg 0 for systems
 * without PCIe)
 */

#if defined(CONFIG_MPC8540) || defined(CONFIG_MPC8560)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCI] = {
		.cfg =   0,
	},
};
#elif defined(CONFIG_MPC8541) || defined(CONFIG_MPC8555)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCI] = {
		.cfg =   0,
	},
};
#elif defined(CONFIG_MPC8536)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCI] = {
		.cfg =   0,
	},
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 2) | (1 << 3) | (1 << 5) | (1 << 7),
	},
	[LAW_TRGT_IF_PCIE_2] = {
		.cfg =   (1 << 5) | (1 << 7),
	},
	[LAW_TRGT_IF_PCIE_3] = {
		.cfg =   (1 << 7),
	},
};
#elif defined(CONFIG_MPC8544)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCI] = {
		.cfg =   0,
	},
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) |
			 (1 << 6) | (1 << 7),
	},
	[LAW_TRGT_IF_PCIE_2] = {
		.cfg =   (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7),
	},
	[LAW_TRGT_IF_PCIE_3] = {
		.cfg =   (1 << 6) | (1 << 7),
	},
};
#elif defined(CONFIG_MPC8548)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCI_1] = {
		.cfg =   0,
	},
	[LAW_TRGT_IF_PCI_2] = {
		.cfg =   0,
	},
	/* PCI_2 is always host and we dont use iosel to determine enable/disable */
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 3) | (1 << 4) | (1 << 7),
	},
};
#elif defined(CONFIG_MPC8568)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCI] = {
		.cfg =   0,
	},
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 3) | (1 << 4) | (1 << 7),
	},
};
#elif defined(CONFIG_MPC8569)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 0) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) |
			 (1 << 8) | (1 << 0xc) | (1 << 0xf),
	},
};
#elif defined(CONFIG_MPC8572)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 2) | (1 << 3) | (1 << 7) |
			 (1 << 0xb) | (1 << 0xc) | (1 << 0xf),
	},
	[LAW_TRGT_IF_PCIE_2] = {
		.cfg =   (1 << 3) | (1 << 7),
	},
	[LAW_TRGT_IF_PCIE_3] = {
		.cfg =   (1 << 7),
	},
};
#elif defined(CONFIG_MPC8610)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCI_1] = {
		.cfg =   0,
	},
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 1) | (1 << 4),
	},
	[LAW_TRGT_IF_PCIE_2] = {
		.cfg =   (1 << 0) | (1 << 4),
	},
};
#elif defined(CONFIG_MPC8641)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6) |
			 (1 << 7) | (1 << 0xe) | (1 << 0xf),
	},
};
#elif defined(CONFIG_P1011) || defined(CONFIG_P1020)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 0) | (1 << 6) | (1 << 0xe) | (1 << 0xf),
	},
	[LAW_TRGT_IF_PCIE_2] = {
		.cfg =   (1 << 0xe),
	},
};
#elif defined(CONFIG_P2010) || defined(CONFIG_P2020)
static struct pci_info pci_config_info[] =
{
	[LAW_TRGT_IF_PCIE_1] = {
		.cfg =   (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) |
			 (1 << 0xd) | (1 << 0xe) | (1 << 0xf),
	},
	[LAW_TRGT_IF_PCIE_2] = {
		.cfg =   (1 << 2) | (1 << 0xe),
	},
	[LAW_TRGT_IF_PCIE_3] = {
		.cfg =   (1 << 2) | (1 << 4),
	},
};
#elif defined(CONFIG_FSL_CORENET)
#else
#error Need to define pci_config_info for processor
#endif

typedef enum upgrade_state 
{
	UPGRADE_READONLY = 0,	/* set by readonly u-boot */
	UPGRADE_START,		/* set by loader/JunOS */
	UPGRADE_CHECK,		/* set by readonly U-boot */
	UPGRADE_TRY,		/* set by readonly U-boot */
	UPGRADE_OK		/* set by upgrade U-boot */
} upgrade_state_t;

extern int usb_stor_curr_dev; /* current device */
extern int i2c_mfgcfg;
extern int i2c_manual;

int post_result_cpu = 0;
int post_result_mem = 0;
int post_result_rom = 0;
int i2c_post = 0;

int usb_scan_dev = 0;
int post_result_ether = 0;
int post_result_pci = 0;
static struct tsec_private *mgmt_priv = NULL;
static int mdio_inuse = 0;
static uint8_t curmode = 0xff;

static uint16_t version_epld(void);

int console_silent_init (void)
{

#ifdef CONFIG_SILENT_CONSOLE
	if (getenv("silent") != NULL)
	    gd->flags |= GD_FLG_SILENT;
#endif

	return 0;
}

int set_upgrade_state (upgrade_state_t state)
{
	ulong upgrade_addr = (CONFIG_SYS_FLASH_BASE +
				CFG_UPGRADE_BOOT_STATE_OFFSET);
	ulong ustate = state;
	int ret;

	ret = flash_write_direct(upgrade_addr, (char *)&ustate, sizeof(ulong));
	return ret;
}

int get_upgrade_state (void)
{
	volatile upgrade_state_t *ustate =
	    (upgrade_state_t *) (CONFIG_SYS_FLASH_BASE +
				    CFG_UPGRADE_BOOT_STATE_OFFSET);

	if ((*ustate < UPGRADE_READONLY) || (*ustate > UPGRADE_OK)) {
	    set_upgrade_state(UPGRADE_READONLY);
	    return (UPGRADE_READONLY);
	}

	return (*ustate);
}

int valid_env (unsigned long addr)
{
	return 1;
}

int valid_uboot_image (unsigned long addr)
{
	uint32_t icrc;
	image_header_t *haddr = (image_header_t *) (addr + CFG_IMG_HDR_OFFSET);

	if ((icrc = crc32(0, (unsigned char *)(addr + CFG_CHKSUM_OFFSET),
		    CFG_MONITOR_LEN - CFG_CHKSUM_OFFSET)) == haddr->ih_dcrc) {
	    return 1;
	}

	printf("crc: 0x%x / 0x%x\n", icrc, haddr->ih_dcrc);
	return 0;
}

#ifdef CONFIG_EX4500
int check_upgrade (void)
{
#if defined(CONFIG_READONLY)
	ulong addr;
	upgrade_state_t state = get_upgrade_state();

	if (state == UPGRADE_START) {
	    /* verify upgrade */
	    if (valid_env(CONFIG_SYS_FLASH_BASE) &&
		valid_elf_image(CFG_UPGRADE_LOADER_ADDR) &&
		valid_uboot_image(CFG_UPGRADE_BOOT_IMG_ADDR)) {
		set_upgrade_state(UPGRADE_CHECK);
	    } else {
		set_upgrade_state(UPGRADE_READONLY);
	    }
	} else if (state == UPGRADE_TRY) {
	    /* Something is wrong. Upgrade did not boot up. Clear the state */
	    set_upgrade_state(UPGRADE_READONLY);
	}

	state = get_upgrade_state();
	/* boot from upgrade check */
	if ((state == UPGRADE_OK) || (state == UPGRADE_CHECK)) {
	    /* First time to boot Upgrade */
	    set_upgrade_state(UPGRADE_TRY);

	    /* select upgrade image to be boot from */
	    addr = CFG_UPGRADE_BOOT_ADDR;
	    ((ulong (*)())addr) ();
	}
#endif
	return 0;
}
#endif

/*
 * check upgrade mechanism on EX4510 is differnt from that of EX4500
 * The flash present on EX4510 deos not support parallel read/writes,
 * hence we need to do flash writes only after relocating to DDR and
 * uboot is executing from flash.
 * Also, EX4510 will support flash swizzle option similar to modular
 * platforms. Hence we need to define the check_upgarde differently for
 * EX4510.
 *
 */
#ifdef CONFIG_EX4510
int check_upgrade (void)
{
#ifdef CONFIG_READONLY
        ulong addr;
        upgrade_state_t state = get_upgrade_state();

        if ((gd->flags & GD_FLG_RELOC) && (state == UPGRADE_START)) {
            /* verify upgrade */
            if (valid_env(CONFIG_SYS_FLASH_BASE) &&
                valid_elf_image(CFG_UPGRADE_LOADER_ADDR) &&
                valid_uboot_image(CFG_UPGRADE_BOOT_IMG_ADDR)) {
                set_upgrade_state(UPGRADE_CHECK);
            } else {
                set_upgrade_state(UPGRADE_READONLY);
            }
        } else if ((gd->flags & GD_FLG_RELOC) && (state == UPGRADE_TRY)) {
            /* Something is wrong. Upgrade did not boot up. Clear the state */
            set_upgrade_state(UPGRADE_READONLY);
        }

        state = get_upgrade_state();
        /* boot from upgrade check */
        if ((gd->flags & GD_FLG_RELOC) && ((state == UPGRADE_OK) ||
                                           (state == UPGRADE_CHECK))) {
            /* First time to boot Upgrade */
            set_upgrade_state(UPGRADE_TRY);

            /* select upgrade image to be boot from */
            addr = CFG_UPGRADE_BOOT_ADDR;
            ((ulong (*)())addr) ();
        }
#endif
        return 0;
}
#endif

int set_upgrade_ok (void)
{
#if defined(CONFIG_UPGRADE)
	char temp[5];
	int state = get_upgrade_state();

	/* Upgrade U-boot is running from RAM. */
	if (state == UPGRADE_TRY) {
	    set_upgrade_state(UPGRADE_OK);
	    state = get_upgrade_state();
	}

	/* environment variable current U-boot image */
	setenv("boot.current", "upgrade");

	sprintf(temp,"%d ", state);
	setenv("boot.state", temp);
#endif

	return 0;
}

int board_id (void)
{
	return ((gd->board_type & 0xffff0000) >> 16);
}

int version_major (void)
{
	return ((gd->board_type & 0xff00) >> 8);
}

int version_minor (void)
{
	return (gd->board_type & 0xff);
}

static uint16_t version_epld (void)
{
	volatile uint16_t ver;

	epld_reg_read(EPLD_WATCHDOG_SW_UPDATE, (uint16_t *)&ver);
	return ver;
}

static int mem_ecc (void)
{
	return 1;
}

int mem_size (void)
{	
	return (CONFIG_SYS_SDRAM_SIZE);
}

int board_early_init_f (void)
{
#ifdef CONFIG_EX4500
	volatile ccsr_gur_t *gur = (void *)CONFIG_SYS_MPC85xx_GUTS_ADDR;
	volatile uint16_t epldMiscCtrl;

	gur->clkocr = 0x80000000;	/* enable CLK_OUT */

	gur->gpiocr = 0x00010200; /* enable GPIN[0-7] and GPOUT[0-7] */
	/* toggle all devices, all in reset (active low) */
	gur->gpoutdr = 0x00000004; /* all in reset */        
	udelay(10000);
	/* ISP1564, ST72681, W83782G, USB hub, I2C out of reset */
	gur->gpoutdr = 0x0000001B;         

	/* enable secure EEPROM + USB hub */
	epld_reg_read(EPLD_MISC_CONTROL_0, (uint16_t *)&epldMiscCtrl);
	epldMiscCtrl |= EPLD_RRSH_RESET | EPLD_USB_POWER_EN;
	epld_reg_write(EPLD_MISC_CONTROL_0, epldMiscCtrl);
#endif

#ifdef CONFIG_EX4510
	volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);
	volatile uint16_t epldMiscCtrl;
	volatile uint16_t epldSysReset;

	/* 
	 * all 16 gpio pins are used as resets
	 * all 16 signals should be configured
	 * as output
	 */
	setbits_be32(&pgpio->gpdir, 0xffff0000);
	/*
	 * GPIO 0 -- Front module reset
	 * GPIO 1 -- Rear module reset
	 * GPIO 2 -- PFE reset
	 * GPIO 3 -- MGMT PHY reset
	 * GPIO 4 -- ST72681 reset
	 * GPIO 5 -- PCI SW reset
	 * GPIO 6 -- I2C1 reset
	 * GPIO 7 -- I2C2 reset
	 * GPIO 8 -- PHY1 reset
	 * GPIO 9 -- PHY2 reset
	 * GPIO 10 -- PHY3 reset
	 * GPIO 11 -- PHY4 reset
	 * GPIO 12 -- PHY5 reset
	 * GPIO 13 -- PHY6 reset
	 * GPIO 14 -- PHY7 reset
	 * GPIO 15 -- PHY8 reset
	 */
	/* NOTE: need to modify this based on final testing */
	clrsetbits_be32(&pgpio->gpdat, 0x00ff0000, 0x3f000000);

	/* NOTE: syspld resets */
	epld_reg_read(EPLD_MISC_CONTROL_0, (uint16_t *)&epldMiscCtrl);
	epldMiscCtrl |= EPLD_RRSH_RESET;
	epld_reg_write(EPLD_MISC_CONTROL_0, epldMiscCtrl);

	epld_reg_read(EPLD_SYSTEM_RESET, (uint16_t *)&epldSysReset);
	epldSysReset &= ~(EPLD_USB_PHY_RST | EPLD_USB_HUB_RST
			  | EPLD_USB_CONS_RST | EPLD_RTC_RST);
	epld_reg_write(EPLD_SYSTEM_RESET, epldSysReset);

#endif

	return 0;
}

#if defined(CONFIG_EX4510) && defined(CONFIG_TSEC_ENET)
int board_eth_init(bd_t *bis)
{
	struct tsec_info_struct tsec_info[4];
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	int num = 0;

#ifdef CONFIG_TSEC1
	SET_STD_TSEC_INFO(tsec_info[num], 1);
	num++;
#endif
#ifdef CONFIG_TSEC2
	SET_STD_TSEC_INFO(tsec_info[num], 2);
	if (!(gur->pordevsr & MPC85xx_PORDEVSR_SGMII2_DIS))
		tsec_info[num].flags |= TSEC_SGMII;
	num++;
#endif
#ifdef CONFIG_TSEC3
	SET_STD_TSEC_INFO(tsec_info[num], 3);
	if (!(gur->pordevsr & MPC85xx_PORDEVSR_SGMII3_DIS))
		tsec_info[num].flags |= TSEC_SGMII;
	num++;
#endif

	if (!num) {
		printf("No TSECs initialized\n");

		return 0;
	}

	if ((board_id() == I2C_ID_JUNIPER_EX4510_32_F) &&
	    (version_major() == 1)) {
	    setenv("ethprime", "eTSEC1");
	} else {
	    setenv("ethprime", "eTSEC3");
	}

	tsec_eth_init(bis, tsec_info, num);

	return (num);
}
#endif

int board_early_init_r (void)
{
	usb_scan_dev = 0;

	return 0;
}

#ifdef CONFIG_EX4500
/*
 * Initialize Local Bus
 */
static void local_bus_init (void)
{
	volatile fsl_lbc_t *lbc = (void *)CONFIG_SYS_LBC_ADDR;
	uint clkdiv;
	uint lbc_hz;
	sys_info_t sysinfo;

	/*
	 * Errata LBC11.
	 * Fix Local Bus clock glitch when DLL is enabled.
	 * If localbus freq is < 66Mhz, DLL bypass mode must be used.
	 * If localbus freq is > 133Mhz, DLL can be safely enabled.
	 * Between 66 and 133, the DLL is enabled with an override workaround
	 */

	get_sys_info(&sysinfo);
	clkdiv = lbc->lcrr & 0x0f;
	lbc_hz = sysinfo.freqSystemBus / 1000000 / clkdiv;

	if (lbc_hz < 66) {
	    lbc->lcrr |= 0x80000000;	/* DLL Bypass */
	} else if (lbc_hz >= 133) {
	    lbc->lcrr &= (~0x80000000);	/* DLL Enabled */
	} else {
	    lbc->lcrr &= (~0x8000000);	/* DLL Enabled */
	    udelay(200);
	}
}
#endif

int checkboard (void)
{
	volatile uint16_t epld_ver, epldLastRst;
	unsigned char ch = 0x10, mcfg;
	uint32_t id, bid = 0, hwreworkrev;
	int ret_i2c = 0;
   
	i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	i2c_set_bus_num(CFG_I2C_CTRL_1);

	/* read assembly ID from main board EEPROM offset 0x4 */
	/* select i2c controller 1 */
	ch = 0x10;
	ret_i2c = i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);

	gd->secure_eeprom = 0;
	if (i2c_read_norsta(CFG_I2C_C1_9548SW1_P4_SEEPROM, 4, 1, 
		      (uint8_t *)&id, 4) == 0) {
	    i2c_read_norsta(CFG_I2C_C1_9548SW1_P4_SEEPROM, 0x72, 1,
		      (uint8_t *)&mcfg, 1);
	    if (id != 0xFFFFFFFF)
		gd->secure_eeprom = 1;
	}

	ret_i2c = i2c_read_norsta(CFG_I2C_C1_9548SW1_P4_SEEPROM, 0x31, 1,
			(uint8_t *)&hwreworkrev, 2);

	ch = 0;
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1); /* disable mux channel */
	gd->mem_cfg = mcfg;
	bid = (id & 0xffff0000) >> 16;

	/* select i2c controller 1 */
	i2c_set_bus_num(CFG_I2C_CTRL_1);

	gd->valid_bid = 1;
	gd->board_type = id;

	switch (board_id()) {
#ifdef CONFIG_EX4500
	case I2C_ID_JUNIPER_EX4500_40F:
	    puts("Board: EX4500-40F");
	    break;
	case I2C_ID_JUNIPER_EX4500_20F:
	    puts("Board: EX4500-20F");
	    break;
#endif
#ifdef CONFIG_EX4510
	case I2C_ID_JUNIPER_EX4510_32_F:
	    puts("Board: EX4510-32F");
	    break;
	case I2C_ID_JUNIPER_EX4510_32_T:
	    puts("Board: EX4510-32T");
	    break;
#endif
	default:
	    puts("Board: Unknown");
	    break;
	}

	printf(" %d.%d\n", version_major(), version_minor());
	epld_ver = version_epld();
	epld_reg_read(EPLD_LAST_RESET, (uint16_t *)&epldLastRst);
	printf("EPLD:  Version %d.%d (0x%02x)\n", (epld_ver & 0xff00) >> 8,
		    (epld_ver & 0x00C0) >> 6, epldLastRst & 0xFF);
	epld_reg_write(EPLD_LAST_RESET, 0);
	gd->last_reset = epldLastRst & 0xFF;

	printf("HW Re-work Revsion: %.2s\n", (char *) &hwreworkrev);

#ifdef CONFIG_EX4500
	 /* Initialize local bus */
	local_bus_init ();
#endif

	return 0;
}

phys_size_t initdram (int board_type)
{
	phys_size_t dram_size = 0;

	puts("Initializing ");

#if defined(CONFIG_DDR_DLL)
	{
	    /*
	     * Work around to stabilize DDR DLL MSYNC_IN.
	     * Errata DDR9 seems to have been fixed.
	     * This is now the workaround for Errata DDR11:
	     * Override DLL = 1, Course Adj = 1, Tap Select = 0
	     */

	    volatile ccsr_gur_t *gur= (void *)CONFIG_SYS_MPC85xx_GUTS_ADDR;

	    gur->ddrdllcr = 0x81000000;
	    asm("sync;isync;msync");
	    udelay(200);
	}
#endif
	dram_size = fixed_sdram();
	if (set_ddr_laws(CONFIG_SYS_DDR_SDRAM_BASE, dram_size,
			 LAW_TRGT_IF_DDR) < 0) {
	    puts("ERROR setting Local Access Windows for DDR\n");
	    return 0;
	}

	dram_size = setup_ddr_tlbs(dram_size / 0x100000);
	dram_size *= 0x100000;

	puts("DDR: ");

#if defined(CONFIG_DDR_ECC)
	/* Initialize and enable DDR ECC */
	if (mem_ecc()) {
	    ddr_enable_ecc(dram_size);
	}
#endif

	return (dram_size);
}

#if defined(CONFIG_SYS_DRAM_TEST)
int testdram(void)
{
	uint *pstart = (uint *) CONFIG_SYS_MEMTEST_START;
	uint *pend = (uint *) CONFIG_SYS_MEMTEST_END;
	uint *p;

	printf("Testing DRAM from 0x%08x to 0x%08x\n",
		    CONFIG_SYS_MEMTEST_START,CONFIG_SYS_MEMTEST_END);

	printf("DRAM test phase 1:\n");
	for (p = pstart; p < pend; p++)
	    *p = 0xaaaaaaaa;

	for (p = pstart; p < pend; p++) {
	    if (*p != 0xaaaaaaaa) {
		printf ("DRAM test fails at: %08x\n", (uint) p);
		return 1;
	    }
	}

	printf("DRAM test phase 2:\n");
	for (p = pstart; p < pend; p++)
	    *p = 0x55555555;

	for (p = pstart; p < pend; p++) {
	    if (*p != 0x55555555) {
		printf ("DRAM test fails at: %08x\n", (uint) p);
		return 1;
	    }
	}

	printf("DRAM test passed.\n");
	return 0;
}
#endif

#if defined(CONFIG_PCI)
/* Initialize PCI Devices, report devices found */
#ifndef CONFIG_PCI_PNP
static struct pci_config_table pci_java_config_table[] = {
	{ PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID,
	    PCI_IDSEL_NUMBER, PCI_ANY_ID,
	    pci_cfgfunc_config_device, {PCI_ENET0_IOADDR,
				PCI_ENET0_MEMADDR,
				PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER }
	},
	{ }
};
#endif

#ifndef CONFIG_FSL_CORENET
int is_fsl_pci_cfg(enum law_trgt_if trgt, u32 io_sel)
{
	return ((1 << io_sel) & pci_config_info[trgt].cfg);
}
#endif

struct pci_controller hose = {
#ifndef CONFIG_PCI_PNP
	config_table: pci_java_config_table,
#endif
};
#endif	/* CONFIG_PCI */

#ifdef CONFIG_PCIE1
struct pci_controller pcie1_hose[1];
#endif

static struct pci_controller pci1_hose;
void pci_init_board (void)
{
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	struct fsl_pci_info pci_info[4];
	u32 devdisr, pordevsr, io_sel, porpllsr;
#ifdef CONFIG_EX4500
	u32 pci_agent, pci_32, pci_arb, pci_clk_sel;
#endif
	int first_free_busno = 0;
	int num = 0;

	int pcie_ep, pcie_configured;

	devdisr = in_be32(&gur->devdisr);
	pordevsr = in_be32(&gur->pordevsr);
	porpllsr = in_be32(&gur->porpllsr);
	io_sel = (pordevsr & MPC85xx_PORDEVSR_IO_SEL) >> 19;

	debug ("   pci_init_board: devdisr=%x, io_sel=%x\n", devdisr, io_sel);

#ifdef CONFIG_EX4500
#ifdef CONFIG_PCI1

	pci_32 = pordevsr & MPC85xx_PORDEVSR_PCI1_PCI32;	/* PORDEVSR[15] */
	pci_arb = pordevsr & MPC85xx_PORDEVSR_PCI1_ARB;
	pci_clk_sel = porpllsr & MPC85xx_PORDEVSR_PCI1_SPD;

	if (!(devdisr & MPC85xx_DEVDISR_PCI1)) {
		SET_STD_PCI_INFO(pci_info[num], 1);
		pci_agent = fsl_setup_hose(&pci1_hose, pci_info[num].regs);
		printf ("\n    PCI: %d bit, %s, %s, %s (base address %lx)\n",
			(pci_32) ? 32 : 64,
			pci_clk_sel ? "sync" : "async",
			pci_agent ? "agent" : "host",
			pci_arb ? "arbiter" : "external-arbiter",
			pci_info[num].regs);

		first_free_busno = fsl_pci_init_port(&pci_info[num++],
					&pci1_hose, first_free_busno);

#ifdef CONFIG_PCIX_CHECK
		if (!(pordevsr & MPC85xx_PORDEVSR_PCI1)) {
			/* PCI-X init */
			if (CONFIG_SYS_CLK_FREQ < 66000000)
				printf("PCI-X will only work at 66 MHz\n");

			reg16 = PCI_X_CMD_MAX_SPLIT | PCI_X_CMD_MAX_READ
				| PCI_X_CMD_ERO | PCI_X_CMD_DPERR_E;
			pci_hose_write_config_word(hose, bus, PCIX_COMMAND, reg16);
		}
#endif
	} else {
		printf ("    PCI: disabled\n");
	}
#endif
#endif
	/*
	 * PCIE1 configuration is for both EX4500 and EX4510
	 */
#ifdef	CONFIG_PCIE1
	pcie_configured = is_fsl_pci_cfg(LAW_TRGT_IF_PCIE_1, io_sel);

	if (pcie_configured && !(devdisr & MPC85xx_DEVDISR_PCIE)){
	    SET_STD_PCIE_INFO(pci_info[num], 1);
	    pcie_ep = fsl_setup_hose(&pcie1_hose[0], pci_info[num].regs);
	    printf ("    PCIE1 connected to Slot as %s (base addr %lx)\n",
		    pcie_ep ? "Endpoint" : "Root Complex",
		    pci_info[num].regs);
	    first_free_busno = fsl_pci_init_port(&pci_info[num++],
						 &pcie1_hose[0], first_free_busno);
	} else {
	    printf ("    PCIE1: disabled\n");
	}

	puts("\n");
#endif
}

int misc_init_r (void)
{
	int i;

#ifdef CONFIG_EX4500
	volatile uint16_t pot;
	unsigned char ch;

	i2c_manual = 1;

	/* 2nd I2C controller */
	i2c_set_bus_num(CFG_I2C_CTRL_2);

	/* PCA9506#1 (U16) I/O expander */
	/* IO1_4, IO1_5 inputs, all other outputs */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x18, 0x30);
	/* IO1_0 - IO1_1 outputs, all other inputs */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x19, 0xFC);
	/* IO2_0 - IO2_7 all outputs */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x1A, 0x0);
	/* IO3_0 - IO3_7 all outputs */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x1B, 0x0);
	/* IO4_7 input, all other outputs */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x1C, 0x80);

	/* not invert all reset pins (active low) */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x10, 0x0);
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x11, 0x0);
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x12, 0x0);
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x13, 0x0);
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x14, 0x0);

	/* temporary */
	/* MUX2_RST_L, MUX3_RST_L low to high */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x8, 0x0);
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x8, 0x3);
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0x9, 0x0);
	/* CUB#0-7 out of reset */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0xA, 0xFF);
	/* CUB#8-9, MUXs, PEX, mgmt PHYs, LION#1 out ouf reset */
	/* MUX0_RST_L, MUX1_RST_L low to high */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0xB, 0xF3);
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0xB, 0xFF);
	/* LION#2 out of reset */
	i2c_reg_write(CFG_I2C_C2_9506EXP1, 0xC, 0x01);

	ch = 0x1;
	 /* enable mux port 0  */
	i2c_write(CFG_I2C_C2_9546SW1, 0, 0, &ch, 1);

	/* PCA9506#2 (U55) I/O expander */
	/* all outputs */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x18, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x19, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x1A, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x1B, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x1C, 0x0);

	/* not invert all reset pins (active low) */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x10, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x11, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x12, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x13, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x14, 0x0);

	/* temporary */
	/* all PHY#0-19 in reset */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x8, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0x9, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0xA, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0xB, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P0_9506EXP2, 0xC, 0x0);

	ch = 0x2;
	/* enable mux port 1  */
	i2c_write(CFG_I2C_C2_9546SW1, 0, 0, &ch, 1);

	/* PCA9506#3 (U65) I/O expander */
	/* all outputs */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x18, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x19, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x1A, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x1B, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x1C, 0x0);

	/* not invert all reset pins (active low) */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x10, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x11, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x12, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x13, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x14, 0x0);

	/* temporary */
	/* SFP+RS0 (0..19) SFP+RS1 (0..19) set to 0 */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x8, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0x9, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0xA, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0xB, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP3, 0xC, 0x0);


	/* PCA9506#4 (U66) I/O expander */
	/* all outputs */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x18, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x19, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x1A, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x1B, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x1C, 0x0);

	/* not invert all reset pins (active low) */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x10, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x11, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x12, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x13, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x14, 0x0);

	/* temporary */
	/* SFP+RS0 (20..39) SFP+RS1 (20..39) set to 0 */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x8, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x9, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0xA, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0xB, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0xC, 0x0);

	/* PCA9506#5 (U64) I/O expander */
	/* IO0_0-5 output,  IO0_6-7 NC */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x18, 0xC0);
	/* IO1_0-5 input,  IO1_6-7 output */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x19, 0x3F);
	/* IO2_0-7 NC */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x1A, 0x0);
	/* IO3_0-5 output,  IO3_6-7 input */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x1B, 0xC0);
	/* IO4_0-7 input */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x1C, 0xFF);

	/* not invert all reset pins (active low) */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x10, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x11, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x12, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x13, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x14, 0x0);

	/* temporary */
	/* set all to 0 */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x8, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0x9, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0xA, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0xB, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP4, 0xC, 0x0);

	ch = 0x4;
	/* enable mux port 2 */
	i2c_write(CFG_I2C_C2_9546SW1, 0, 0, &ch, 1);

	/* PCA9506#6 (U207) I/O expander */
	/* all inputs */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x18, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x19, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x1A, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x1B, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x1C, 0xFF);

	/* not invert all reset pins (active low) */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x10, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x11, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x12, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x13, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP6, 0x14, 0x0);

	/* PCA9506#7 (U210) I/O expander */
	/* all inputs */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x18, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x19, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x1A, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x1B, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x1C, 0xFF);

	/* not invert all reset pins (active low) */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x10, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x11, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x12, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x13, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP7, 0x14, 0x0);
	
	/* PCA9506#8 (U208) I/O expander */
	/* all inputs */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x18, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x19, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x1A, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x1B, 0xFF);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x1C, 0xFF);

	/* not invert all reset pins (active low) */
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x10, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x11, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x12, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x13, 0x0);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P2_9506EXP8, 0x14, 0x0);

	ch = 0x0;
	/* disable mux port */
	i2c_write(CFG_I2C_C2_9546SW1, 0, 0, &ch, 1);

	i2c_set_bus_num(CFG_I2C_CTRL_1);

	led_init();
	led_init_back();
	port_led_init();

	/* LCD contrast */
	i2c_set_bus_num(CFG_I2C_CTRL_1);
	pot = 0;
	ch = 1 << 3;
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);  /* enable mux port 3 */
	i2c_write(CFG_I2C_C1_9548SW1_P3_AD5245_1, 0, 0, (uint8_t*)&pot, 2);
	i2c_write(CFG_I2C_C1_9548SW1_P3_AD5245_2, 0, 0, (uint8_t*)&pot, 2); 
	ch = 0;
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);  /* disable mux port 3 */

	/* booting */
	set_led(MIDDLE_LED, LED_GREEN, LED_SLOW_BLINKING);
	set_led_back(MIDDLE_LED_BACK, LED_GREEN, LED_SLOW_BLINKING);

	i2c_manual = 0;
#endif
	   
	for (i = 0; i < CFG_LCD_NUM; i++)
	    lcd_init(i);
	
	udelay(200000);
	
	for (i = 0; i < CFG_LCD_NUM; i++)
	    lcd_init(i); /* second init is required */
	
	/* EPLD */
	epld_watchdog_init();

	/* Print the juniper internal version of the u-boot */
	printf("\nFirmware Version: %02X.%02X.%02X\n",
		    CFG_VERSION_MAJOR, CFG_VERSION_MINOR, CFG_VERSION_PATCH);

	return 0;
}

/* 
 * mode 0 - 88E1145
 * mode 1 - 88E1112
 * mode 2 - M2, M40
 * mode 3 - M80
 */
void mdio_mode_set (uint8_t mode, int inuse)
{
	unsigned char ch;
	uint8_t temp;
	int currCtrl;
	
	if (mode > 3) {
	    printf("mode %d out of range (0..3).\n", mode);
	    return;
	}

	mdio_inuse = inuse;

	if (curmode == mode)
	    return;

	curmode = mode;

	/* 2nd I2C controller */
	currCtrl = i2c_get_bus_num();
	i2c_set_bus_num(CFG_I2C_CTRL_2);
	ch = 0x2;
	/* enable mux port 1  */
	i2c_write(CFG_I2C_C2_9546SW1, 0, 0, &ch, 1);
	temp = i2c_reg_read(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x8);
	temp &= ~0x30;
	temp |= (mode << 4);
	i2c_reg_write(CFG_I2C_C2_9546SW1_P1_9506EXP5, 0x8, temp);
	ch = 0;
	/* disable mux port 1  */
	i2c_write(CFG_I2C_C2_9546SW1, 0, 0, &ch, 1);
	i2c_set_bus_num(currCtrl);
}

uint8_t mdio_mode_get (void)
{
	return curmode;
}

int mdio_get_inuse (void)
{
	return mdio_inuse;
}

uint mdio_mode_select(uint mode, struct tsec_private * priv)
{
	mdio_mode_set(mode & 0xf, (mode & 0xf0) ? 1 : 0);
	return 0;
}

int board_start_usb(void)
{
    usb_multi_root_hub = 0;
    usb_num_root_hub = 1;

    return 1;
}

int last_stage_init(void)
{
	struct eth_device *dev;
#ifdef CONFIG_EX4500
	volatile ccsr_gur_t *gur = (void *)CONFIG_SYS_MPC85xx_GUTS_ADDR;
	uint8_t ch, tdata, present, gpo = 0;
	int m1_40 = 0, m1_80 = 0;

	i2c_manual = 1;

	i2c_set_bus_num(CFG_I2C_CTRL_1);
#if defined(T40_REV3)
	ch = 1 << 0;
#else
	ch = 1 << 6;
#endif
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);
	
	/* W83782G#0 fan setup */
	tdata = 0x80;  /* bank 0 */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x4e, 1, &tdata, 1);
	tdata = 0x0;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x5c, 1, &tdata, 1);
	tdata = 0x84;  /* bank 4 */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x4e, 1, &tdata, 1);
	tdata = 0x0;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x5c, 1, &tdata, 1);
	tdata = 0x80;  /* bank 0 */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x4e, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_0, 0x47, 1, &tdata, 1);
	tdata |= 0xf0;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x47, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_0, 0x4b, 1, &tdata, 1);
	tdata |= 0xc0;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x4b, 1, &tdata, 1);
	tdata = 0xc0;
	/* all fan with 75% duty cycle */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x5b, 1, &tdata, 1);
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x5e, 1, &tdata, 1);
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x5f, 1, &tdata, 1);

	/* W83782G#1 fan setup */
	tdata = 0x80;  /* bank 0 */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x4e, 1, &tdata, 1);
	tdata = 0x0;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x5c, 1, &tdata, 1);
	tdata = 0x84;  /* bank 4 */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x4e, 1, &tdata, 1);
	tdata = 0x0;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x5c, 1, &tdata, 1);
	tdata = 0x80;  /* bank 0 */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x4e, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_1, 0x47, 1, &tdata, 1);
	tdata |= 0xf0;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x47, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_1, 0x4b, 1, &tdata, 1);
	tdata |= 0xc0;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x4b, 1, &tdata, 1);
	tdata = 0xc0;
	/* all fan with 75% duty cycle */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x5b, 1, &tdata, 1);
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x5e, 1, &tdata, 1);

	/* W83782G#0 temperature sensor setup */
	tdata = 0x80;  /* bank 0 */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x4e, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_0, 0x5d, 1, &tdata, 1);
	tdata |= 0x0e;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x5d, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_0, 0x59, 1, &tdata, 1);
	tdata &= ~0x70;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x59, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_0, 0x4b, 1, &tdata, 1);
	tdata &= 0xf3;
	tdata |= 0x08;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_0, 0x4b, 1, &tdata, 1);
	
	/* W83782G#1 temperature sensor setup */
	tdata = 0x80;  /* bank 0 */
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x4e, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_1, 0x5d, 1, &tdata, 1);
	tdata |= 0x0e;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x5d, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_1, 0x59, 1, &tdata, 1);
	tdata &= ~0x70;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x59, 1, &tdata, 1);
	i2c_read(CFG_I2C_C1_9548SW1_ENV_1, 0x4b, 1, &tdata, 1);
	tdata &= 0xf3;
	tdata |= 0x08;
	i2c_write(CFG_I2C_C1_9548SW1_ENV_1, 0x4b, 1, &tdata, 1);
	
	ch = 0;
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);

	/* RTC */
	ch = 1 << 5;
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);
	i2c_read(CFG_I2C_C1_9548SW1_P5_RTC, 2, 1, &tdata, 1);
	ch = 0;
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);
	if (tdata & 0x80) {  /* RTC voltage low */
	    rtc_init();
	    rtc_stop();
	    rtc_start();
	    /* default 2000/1/1 00:00:00 */
	    rtc_set_time(0, 1, 1, 0, 0, 0);
	}

	dev = eth_get_dev_by_name(CONFIG_TSEC4_NAME);
	if (dev)
	    mgmt_priv = (struct tsec_private *)dev->priv;

	/* module init */
	i2c_set_bus_num(CFG_I2C_CTRL_2);
	i2c_read(CFG_I2C_C2_9506EXP1, 0x1, 1, &present, 1);

	if ((present & 0x1c) != 0x1c) {  /* present */
	    i2c_read(CFG_I2C_C2_9506EXP1, 0xc, 1, &tdata, 1);
	    if ((present & 0x4) == 0) {  /* M1-40 present */
		tdata |= 0x12;  /* M1-40 power enable, i2c out of reset */
		gpo |= 0x80;  /* M1-40 out of reset */
		m1_40 = 1;
	    }

	    if ((present & 0x8) == 0) {  /* M1-80 present */
		tdata |= 0x24;  /* M1-80 power enable, i2c out of reset */
		gpo |= 0x40;  /* M1-80 out of reset */
		m1_80 = 1;
	    }

	    if ((present & 0x10) == 0) {  /* M2 present */
		tdata |= 0x48;  /* M2 power enable, i2c out of reset */
		gpo |= 0x20;  /* M2 out of reset */
	    }
	    i2c_write(CFG_I2C_C2_9506EXP1, 0xc, 1, &tdata, 1);
	    gur->gpoutdr |= gpo;         
	}
	i2c_manual = 0;

	if (m1_40) {
	    /* clear M1-SFP+ LED shift register */
	    udelay(10000);
	    epld_reg_write(EPLD_LED_AB_PORT_49_52, 0xF000);
	    udelay(1);
	    epld_reg_write(EPLD_LED_AB_PORT_49_52, 0);
	}
	
	if (m1_80) {
	    /* clear M1-SFP+ LED shift register */
	    udelay(10000);
	    epld_reg_write(EPLD_LED_AB_PORT_41_44, 0xF000);
	    udelay(1);
	    epld_reg_write(EPLD_LED_AB_PORT_41_44, 0);
	}
#endif /* CONFIG_EX4500 */
	
#ifdef CONFIG_EX4510
	volatile uint16_t fanCtrl;

	dev = eth_get_dev_by_name(CONFIG_TSEC1_NAME);
	if (dev)
	    mgmt_priv = (struct tsec_private *)dev->priv;
	/* 
	 * NOTE: In case rtc has to be inited, do it here.
	 * As of now we are not initializing rtc under u-boot.
	 */
	epld_reg_read(EPLD_FAN_CTRL_0_1, (uint16_t *)&fanCtrl);

	if (fanCtrl & FAN_0_PRNT) {
		fanCtrl = fanCtrl | FAN_0_LED_ON | FAN_0_DEFAULT_SPEED;
	}

	if (fanCtrl & FAN_1_PRNT) {
		fanCtrl = fanCtrl | FAN_1_LED_ON | FAN_1_DEFAULT_SPEED;
	}

	epld_reg_write(EPLD_FAN_CTRL_0_1, fanCtrl);
	fanCtrl = 0;

	epld_reg_read(EPLD_FAN_CTRL_2_3, (uint16_t *)&fanCtrl);

	if (fanCtrl & FAN_2_PRNT) {
		fanCtrl = fanCtrl | FAN_2_LED_ON | FAN_2_DEFAULT_SPEED;
	}

	if (fanCtrl & FAN_3_PRNT) {
		fanCtrl = fanCtrl | FAN_3_LED_ON | FAN_3_DEFAULT_SPEED;
	}
	
	epld_reg_write(EPLD_FAN_CTRL_2_3, fanCtrl);

#endif /* CONFIG_EX4510 */

	return 0;
}

/* retrieve MAC address from main board EEPROM */
void board_get_enetaddr (uchar * enet)
{
	unsigned char ch = 1 << 4;  /* port 4 */
	char temp[60], tmp;
	char *s;
	uint8_t esize = 0x40;
	uint16_t id;
	bd_t *bd = gd->bd;
	int mloop = 1;
	int i;
#if defined(CONFIG_READONLY)
	int state = get_upgrade_state();
#endif
	
	/* environment variable for board type */
	id = board_id();

	i2c_set_bus_num(CFG_I2C_CTRL_1);
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);

	enet[0] = 0x00;
	enet[1] = 0xE0;
	enet[2] = 0x0C;
	enet[3] = 0x00;
	enet[4] = 0x00;
	enet[5] = 0x00;
	    
	if (gd->secure_eeprom) {
	    secure_eeprom_read(CFG_I2C_C1_9548SW1_P4_SEEPROM, 
		CFG_EEPROM_MAC_OFFSET, enet, 6);
	    secure_eeprom_read(CFG_I2C_C1_9548SW1_P4_SEEPROM, 
		CFG_EEPROM_MAC_OFFSET-1, &esize, 1);
	}
	
	ch = 0;
	i2c_write(CFG_I2C_C1_9548SW1, 0, 0, &ch, 1);

#ifdef CONFIG_EX4500
	if (esize == 0x40) {
	    /* 
	     * As per the specification, the management interface is assigned the
	     * last MAC address out of the range programmed in the EEPROM. U-Boot
	     * was using the first in the range and this can cause problems when
	     * both U-Boot and JUNOS generate network traffic in short order. The
	     * switch to which the management interface is connected is exposed to
	     * a sudden MAC address change for the same IP in that case and may
	     * not forward packets correctly at first. This can cause timeouts and
	     * thus cause user-noticable failures.
	     *
	     * Using the same algorithm from JUNOS to guarantee the same MAC
	     * address picked for tsec0 in U-boot.
	     */
	     /*
	      * EX4500 ethernet mapping:
	      *
	      * Since loader only knows and uses bd->bi_enetaddr, T40 u-boot 
	      * remapped TSEC3 to bd->bi_enetaddr.  
	      * It is swapping the u-boot internal bd->bi_enetaddr and  
	      * bd->bi_enet3addr between TSEC0 and TSEC3.
	      * Note: U-boot is using eth3addr as MAC along with TSEC3 for ethernet
	      * packets.  The loader is using bd->bi_enetaddr as MAC for TSEC3 
	      * ethernet packets.
	      * 
	      *     TSEC    slot    ethXaddr(env)    bd->bi_enetXaddr
	      *     0       M1-80                              3 
	      *     1       M1-40      1                       1
	      *     2       M2         2                       2
	      *     3       T40        3                       <-- mgmt port
	      *
	      */
	    enet[5] += ((esize - 1) - 3);  /* bd->enetaddr, TSEC0 */
	}

	/* TSEC0 */
	tmp = enet[5];
	sprintf(temp, "%02x:%02x:%02x:%02x:%02x:%02x",
		    enet[0], enet[1], enet[2], enet[3], enet[4], tmp);
	s = getenv("ethaddr");
	if (memcmp(s, temp, sizeof(temp)) != 0) {
	    setenv("ethaddr", temp);
	}
	
	for (i = 0; i < 5; ++i) {
	    bd->bi_enet3addr[i] = enet[i];
	}
	bd->bi_enet3addr[5] = tmp;

	/* TSEC1 */
	tmp++;
	sprintf(temp, "%02x:%02x:%02x:%02x:%02x:%02x",
		    enet[0], enet[1], enet[2], enet[3], enet[4], tmp);
	s = getenv("eth1addr");
	if (memcmp(s, temp, sizeof(temp)) != 0) {
	    setenv("eth1addr", temp);
	}
	
	for (i = 0; i < 5; ++i) {
	    bd->bi_enet1addr[i] = enet[i];
	}
	bd->bi_enet1addr[5] = tmp;
	
	/* TSEC2 */
	tmp++;
	sprintf(temp, "%02x:%02x:%02x:%02x:%02x:%02x",
		    enet[0], enet[1], enet[2], enet[3], enet[4], tmp);
	s = getenv("eth2addr");
	if (memcmp(s, temp, sizeof(temp)) != 0) {
	    setenv("eth2addr", temp);
	}

	for (i = 0; i < 5; ++i) {
	    bd->bi_enet2addr[i] = enet[i];
	}
	bd->bi_enet2addr[5] = tmp;
	
	/* TSEC3 -- mgmt port*/
	/* set env for mgmt port only */
	tmp++;
	sprintf(temp, "%02x:%02x:%02x:%02x:%02x:%02x",
		    enet[0], enet[1], enet[2], enet[3], enet[4], tmp);
	s = getenv("eth3addr");
	if (memcmp(s, temp, sizeof(temp)) != 0) {
	    setenv("eth3addr", temp);
	}
	
	for (i = 0; i < 5; ++i) {
	    bd->bi_enetaddr[i] = enet[i];
	}
	bd->bi_enetaddr[5] = tmp;

	/*
	 * In case the board id is not populated or
	 * read incorrectly, try to set it to default
	 * board type
	 */
	if (id == 0xFFFF)
	    id = I2C_ID_JUNIPER_EX4500_40F;

	s = getenv("ethprime");
	if (!s)
	    setenv("ethprime", CONFIG_ETHPRIME);
#endif

#ifdef CONFIG_EX4510
	/*
	 * On Rev1 of EX4510_32_F
	 * eTSEC1 --> MGMT PORT
	 * eTSEC2 --> FRONT MODULE
	 * eTSEC3 --> REAR MODULE
	 *
	 * On Rev2 of EX4510_32_F and
	 * all revs of EX4510_32_T
	 * eTSEC1 --> UNUSED
	 * eTSEC2 --> FRONT MODULE
	 * eTSEC3 --> MGMT PORT
	 */
	if (esize == 0x40) {
	    enet[5] += (esize - 1);
	}
	tmp = enet[5];
	sprintf(temp, "%02x:%02x:%02x:%02x:%02x:%02x",
		    enet[0], enet[1], enet[2], enet[3], enet[4], tmp);

	if ((board_id() == I2C_ID_JUNIPER_EX4510_32_F) && (version_major() == 1)) {
	    s = getenv("ethaddr");
	    if (memcmp(s, temp, sizeof(temp)) != 0) {
		setenv("ethaddr", temp);
	    }
	} else {
	    s = getenv("eth2addr");
	    if (memcmp(s, temp, sizeof(temp)) != 0) {
		setenv("eth2addr", temp);
	    }
	}
	
	for (i = 0; i < 5; ++i) {
	    bd->bi_enetaddr[i] = enet[i];
	}
	
	bd->bi_enetaddr[5] = tmp;

	/*
	 * In case the board id is not populated or
	 * read incorrectly, try to set it to default
	 * board type
	 */
	if (id == 0xFFFF)
	    id = I2C_ID_JUNIPER_EX4510_32_F;
#endif

	sprintf(temp,"%04x ", id);
	setenv("hw.board.type", temp);

	/* environment variable for last reset */
	sprintf(temp,"%02x ", gd->last_reset);
	setenv("hw.board.reset", temp);

#if defined(CONFIG_READONLY)
	/* environment variable current U-boot image */
	sprintf(temp,"%s ", "readonly");
	setenv("boot.current", temp);

	sprintf(temp,"%d ", state);
	setenv("boot.state", temp);
#endif

#if defined(CONFIG_UPGRADE)
	s = (char *) (CFG_UPGRADE_BOOT_IMG_ADDR + 4);
	setenv("boot.upgrade.ver", s);
	s = (char *) (CFG_READONLY_BOOT_IMG_ADDR + 4);
	setenv("boot.readonly.ver", s);
#endif

#if defined(CONFIG_READONLY)
	s = NULL;
	if (state >= UPGRADE_CHECK) {
	    s = (char *) (CFG_UPGRADE_BOOT_IMG_ADDR + 4);
	}
	setenv("boot.upgrade.ver", s);
	s = (char *) (CFG_READONLY_BOOT_IMG_ADDR + 4);
	setenv("boot.readonly.ver", s);
#endif

	/* set the internal uboot version */
	sprintf(temp, " %d.%d.%d",
		CFG_VERSION_MAJOR, CFG_VERSION_MINOR, CFG_VERSION_PATCH);
	setenv("boot.intrver", temp);

	if ((s = getenv("boot.test")) != NULL) {
	    mloop = simple_strtoul (s, NULL, 16);
	    /*
	     * Since we relocate to the top 1GB,
	     * memtest should test only the possible
	     * ranges which u-boot will not use
	     */
	    if (memtest(0x2000, CONFIG_MEM_RELOCATE_TOP - 0x100000, mloop, 1)) {
	        setenv ("boot.ram", "FAILED ");
	    } else {
	        /*
	         * If the first region succeeds then
	         * go ahead with the second 1GB range
	         */
	        if (memtest(CONFIG_MEM_RELOCATE_TOP + 0x2000,
                            bd->bi_memsize - 0x2000, mloop, 1)) {
		    setenv ("boot.ram", "FAILED ");
	        } else {
		    setenv ("boot.ram", "PASSED ");
		}
	    }
	}

#if defined(CONFIG_READONLY) || defined(CONFIG_UPGRADE)

	/* uboot/loader memory map */
	sprintf(temp,"0x%08x ", CONFIG_SYS_FLASH_BASE);
	setenv("boot.flash.start", temp);

	sprintf(temp,"0x%08x ", CFG_FLASH_SIZE);
	setenv("boot.flash.size", temp);

	sprintf(temp,"0x%08x ", CFG_ENV_OFFSET);
	setenv("boot.env.start", temp);

	sprintf(temp,"0x%08x ", CFG_ENV_SIZE);
	setenv("boot.env.size", temp);

	sprintf(temp,"0x%08x ", CFG_OPQ_ENV_SECT_SIZE);
	setenv("boot.opqenv.start", temp);

	sprintf(temp,"0x%08x ", CFG_OPQ_ENV_SECT_SIZE);
	setenv("boot.opqenv.size", temp);

	sprintf(temp,"0x%08x ", CFG_UPGRADE_BOOT_STATE_OFFSET);
	setenv("boot.upgrade.state", temp);
	
	sprintf(temp,"0x%08x ", CFG_UPGRADE_LOADER_ADDR - CONFIG_SYS_FLASH_BASE);
	setenv("boot.upgrade.loader", temp);
	
	sprintf(temp,"0x%08x ", CFG_UPGRADE_BOOT_IMG_ADDR - CONFIG_SYS_FLASH_BASE);
	setenv("boot.upgrade.uboot", temp);

	sprintf(temp,"0x%08x ", CFG_READONLY_LOADER_ADDR - CONFIG_SYS_FLASH_BASE);
	setenv("boot.readonly.loader", temp);
	
	sprintf(temp,"0x%08x ", CFG_READONLY_BOOT_IMG_ADDR - CONFIG_SYS_FLASH_BASE);
	setenv("boot.readonly.uboot", temp);

#endif

	s = getenv("bootcmd.prior");
	if (s) {
	    sprintf(temp,"%s; %s", s, CONFIG_BOOTCOMMAND);
	    setenv("bootcmd", temp);
	} else {
	    setenv("bootcmd", CONFIG_BOOTCOMMAND);
	}

	sprintf(temp,"0x%02x ", CFG_I2C_C1_9548SW1_P4_SEEPROM);
	setenv("boot.ideeprom", temp);

	if (gd->env_valid == 3) {
	    /* save default environment */
	    gd->flags |= GD_FLG_SILENT;
	    saveenv();
	    gd->flags &= ~GD_FLG_SILENT;
	    gd->env_valid = 1;
	}

}

#ifdef CONFIG_SHOW_ACTIVITY
void board_show_activity (ulong timestamp)
{
	static int link_status = 0;
	int i, temp, temp_page;
	volatile uint16_t temp1;
	PortDuplex port_duplexity = 0;
	PortSpeed port_speed = 0;

	if (epld_reg_read(EPLD_WATCHDOG_ENABLE, (uint16_t *)&temp1)) {
	    if (temp1 & 0x1)  /* watchdog enabled */
		epld_reg_write(EPLD_WATCHDOG_SW_UPDATE, 0x1);
	}

#ifdef CONFIG_EX4500
	/*
	 * update the management i/f LED, every 2 seconds.
	 */
	if ((mdio_inuse == 0) && (i2c_manual == 0) && (i2c_post == 0) && 
	    (i2c_mfgcfg == 0) ) {
	    if (mgmt_priv && (timestamp % (2 * CONFIG_SYS_HZ) == 0)) {
		mdio_mode_set(2, 1);
		temp_page = read_phy_reg_private(mgmt_priv, 0x16);
		write_phy_reg_private(mgmt_priv, 0x16, 0);
		temp = read_phy_reg_private(mgmt_priv, 0x11);
		write_phy_reg_private(mgmt_priv, 0x16, temp_page);
		if ((temp & 0xe400) != (link_status & 0xe400)) {
		    link_status = temp;
		    if (link_status & MIIM_88E1011_PHYSTAT_LINK) {
			if (link_status & MIIM_88E1011_PHYSTAT_DUPLEX)
			    port_duplexity = FULL_DUPLEX;
			else
			    port_duplexity = HALF_DUPLEX;
			if (link_status & MIIM_88E1011_PHYSTAT_GBIT)
			    port_speed = SPEED_1G;
		    	else if (link_status & MIIM_88E1011_PHYSTAT_100)
			    port_speed = SPEED_100M;
			else
			    port_speed = SPEED_10M;

			set_port_led(0, LINK_UP, port_duplexity, port_speed);
		    } else {
			set_port_led(0, LINK_DOWN, 0, 0);  /* don't care */  
		    }
		}
		mdio_mode_set(0, 0);
	    }
	}

	/* 100ms */
	if (timestamp % (CONFIG_SYS_HZ /10) == 0) {
	    if ((i2c_manual == 0) && (i2c_post == 0) && (i2c_mfgcfg == 0) ) {
		update_led();
#if !defined(T40_REV5)
		update_led_back();
#endif
	    }
	}
#endif

#ifdef CONFIG_EX4510
	volatile uint16_t ledctrl;
	/*
	 * update the management i/f LED, every 2 seconds,
	 * logic is exactly similar to EX4500, but there is no
	 * requirement of any mode settings
	 */
	if (mgmt_priv && (timestamp % (2 * CONFIG_SYS_HZ) == 0)) {
	    temp_page = read_phy_reg_private(mgmt_priv, 0x16);
	    write_phy_reg_private(mgmt_priv, 0x16, 0x0);
	    temp = read_phy_reg_private(mgmt_priv, 0x11);
	    write_phy_reg_private(mgmt_priv, 0x16, temp_page);
	    if ((temp & 0xe400) != (link_status & 0xe400)) {
		link_status = temp;
		if (link_status & MIIM_88E1011_PHYSTAT_LINK) {
		    if (link_status & MIIM_88E1011_PHYSTAT_DUPLEX)
			port_duplexity = FULL_DUPLEX;
		    else 
			port_duplexity = HALF_DUPLEX;
		    if (link_status & MIIM_88E1011_PHYSTAT_GBIT)
			port_speed = SPEED_1G;
		    else if (link_status & MIIM_88E1011_PHYSTAT_100)
			port_speed = SPEED_100M;
		    else
			port_speed = SPEED_10M;

		    set_port_led(0, LINK_UP, port_duplexity, port_speed);
		} else {
		    set_port_led(0, LINK_DOWN, 0, 0);  /* don't care */
		}
	    }
	}

        /* Make the SYSTEM LED BLINK */
        if (timestamp % (CONFIG_SYS_HZ / 2) == 0) {
            if (epld_reg_read(EPLD_FRONT_LED_CTRL, (uint16_t *)&ledctrl)) {
                /* check if the SYSTEM LED GREEN is ON */
                if (ledctrl & SYS_LED_GREEN_ON) {
                    ledctrl = SYS_LED_GREEN_OFF; /* Turn if OFF */
                } else {
                    ledctrl = SYS_LED_GREEN_ON; /* Turn it ON */
                }
                epld_reg_write(EPLD_FRONT_LED_CTRL, ledctrl);
            }
        }
#endif

	/* 500ms */
	if (timestamp % (CONFIG_SYS_HZ / 2) == 0) {
	    for (i = 0; i < CFG_LCD_NUM; i++)
		lcd_heartbeat(i);
	}

}

void show_activity (int arg)
{
}
#endif

/*** Below post_word_store  and post_word_load  code is temporary code,
			 this will be modifed  ***/
#ifdef CONFIG_POST
DECLARE_GLOBAL_DATA_PTR;

/* post test name declartion */
static char *post_test_name[] = { "cpu","ram","pci","ethernet"};

void
post_run_ex45xx (void)
{
	int i;
	
	/* 
	 *  Run Post test to validate the cpu, ram, pci and ethernet 
	 *  (mac, phy loopback) 
	 */
	for (i = 0; i < sizeof(post_test_name) / sizeof(char *); i++) {
	    if (post_run(post_test_name[i],
			 POST_RAM | POST_MANUAL) != 0)
		printf(" Unable to execute the POST test %s \n",
		       post_test_name[i]);
	}

	if (post_result_pci == -1) {
	    setenv("post.pci", "FAILED ");
	    printf("POST: pci FAILED\n");
	}
	else {
	    setenv("post.pci", "PASSED ");
	}
	if (post_result_ether == -1) {
	    setenv("post.ethernet", "FAILED ");
	    printf("POST: ethernet FAILED\n");
	}
	else {
	    setenv("post.ethernet", "PASSED ");
	}
	if (gd->valid_bid == 0) {
	    setenv("post.eeprom", "FAILED ");
	    printf("POST: eeprom FAILED\n");
	}
	else {
	    setenv("post.eeprom", "PASSED ");
	}
}

void post_word_store (ulong a)
{
	volatile ulong  *save_addr_word =
			    (volatile ulong *)(POST_STOR_WORD);

	*save_addr_word = a;
}

ulong post_word_load (void)
{
	volatile ulong *save_addr_word =
			    (volatile ulong *)(POST_STOR_WORD);

	return (*save_addr_word);
}

/*
 * Returns 1 if keys pressed to start the power-on long-running tests
 * Called from board_init_f().
 */
int 
post_hotkeys_pressed (void)
{
	return 0;   /* No hotkeys supported */
}

#endif

int post_stop_prompt (void)
{
	char *s;
	int mem_result;
	
	if (getenv("boot.nostop")) return (0);

	if (((s = getenv("boot.ram")) != NULL) &&
	     ((strcmp(s,"PASSED") == 0) ||
	     (strcmp(s,"FAILED") == 0))) {
	     mem_result = 0;
	} else {
	     mem_result = post_result_mem;
	}
	if (mem_result == -1 || 
	    post_result_ether == -1 ||
	    gd->valid_bid == 0)
	    return 1;

	return 0;
}

int secure_eeprom_read (unsigned dev_addr, unsigned offset, 
			uchar *buffer, unsigned cnt)
{
	int alen = (offset > 255) ? 2 : 1;

	return i2c_read_norsta(dev_addr, offset, alen, buffer, cnt);
}

#ifdef CONFIG_EX4500
void reset_phy (void)
{
	struct eth_device *dev;

	/* reset mgmt port phy */
	dev = eth_get_dev_by_name(CONFIG_TSEC4_NAME);
	ether_tsec_reinit(dev);
}
#endif

#ifdef CONFIG_BOARD_RESET
void board_reset(void)
{
    epld_system_reset();
}
#endif