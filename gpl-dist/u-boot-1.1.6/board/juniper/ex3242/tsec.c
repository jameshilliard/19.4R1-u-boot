/*
 * Copyright (c) 2007-2011, Juniper Networks, Inc.
 * All rights reserved.
 *
 * Freescale Three Speed Ethernet Controller driver
 *
 * This software may be used and distributed according to the
 * terms of the GNU Public License, Version 2, incorporated
 * herein by reference.
 *
 * Copyright 2004 Freescale Semiconductor.
 * (C) Copyright 2003, Motorola, Inc.
 * author Andy Fleming
 *
 */

#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>

#if defined(CONFIG_EX3242)
#include "tsec.h"
#include <miiphy.h>

DECLARE_GLOBAL_DATA_PTR;

#define TX_BUF_CNT		2

static uint rxIdx;		/* index of the current RX buffer */
static uint txIdx;		/* index of the current TX buffer */
static int toggle_rx_errata =0;

typedef volatile struct rtxbd {
	txbd8_t txbd[TX_BUF_CNT];
	rxbd8_t rxbd[PKTBUFSRX];
} RTXBD;

struct tsec_info_struct {
	unsigned int phyaddr;
	u32 flags;
	unsigned int phyregidx;
};

/* The tsec_info structure contains 3 values which the
 * driver uses to determine how to operate a given ethernet
 * device. The information needed is:
 *  phyaddr - The address of the PHY which is attached to
 *	the given device.
 *
 *  flags - This variable indicates whether the device
 *	supports gigabit speed ethernet, and whether it should be
 *	in reduced mode.
 *
 *  phyregidx - This variable specifies which ethernet device
 *	controls the MII Management registers which are connected
 *	to the PHY.  For now, only TSEC1 (index 0) has
 *	access to the PHYs, so all of the entries have "0".
 *
 * The values specified in the table are taken from the board's
 * config file in include/configs/.  When implementing a new
 * board with ethernet capability, it is necessary to define:
 *   TSECn_PHY_ADDR
 *   TSECn_PHYIDX
 *
 * for n = 1,2,3, etc.  And for FEC:
 *   FEC_PHY_ADDR
 *   FEC_PHYIDX
 */
static struct tsec_info_struct tsec_info[] = {
#if defined(CONFIG_MPC85XX_TSEC1)
	{TSEC1_PHY_ADDR, TSEC_GIGABIT | TSEC_REDUCED | TSEC_SGMII, TSEC1_PHYIDX},
#else
	{0, 0, 0},
#endif
#if defined(CONFIG_MPC85XX_TSEC2)
	{TSEC2_PHY_ADDR, TSEC_GIGABIT | TSEC_REDUCED | TSEC_SGMII, TSEC2_PHYIDX},
#else
	{0, 0, 0},
#endif
#if defined(CONFIG_MPC85XX_TSEC3)
	{TSEC3_PHY_ADDR, TSEC_GIGABIT | TSEC_REDUCED | TSEC_SGMII | TSEC_INTERNAL, TSEC3_PHYIDX},
#else
	{0, 0, 0},
#endif
#if defined(CONFIG_MPC85XX_TSEC4)
	{TSEC4_PHY_ADDR, TSEC_GIGABIT | TSEC_REDUCED | TSEC_SGMII, TSEC4_PHYIDX},
#else
	{0, 0, 0},
#endif
};

#define MAXCONTROLLERS	(4)
static int relocated = 0;

static struct tsec_private *privlist[MAXCONTROLLERS];

#ifdef __GNUC__
static RTXBD rtx __attribute__ ((aligned(8)));
#else
#error "rtx must be 64-bit aligned"
#endif

static int tsec_send(struct eth_device *dev,
		     volatile void *packet, int length);
static int tsec_recv(struct eth_device *dev);
static int tsec_init(struct eth_device *dev, bd_t * bd);
int tsec_init_diag(struct eth_device *dev, bd_t * bd);
static void tsec_halt(struct eth_device *dev);
void init_registers(volatile tsec_t * regs);
static void startup_tsec(struct eth_device *dev);
int init_phy(struct eth_device *dev);
void write_phy_reg(struct tsec_private *priv, uint regnum, uint value);
uint read_phy_reg(struct tsec_private *priv, uint regnum);
struct phy_info *get_phy_info(struct eth_device *dev);
void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd);
static void adjust_link(struct eth_device *dev);
static void relocate_cmds(void);
static int tsec_miiphy_write(char *devname, unsigned char addr,
			     unsigned char reg, unsigned short value);
static int tsec_miiphy_read(char *devname, unsigned char addr,
			    unsigned char reg, unsigned short *value);

static void toggle_tsec_rx(struct eth_device *dev);
static int tsec_recv_local(struct eth_device *dev, void *recv_pack, int max_len);

static void toggle_tsec_rx(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	regs->maccfg1 &= ~MACCFG1_RX_EN;
	regs->maccfg1 |= MACCFG1_RX_EN;
}

/*
 * etsec recv routine
 */
static int tsec_recv_local(struct eth_device *dev, void *recv_pack, int max_len)
{
	int length = 0;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
	udelay(1000);	
	while(!(rtx.rxbd[rxIdx].status & RXBD_EMPTY)) {

		length = rtx.rxbd[rxIdx].length + length;

		if (!(rtx.rxbd[rxIdx].status & RXBD_STATS)) {
			length = length - 4;

			memcpy(recv_pack, (void*) NetRxPackets[rxIdx], length < max_len
				   ? length : max_len); 

		} else {
			printf("Got error %x\n",
					 (rtx.rxbd[rxIdx].status & RXBD_STATS));
		}

		rtx.rxbd[rxIdx].length = 0;

		/* Set the wrap bit if this is the last element in the list */
		rtx.rxbd[rxIdx].status =
			RXBD_EMPTY | (((rxIdx + 1) == PKTBUFSRX) ? RXBD_WRAP : 0);

		rxIdx = (rxIdx + 1) % PKTBUFSRX;
	}
	if (regs->ievent & IEVENT_BSY) {
		regs->ievent = IEVENT_BSY;
		regs->rstat = RSTAT_CLEAR_RHALT;
	}
	return length;
}

void write_phy_reg_tbi(struct tsec_private *priv, uint regnum, uint value)
{
	volatile tsec_t *regbase = priv->phyregs;
	volatile tsec_t *regs = priv->regs;
	int timeout = 1000000;

	regbase->miimadd = (regs->tbipa << 8) | regnum;
	regbase->miimcon = value;
	asm("sync");

	timeout = 1000000;
	while ((regbase->miimind & MIIMIND_BUSY) && timeout--) ;
}

uint read_phy_reg_tbi(struct tsec_private *priv, uint regnum)
{
	uint value;
	volatile tsec_t *regbase = priv->phyregs;
	volatile tsec_t *regs = priv->regs;

	/* Put the address of the phy, and the register
	 * number into MIIMADD */
	regbase->miimadd = (regs->tbipa << 8) | regnum;

	/* Clear the command register, and wait */
	regbase->miimcom = 0;
	asm("sync");

	/* Initiate a read command, and wait */
	regbase->miimcom = MIIM_READ_COMMAND;
	asm("sync");

	/* Wait for the the indication that the read is done */
	while ((regbase->miimind & (MIIMIND_NOTVALID | MIIMIND_BUSY))) ;

	/* Grab the value read from the PHY */
	value = regbase->miimstat;

	return value;
}

void sgmii_sequence (struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
	uint tbicr, tbisr, i, tempval;
	char tmpbuf[MAC_ADDR_LEN];

	/* Init MACCFG2.  Defaults to GMII */
	regs->maccfg2 = MACCFG2_INIT_SETTINGS;

	/* Init ECNTRL */
	regs->ecntrl = ECNTRL_INIT_SETTINGS;

	/* Copy the station address into the address registers.
	 * Backwards, because little endian MACS are dumb */
	for (i = 0; i < MAC_ADDR_LEN; i++) {
		tmpbuf[MAC_ADDR_LEN - 1 - i] = dev->enetaddr[i];
	}
	regs->macstnaddr1 = *((uint *) (tmpbuf));

	tempval = *((uint *) (tmpbuf + 4));

	regs->macstnaddr2 = tempval;

	/* Assign a Physical address to the TBI */
	regs->tbipa = TBIPA_VALUE - priv->phyaddr;
//	regs = (volatile tsec_t *)(TSEC_BASE_ADDR + TSEC_SIZE);
//	regs->tbipa = TBIPA_VALUE;
	asm("sync");

	/* Reset MII (due to new addresses) */
	priv->phyregs->miimcfg = MIIMCFG_RESET;
	asm("sync");
	priv->phyregs->miimcfg = MIIMCFG_INIT_VALUE;
	asm("sync");
	while (priv->phyregs->miimind & MIIMIND_BUSY) ;

	tbicr = read_phy_reg_tbi(priv, 0);
	debug("tbicr: %#x\n", tbicr);

	tbisr = read_phy_reg_tbi(priv, 1);
	debug("tbisr: %#x\n", tbisr);

	write_phy_reg_tbi(priv, 0x11, 0x20);

	write_phy_reg_tbi(priv, 0x4, 0x1a0);

       tempval = read_phy_reg(priv, 22);

       write_phy_reg(priv, 22, 0x8002);
       debug("Mac interface registers of PHY - page 2\n");
       for (i = 0; i < 32; i++) {
            debug("reg %d - %#04x\n", i, read_phy_reg(priv, i));
       }

       i = read_phy_reg(priv, 16);
       write_phy_reg(priv, 16, (i | 0x300) & 0xffff);
       debug("reg %d - %#04x\n", 16, read_phy_reg(priv, 16));

       i = read_phy_reg(priv, 26);
       write_phy_reg(priv, 26, (i | 0x10) & 0xffff);
       debug("reg %d - %#04x\n", 26, read_phy_reg(priv, 26));

       write_phy_reg(priv, 22, tempval & 0xffff);

	write_phy_reg_tbi(priv, 0x0, 0x1200);

	udelay(1000);

	tbisr = read_phy_reg_tbi(priv, 1);
	debug("tbisr: %#x\n", tbisr);
	
}

/* Initialize device structure. Returns success if PHY
 * initialization succeeded (i.e. if it recognizes the PHY)
 */
int tsec_initialize(bd_t * bis, int index, char *devname)
{
	struct eth_device *dev;
	int i;
	struct tsec_private *priv;
	uint svr;

	dev = (struct eth_device *)malloc(sizeof *dev);

	if (NULL == dev)
		return 0;

	memset(dev, 0, sizeof *dev);

	priv = (struct tsec_private *)malloc(sizeof(*priv));

	if (NULL == priv)
		return 0;

	privlist[index] = priv;
	priv->regs = (volatile tsec_t *)(TSEC_BASE_ADDR + index * TSEC_SIZE);
	priv->phyregs = (volatile tsec_t *)(TSEC_BASE_ADDR +
					    tsec_info[index].phyregidx *
					    TSEC_SIZE);

	priv->phyaddr = tsec_info[index].phyaddr;
	priv->flags = tsec_info[index].flags;

	sprintf(dev->name, devname);
	dev->iobase = 0;
	dev->priv = priv;
	dev->init = tsec_init;
	dev->halt = tsec_halt;
	dev->send = tsec_send;
	dev->recv = tsec_recv;

	/* Tell u-boot to get the addr from the env */
	for (i = 0; i < 6; i++)
		dev->enetaddr[i] = 0;

	eth_register(dev);

	/* Reset the MAC */
	priv->regs->maccfg1 |= MACCFG1_SOFT_RESET;
	priv->regs->maccfg1 &= ~(MACCFG1_SOFT_RESET);

#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII) \
	&& !defined(BITBANGMII)
	miiphy_register(dev->name, tsec_miiphy_read, tsec_miiphy_write);
#endif

	sgmii_sequence(dev);

	/* 8544 Rev 1.0 errata eTSEC 27: rx_en synchronization may cause unrecoverable
	   RX error at startup.  Workaround is to test one loopback packet.  If failed, toggle rx_en
	   bit.  This problem will be fix in Rev 1.1.  Setting toggle_rx_errata to 1 for Rev 1.0 CPU.
	 */
	svr = get_svr();
	if ((svr == 0x803c0110) || (svr == 0x80340110))
		toggle_rx_errata = 1;

	return init_phy(dev);
}

int tsec_init_diag(struct eth_device *dev, bd_t * bd)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	/* Reset the MAC */
	priv->regs->maccfg1 |= MACCFG1_SOFT_RESET;
	priv->regs->maccfg1 &= ~(MACCFG1_SOFT_RESET);
	sgmii_sequence(dev);
	return init_phy(dev);
}

/* Initializes data structures and registers for the controller,
 * and brings the interface up.	 Returns the link status, meaning
 * that it returns success if the link is up, failure otherwise.
 * This allows u-boot to find the first active controller.
 */
int tsec_init(struct eth_device *dev, bd_t * bd)
{
	uint tempval;
	char tmpbuf[MAC_ADDR_LEN];
	int i;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	/* Make sure the controller is stopped */
	tsec_halt(dev);

	/* Init MACCFG2.  Defaults to GMII */
	regs->maccfg2 = MACCFG2_INIT_SETTINGS;

	/* Init ECNTRL */
	regs->ecntrl = ECNTRL_INIT_SETTINGS;

	/* Copy the station address into the address registers.
	 * Backwards, because little endian MACS are dumb */
	for (i = 0; i < MAC_ADDR_LEN; i++) {
		tmpbuf[MAC_ADDR_LEN - 1 - i] = dev->enetaddr[i];
	}
	regs->macstnaddr1 = *((uint *) (tmpbuf));

	tempval = *((uint *) (tmpbuf + 4));

	regs->macstnaddr2 = tempval;

	/* reset the indices to zero */
	rxIdx = 0;
	txIdx = 0;

	/* Clear out (for the most part) the other registers */
	init_registers(regs);

	/* Ready the device for tx/rx */
	startup_tsec(dev);

	/* If there's no link, fail */
	return priv->link;

}

/* Write value to the device's PHY through the registers
 * specified in priv, modifying the register specified in regnum.
 * It will wait for the write to be done (or for a timeout to
 * expire) before exiting
 */
void write_phy_reg(struct tsec_private *priv, uint regnum, uint value)
{
	volatile tsec_t *regbase = priv->phyregs;
	uint phyid = priv->phyaddr;
	int timeout = 1000000;

	regbase->miimadd = (phyid << 8) | regnum;
	regbase->miimcon = value;
	asm("sync");

	timeout = 1000000;
	while ((regbase->miimind & MIIMIND_BUSY) && timeout--) ;
}

/* Reads register regnum on the device's PHY through the
 * registers specified in priv.	 It lowers and raises the read
 * command, and waits for the data to become valid (miimind
 * notvalid bit cleared), and the bus to cease activity (miimind
 * busy bit cleared), and then returns the value
 */
uint read_phy_reg(struct tsec_private *priv, uint regnum)
{
	uint value;
	volatile tsec_t *regbase = priv->phyregs;
	uint phyid = priv->phyaddr;

	/* Put the address of the phy, and the register
	 * number into MIIMADD */
	regbase->miimadd = (phyid << 8) | regnum;

	/* Clear the command register, and wait */
	regbase->miimcom = 0;
	asm("sync");

	/* Initiate a read command, and wait */
	regbase->miimcom = MIIM_READ_COMMAND;
	asm("sync");

	/* Wait for the the indication that the read is done */
	while ((regbase->miimind & (MIIMIND_NOTVALID | MIIMIND_BUSY))) ;

	/* Grab the value read from the PHY */
	value = regbase->miimstat;

	return value;
}

/* Discover which PHY is attached to the device, and configure it
 * properly.  If the PHY is not recognized, then return 0
 * (failure).  Otherwise, return 1
 */
int init_phy(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	struct phy_info *curphy;
	volatile tsec_t *regs = (volatile tsec_t *)(TSEC_BASE_ADDR);

	/* Assign a Physical address to the TBI */
       if (!(priv->flags & TSEC_SGMII)) {
	    regs->tbipa = TBIPA_VALUE;
	    regs = (volatile tsec_t *)(TSEC_BASE_ADDR + TSEC_SIZE);
	    regs->tbipa = TBIPA_VALUE;
	    asm("sync");

	    /* Reset MII (due to new addresses) */
	    priv->phyregs->miimcfg = MIIMCFG_RESET;
	    asm("sync");
	    if (priv->flags & TSEC_SGMII)
	        priv->phyregs->miimcfg = MIIMCFG_SGMII_INIT_VALUE;
	    else
	        priv->phyregs->miimcfg = MIIMCFG_INIT_VALUE;
	    asm("sync");
	    while (priv->phyregs->miimind & MIIMIND_BUSY) ;
       }

	if (0 == relocated)
		relocate_cmds();

	/* Get the cmd structure corresponding to the attached
	 * PHY */
	curphy = get_phy_info(dev);

	if (curphy == NULL) {
		priv->phyinfo = NULL;
		debug("%s: No PHY found\n", dev->name);

		return 0;
	}

	priv->phyinfo = curphy;

	phy_run_commands(priv, priv->phyinfo->config);

	return 1;
}

/*
 * Returns which value to write to the control register.
 * For 10/100, the value is slightly different
 */
uint mii_cr_init(uint mii_reg, struct tsec_private * priv)
{
	if (priv->flags & TSEC_GIGABIT)
		return MIIM_CONTROL_INIT;
	else
		return MIIM_CR_INIT;
}

/* Parse the status register for link, and then do
 * auto-negotiation
 */
uint mii_parse_sr(uint mii_reg, struct tsec_private * priv)
{
	/*
	 * Wait if PHY is capable of autonegotiation and autonegotiation
	 * is not complete.
	 */
	mii_reg = read_phy_reg(priv, MIIM_STATUS);
	if ((mii_reg & PHY_BMSR_AUTN_ABLE)
	    && !(mii_reg & PHY_BMSR_AUTN_COMP)) {
		int i = 0;

		puts("Waiting for PHY auto negotiation to complete");
		while (!((mii_reg & PHY_BMSR_AUTN_COMP)
			 && (mii_reg & MIIM_STATUS_LINK))) {
			/*
			 * Timeout reached ?
			 */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				puts(" TIMEOUT !\n");
				priv->link = 0;
				return 0;
			}

			if ((i++ % 1000) == 0) {
				putc('.');
			}
			udelay(1000);	/* 1 ms */
			mii_reg = read_phy_reg(priv, MIIM_STATUS);
		}
		puts(" done\n");
		priv->link = 1;
		udelay(500000);	/* another 500 ms (results in faster booting) */
	} else {
		priv->link = 1;
	}

	return 0;
}

/* Parse the 88E1011's status register for speed and duplex
 * information
 */
uint mii_parse_88E1011_psr(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	mii_reg = read_phy_reg(priv, MIIM_88E1011_PHY_STATUS);

	if (!((mii_reg & MIIM_88E1011_PHYSTAT_SPDDONE) &&
	      (mii_reg & MIIM_88E1011_PHYSTAT_LINK))) {
		int i = 0;

		puts("Waiting for PHY realtime link");
		while (!((mii_reg & MIIM_88E1011_PHYSTAT_SPDDONE) &&
			 (mii_reg & MIIM_88E1011_PHYSTAT_LINK))) {
			/*
			 * Timeout reached ?
			 */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				puts(" TIMEOUT !\n");
				priv->link = 0;
				break;
			}

			if ((i++ % 1000) == 0) {
				putc('.');
			}
			udelay(1000);	/* 1 ms */
			mii_reg = read_phy_reg(priv, MIIM_88E1011_PHY_STATUS);
		}
		puts(" done\n");
		udelay(500000);	/* another 500 ms (results in faster booting) */
	}

	if (mii_reg & MIIM_88E1011_PHYSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = (mii_reg & MIIM_88E1011_PHYSTAT_SPEED);

	switch (speed) {
	case MIIM_88E1011_PHYSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_88E1011_PHYSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
	}

	return 0;
}


/* Parse the 88E1112's status register for link
 * information
 */
uint mii_parse_88E1112_sr(uint mii_reg, struct tsec_private * priv)
{

	write_phy_reg(priv, 0x16, 0x2);  /* page 2 */
	mii_reg = read_phy_reg(priv, 0x11);

	if (!((mii_reg & 0x400) &&  /* link up */
	      (mii_reg & 0x40))) {     /* good sync */
		int i = 0;

		puts("Waiting for PHY realtime link");
		while (!((mii_reg & 0x400) &&
			 (mii_reg & 0x40))) {
			/*
			 * Timeout reached ?
			 */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				puts(" TIMEOUT !\n");
				priv->link = 0;
				break;
			}

			if ((i++ % 1000) == 0) {
				putc('.');
			}
			udelay(1000);	/* 1 ms */
			mii_reg = read_phy_reg(priv, MIIM_88E1011_PHY_STATUS);
		}
		puts(" done\n");
		priv->link = 1;
		udelay(500000);	/* another 500 ms (results in faster booting) */
	} else {
		priv->link = 1;
	}
	write_phy_reg(priv, 0x16, 0x0);  /* page 0 */

	return 0;
}

/* Parse the cis8201's status register for speed and duplex
 * information
 */
uint mii_parse_cis8201(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	if (mii_reg & MIIM_CIS8201_AUXCONSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = mii_reg & MIIM_CIS8201_AUXCONSTAT_SPEED;
	switch (speed) {
	case MIIM_CIS8201_AUXCONSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_CIS8201_AUXCONSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
		break;
	}

	return 0;
}

/* Parse the vsc8244's status register for speed and duplex
 * information
 */
uint mii_parse_vsc8244(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	if (mii_reg & MIIM_VSC8244_AUXCONSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = mii_reg & MIIM_VSC8244_AUXCONSTAT_SPEED;
	switch (speed) {
	case MIIM_VSC8244_AUXCONSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_VSC8244_AUXCONSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
		break;
	}

	return 0;
}

/* Parse the DM9161's status register for speed and duplex
 * information
 */
uint mii_parse_dm9161_scsr(uint mii_reg, struct tsec_private * priv)
{
	if (mii_reg & (MIIM_DM9161_SCSR_100F | MIIM_DM9161_SCSR_100H))
		priv->speed = 100;
	else
		priv->speed = 10;

	if (mii_reg & (MIIM_DM9161_SCSR_100F | MIIM_DM9161_SCSR_10F))
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	return 0;
}

/*
 * Hack to write all 4 PHYs with the LED values
 */
uint mii_cis8204_fixled(uint mii_reg, struct tsec_private * priv)
{
	uint phyid;
	volatile tsec_t *regbase = priv->phyregs;
	int timeout = 1000000;

	for (phyid = 0; phyid < 4; phyid++) {
		regbase->miimadd = (phyid << 8) | mii_reg;
		regbase->miimcon = MIIM_CIS8204_SLEDCON_INIT;
		asm("sync");

		timeout = 1000000;
		while ((regbase->miimind & MIIMIND_BUSY) && timeout--) ;
	}

	return MIIM_CIS8204_SLEDCON_INIT;
}

uint mii_cis8204_setmode(uint mii_reg, struct tsec_private * priv)
{
	if (priv->flags & TSEC_REDUCED)
		return MIIM_CIS8204_EPHYCON_INIT | MIIM_CIS8204_EPHYCON_RGMII;
	else
		return MIIM_CIS8204_EPHYCON_INIT;
}

/* Initialized required registers to appropriate values, zeroing
 * those we don't care about (unless zero is bad, in which case,
 * choose a more appropriate value)
 */
void init_registers(volatile tsec_t * regs)
{
	/* Clear IEVENT */
	regs->ievent = IEVENT_INIT_CLEAR;

	regs->imask = IMASK_INIT_CLEAR;

	regs->hash.iaddr0 = 0;
	regs->hash.iaddr1 = 0;
	regs->hash.iaddr2 = 0;
	regs->hash.iaddr3 = 0;
	regs->hash.iaddr4 = 0;
	regs->hash.iaddr5 = 0;
	regs->hash.iaddr6 = 0;
	regs->hash.iaddr7 = 0;

	regs->hash.gaddr0 = 0;
	regs->hash.gaddr1 = 0;
	regs->hash.gaddr2 = 0;
	regs->hash.gaddr3 = 0;
	regs->hash.gaddr4 = 0;
	regs->hash.gaddr5 = 0;
	regs->hash.gaddr6 = 0;
	regs->hash.gaddr7 = 0;

	regs->rctrl = 0x00000000;

	/* Init RMON mib registers */
	memset((void *)&(regs->rmon), 0, sizeof(rmon_mib_t));

	regs->rmon.cam1 = 0xffffffff;
	regs->rmon.cam2 = 0xffffffff;

	regs->mrblr = MRBLR_INIT_SETTINGS;

	regs->minflr = MINFLR_INIT_SETTINGS;

	regs->attr = ATTR_INIT_SETTINGS;
	regs->attreli = ATTRELI_INIT_SETTINGS;

}

/* Configure maccfg2 based on negotiated speed and duplex
 * reported by PHY handling code
 */
static void adjust_link(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
       
	if (priv->link) {
		if (priv->duplexity != 0)
			regs->maccfg2 |= MACCFG2_FULL_DUPLEX;
		else
			regs->maccfg2 &= ~(MACCFG2_FULL_DUPLEX);

		switch (priv->speed) {
		case 1000:
			regs->maccfg2 = ((regs->maccfg2 & ~(MACCFG2_IF))
					 | MACCFG2_GMII);
			break;
		case 100:
		case 10:
			regs->maccfg2 = ((regs->maccfg2 & ~(MACCFG2_IF))
					 | MACCFG2_MII);

			/* If We're in reduced mode, we need
			 * to say whether we're 10 or 100 MB.
			 */
			if ((priv->speed == 100)
			    && (priv->flags & TSEC_REDUCED))
				regs->ecntrl |= ECNTRL_R100;
			else
				regs->ecntrl &= ~(ECNTRL_R100);
			break;
		default:
			printf("%s: Speed was bad\n", dev->name);
			break;
		}

		printf("Speed: %d, %s duplex\n", priv->speed,
		       (priv->duplexity) ? "full" : "half");

	} else {
		printf("%s: No link.\n", dev->name);
	}
}

/* Set up the buffers and their descriptors, and bring up the
 * interface
 */
static void startup_tsec(struct eth_device *dev)
{
	int i;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	/* Point to the buffer descriptors */
	regs->tbase = (unsigned int)(&rtx.txbd[txIdx]);
	regs->rbase = (unsigned int)(&rtx.rxbd[rxIdx]);

	/* Initialize the Rx Buffer descriptors */
	for (i = 0; i < PKTBUFSRX; i++) {
		rtx.rxbd[i].status = RXBD_EMPTY;
		rtx.rxbd[i].length = 0;
		rtx.rxbd[i].bufPtr = (uint) NetRxPackets[i];
	}
	rtx.rxbd[PKTBUFSRX - 1].status |= RXBD_WRAP;

	/* Initialize the TX Buffer Descriptors */
	for (i = 0; i < TX_BUF_CNT; i++) {
		rtx.txbd[i].status = 0;
		rtx.txbd[i].length = 0;
		rtx.txbd[i].bufPtr = 0;
	}
	rtx.txbd[TX_BUF_CNT - 1].status |= TXBD_WRAP;

	/* Start up the PHY */
	if(priv->phyinfo)
		phy_run_commands(priv, priv->phyinfo->startup);
	adjust_link(dev);

	/* Enable Transmit and Receive */
	regs->maccfg1 |= (MACCFG1_RX_EN | MACCFG1_TX_EN);

	/* Tell the DMA it is clear to go */
	regs->dmactrl |= DMACTRL_INIT_SETTINGS;
	regs->tstat = TSTAT_CLEAR_THALT;
	regs->rstat = RSTAT_CLEAR_RHALT;
	regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);

	if (toggle_rx_errata) {
           char packet_send[64];
	    char packet_recv[64];
           int j, length;

           packet_send[0] = 0xFF;
	    packet_send[1] = 0xFF;
	    packet_send[2] = 0xFF;
	    packet_send[3] = 0xFF;
	    packet_send[4] = 0xFF;
	    packet_send[5] = 0xFF;

	    /* 8544 Rev 1.0 errata eTSEC 27: rx_en synchronization may cause unrecoverable
	       RX error at startup.  Workaround is to test one loopback packet.  If failed, toggle rx_en
	       bit.
	     */
	    regs->maccfg1 |= MACCFG1_LOOPBACK;
	    for (j = 0; j < 2; j++) {
               tsec_send (dev,packet_send, 64);
               /* Give a little time before trying to receive. */
               udelay(10);
               length = tsec_recv_local (dev, packet_recv, 64);
               if (length == 0) {
                   toggle_tsec_rx(dev);
		 }
	    }
	    regs->maccfg1 &= ~MACCFG1_LOOPBACK;
	}
}

/* This returns the status bits of the device.	The return value
 * is never checked, and this is what the 8260 driver did, so we
 * do the same.	 Presumably, this would be zero if there were no
 * errors
 */
static int tsec_send(struct eth_device *dev, volatile void *packet, int length)
{
	int i;
	int result = 0;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	/* Find an empty buffer descriptor */
	for (i = 0; rtx.txbd[txIdx].status & TXBD_READY; i++) {
		if (i >= TOUT_LOOP) {
			debug("%s: tsec: tx buffers full\n", dev->name);
			return result;
		}
	}

	rtx.txbd[txIdx].bufPtr = (uint) packet;
	rtx.txbd[txIdx].length = length;
	rtx.txbd[txIdx].status |=
	    (TXBD_READY | TXBD_LAST | TXBD_CRC | TXBD_INTERRUPT);

	/* Tell the DMA to go */
	regs->tstat = TSTAT_CLEAR_THALT;
	regs->dmactrl |= DMACTRL_TOD; //cleanup sumatra bringup

	/* Wait for buffer to be transmitted */
	for (i = 0; rtx.txbd[txIdx].status & TXBD_READY; i++) {
		if (i >= TOUT_LOOP) {
			debug("%s: tsec: tx error\n", dev->name);
			return result;
		}
	}

	txIdx = (txIdx + 1) % TX_BUF_CNT;
	result = rtx.txbd[txIdx].status & TXBD_STATS;

	return result;
}

static int tsec_recv(struct eth_device *dev)
{
	int length;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	while (!(rtx.rxbd[rxIdx].status & RXBD_EMPTY)) {

		length = rtx.rxbd[rxIdx].length;

		/* Send the packet up if there were no errors */
		if (!(rtx.rxbd[rxIdx].status & RXBD_STATS)) {
			NetReceive(NetRxPackets[rxIdx], length - 4);
		} else {
			printf("Got error %x\n",
			       (rtx.rxbd[rxIdx].status & RXBD_STATS));
		}

		rtx.rxbd[rxIdx].length = 0;

		/* Set the wrap bit if this is the last element in the list */
		rtx.rxbd[rxIdx].status =
		    RXBD_EMPTY | (((rxIdx + 1) == PKTBUFSRX) ? RXBD_WRAP : 0);

		rxIdx = (rxIdx + 1) % PKTBUFSRX;
	}

	if (regs->ievent & IEVENT_BSY) {
		regs->ievent = IEVENT_BSY;
		regs->rstat = RSTAT_CLEAR_RHALT;
	}

	return -1;

}

/* Stop the interface */
static void tsec_halt(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);
	regs->dmactrl |= (DMACTRL_GRS | DMACTRL_GTS);

	while (!(regs->ievent & (IEVENT_GRSC | IEVENT_GTSC))) ;

	regs->maccfg1 &= ~(MACCFG1_TX_EN | MACCFG1_RX_EN);

	/* Shut down the PHY, as needed */
	if(priv->phyinfo)
		phy_run_commands(priv, priv->phyinfo->shutdown);
}

struct phy_info phy_info_M88E1011S = {
	0x01410c6,
	"Marvell 88E1011S",
	4,
	(struct phy_cmd[]){	/* config */
			   /* Reset and configure the PHY */
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
			   {0x1d, 0x1f, NULL},
			   {0x1e, 0x200c, NULL},
			   {0x1d, 0x5, NULL},
			   {0x1e, 0x0, NULL},
			   {0x1e, 0x100, NULL},
			   {MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
			   {MIIM_ANAR, MIIM_ANAR_INIT, NULL},
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Status is read once to clear old link state */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the status */
			   {MIIM_88E1011_PHY_STATUS, miim_read,
			    &mii_parse_88E1011_psr},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

struct phy_info phy_info_M88E1111S = {
	0x01410cc,
	"Marvell 88E1111S",
	4,
	(struct phy_cmd[]){	/* config */
			   /* Reset and configure the PHY */
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
			   {0x1d, 0x1f, NULL},
			   {0x1e, 0x200c, NULL},
			   {0x1d, 0x5, NULL},
			   {0x1e, 0x0, NULL},
			   {0x1e, 0x100, NULL},
			   {MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
			   {MIIM_ANAR, MIIM_ANAR_INIT, NULL},
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Status is read once to clear old link state */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the status */
			   {MIIM_88E1011_PHY_STATUS, miim_read,
			    &mii_parse_88E1011_psr},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

/* SGMII */
struct phy_info phy_info_M88E1112S = {
	0x01410c9,
	"Marvell 88E1112S",
	4,
	(struct phy_cmd[]){	/* config *//* sgmii copper */
			   /* Reset and configure the PHY */
			   {MIIM_88E1112_PHY_PAGE, 0x2, NULL},  /* page 2 */
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
			   {MIIM_88E1112_PHY_PAGE, 0x0, NULL},  /* page 0 */
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
			   {MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
			   {MIIM_ANAR, MIIM_ANAR_INIT, NULL},
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
			   {MIIM_88E1112_PHY_PAGE, 0x3, NULL},  /* page 3 */
			   {MIIM_88E1112_PHY_LED_CONTROL, MIIM_88E1112_PHY_LED_DEFAULT, NULL},  /* link LED */
			   {MIIM_88E1112_PHY_PAGE, 0x0, NULL},  /* page 0 */
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Status is read once to clear old link state */
			   /* Status is read once to clear old link state */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   {MIIM_88E1011_PHY_STATUS, miim_read,
			    &mii_parse_88E1011_psr},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

static unsigned int m88e1145_setmode(uint mii_reg, struct tsec_private *priv)
{
	uint mii_data = read_phy_reg(priv, mii_reg);

	/* Setting MIIM_88E1145_PHY_EXT_CR */
	if (priv->flags & TSEC_REDUCED)
		return mii_data |
		    MIIM_M88E1145_RGMII_RX_DELAY | MIIM_M88E1145_RGMII_TX_DELAY;
	else
		return mii_data;
}

static struct phy_info phy_info_M88E1145 = {
	0x01410cd,
	"Marvell 88E1145",
	4,
	(struct phy_cmd[]){	/* config */
			   /* Reset and configure the PHY */
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},

			   /* Errata E0, E1 */
			   {29, 0x001b, NULL},
			   {30, 0x418f, NULL},
			   {29, 0x0016, NULL},
			   {30, 0xa2da, NULL},

			   {MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
			   {MIIM_ANAR, MIIM_ANAR_INIT, NULL},
			   {MIIM_88E1011_PHY_SCR, MIIM_88E1011_PHY_MDI_X_AUTO,
			    NULL},
			   {MIIM_88E1145_PHY_EXT_CR, 0, &m88e1145_setmode},
			   {MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, NULL},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Status is read once to clear old link state */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   {MIIM_88E1111_PHY_LED_CONTROL,
			    MIIM_88E1111_PHY_LED_DIRECT, NULL},
			   /* Read the Status */
			   {MIIM_88E1011_PHY_STATUS, miim_read,
			    &mii_parse_88E1011_psr},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

struct phy_info phy_info_cis8204 = {
	0x3f11,
	"Cicada Cis8204",
	6,
	(struct phy_cmd[]){	/* config */
			   /* Override PHY config settings */
			   {MIIM_CIS8201_AUX_CONSTAT,
			    MIIM_CIS8201_AUXCONSTAT_INIT, NULL},
			   /* Configure some basic stuff */
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
			   {MIIM_CIS8204_SLED_CON, MIIM_CIS8204_SLEDCON_INIT,
			    &mii_cis8204_fixled},
			   {MIIM_CIS8204_EPHY_CON, MIIM_CIS8204_EPHYCON_INIT,
			    &mii_cis8204_setmode},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Read the Status (2x to make sure link is right) */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the status */
			   {MIIM_CIS8201_AUX_CONSTAT, miim_read,
			    &mii_parse_cis8201},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

/* Cicada 8201 */
struct phy_info phy_info_cis8201 = {
	0xfc41,
	"CIS8201",
	4,
	(struct phy_cmd[]){	/* config */
			   /* Override PHY config settings */
			   {MIIM_CIS8201_AUX_CONSTAT,
			    MIIM_CIS8201_AUXCONSTAT_INIT, NULL},
			   /* Set up the interface mode */
			   {MIIM_CIS8201_EXT_CON1, MIIM_CIS8201_EXTCON1_INIT,
			    NULL},
			   /* Configure some basic stuff */
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Read the Status (2x to make sure link is right) */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the status */
			   {MIIM_CIS8201_AUX_CONSTAT, miim_read,
			    &mii_parse_cis8201},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};
struct phy_info phy_info_VSC8244 = {
	0x3f1b,
	"Vitesse VSC8244",
	6,
	(struct phy_cmd[]){	/* config */
			   /* Override PHY config settings */
			   /* Configure some basic stuff */
			   {MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Read the Status (2x to make sure link is right) */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the status */
			   {MIIM_VSC8244_AUX_CONSTAT, miim_read,
			    &mii_parse_vsc8244},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

struct phy_info phy_info_dm9161 = {
	0x0181b88,
	"Davicom DM9161E",
	4,
	(struct phy_cmd[]){	/* config */
			   {MIIM_CONTROL, MIIM_DM9161_CR_STOP, NULL},
			   /* Do not bypass the scrambler/descrambler */
			   {MIIM_DM9161_SCR, MIIM_DM9161_SCR_INIT, NULL},
			   /* Clear 10BTCSR to default */
			   {MIIM_DM9161_10BTCSR, MIIM_DM9161_10BTCSR_INIT,
			    NULL},
			   /* Configure some basic stuff */
			   {MIIM_CONTROL, MIIM_CR_INIT, NULL},
			   /* Restart Auto Negotiation */
			   {MIIM_CONTROL, MIIM_DM9161_CR_RSTAN, NULL},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Status is read once to clear old link state */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the status */
			   {MIIM_DM9161_SCSR, miim_read,
			    &mii_parse_dm9161_scsr},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

uint mii_parse_lxt971_sr2(uint mii_reg, struct tsec_private *priv)
{
	unsigned int speed;
	if (priv->link) {
		speed = mii_reg & MIIM_LXT971_SR2_SPEED_MASK;

		switch (speed) {
		case MIIM_LXT971_SR2_10HDX:
			priv->speed = 10;
			priv->duplexity = 0;
			break;
		case MIIM_LXT971_SR2_10FDX:
			priv->speed = 10;
			priv->duplexity = 1;
			break;
		case MIIM_LXT971_SR2_100HDX:
			priv->speed = 100;
			priv->duplexity = 0;
		default:
			priv->speed = 100;
			priv->duplexity = 1;
			break;
		}
	} else {
		priv->speed = 0;
		priv->duplexity = 0;
	}

	return 0;
}

static struct phy_info phy_info_lxt971 = {
	0x0001378e,
	"LXT971",
	4,
	(struct phy_cmd[]){	/* config */
			   {MIIM_CR, MIIM_CR_INIT, mii_cr_init},	/* autonegotiate */
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup - enable interrupts */
			   /* { 0x12, 0x00f2, NULL }, */
			   {MIIM_STATUS, miim_read, NULL},
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   {MIIM_LXT971_SR2, miim_read, &mii_parse_lxt971_sr2},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown - disable interrupts */
			   {miim_end,}
			   },
};

/* Parse the DP83865's link and auto-neg status register for speed and duplex
 * information
 */
uint mii_parse_dp83865_lanr(uint mii_reg, struct tsec_private *priv)
{
	switch (mii_reg & MIIM_DP83865_SPD_MASK) {

	case MIIM_DP83865_SPD_1000:
		priv->speed = 1000;
		break;

	case MIIM_DP83865_SPD_100:
		priv->speed = 100;
		break;

	default:
		priv->speed = 10;
		break;

	}

	if (mii_reg & MIIM_DP83865_DPX_FULL)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	return 0;
}

struct phy_info phy_info_dp83865 = {
	0x20005c7,
	"NatSemi DP83865",
	4,
	(struct phy_cmd[]){	/* config */
			   {MIIM_CONTROL, MIIM_DP83865_CR_INIT, NULL},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* startup */
			   /* Status is read once to clear old link state */
			   {MIIM_STATUS, miim_read, NULL},
			   /* Auto-negotiate */
			   {MIIM_STATUS, miim_read, &mii_parse_sr},
			   /* Read the link and auto-neg status */
			   {MIIM_DP83865_LANR, miim_read,
			    &mii_parse_dp83865_lanr},
			   {miim_end,}
			   },
	(struct phy_cmd[]){	/* shutdown */
			   {miim_end,}
			   },
};

struct phy_info *phy_info[] = {
#if 0
	&phy_info_cis8201,
#endif
	&phy_info_cis8204,
	&phy_info_M88E1011S,
	&phy_info_M88E1111S,
	&phy_info_M88E1112S,
	&phy_info_M88E1145,
	&phy_info_dm9161,
	&phy_info_lxt971,
	&phy_info_VSC8244,
	&phy_info_dp83865,
	NULL
};

/* Grab the identifier of the device's PHY, and search through
 * all of the known PHYs to see if one matches.	 If so, return
 * it, if not, return NULL
 */
struct phy_info *get_phy_info(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	uint phy_reg, phy_ID, old_phyaddr = priv->phyaddr;
	int i;
	struct phy_info *theInfo = NULL;
       extern int board_id(void);

	if (priv->flags & TSEC_INTERNAL)
           return NULL;
    
    // support Cheetah access via extra MDIO/MDC wires to CPU
    // allow 0x0D as alternative MgmtPhy address (1 config wire change)
    // to avoid conflict with Cheetah CpuPhyAddr 1..3 on modified MII bus
    priv->phyaddr = 0x0D;
    phy_reg = read_phy_reg(priv, MIIM_PHYIR1);
    if ((phy_reg != 0) && (phy_reg != 0xffff))
    {
       printf ("Alternative PhyAddr for MDIO workaround detected\n");
    }
    else
    {    
           for (i=0; i<TSEC4_PHY_ADDR+2; i++) {
               priv->phyaddr = i;
               phy_reg = read_phy_reg(priv, MIIM_PHYIR1);
               if ((phy_reg != 0) && (phy_reg != 0xffff))
                   break;
           }
           if (priv->phyaddr > TSEC4_PHY_ADDR)
               priv->phyaddr = old_phyaddr;
    }   
	/* Grab the bits from PHYIR1, and put them in the upper half */
	phy_reg = read_phy_reg(priv, MIIM_PHYIR1);
	phy_ID = (phy_reg & 0xffff) << 16;

	/* Grab the bits from PHYIR2, and put them in the lower half */
	phy_reg = read_phy_reg(priv, MIIM_PHYIR2);
	phy_ID |= (phy_reg & 0xffff);

	/* loop through all the known PHY types, and find one that */
	/* matches the ID we read from the PHY. */
	for (i = 0; phy_info[i]; i++) {
		if (phy_info[i]->id == (phy_ID >> phy_info[i]->shift))
			theInfo = phy_info[i];
	}

	if (theInfo == NULL) {
		printf("%s: PHY id %x is not supported!\n", dev->name, phy_ID);
		return NULL;
	} else {
		debug("%s: PHY is %s (%x)\n", dev->name, theInfo->name, phy_ID);
	}

	return theInfo;
}

/* Execute the given series of commands on the given device's
 * PHY, running functions as necessary
 */
void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd)
{
	int i;
	uint result;
	volatile tsec_t *phyregs = priv->phyregs;

	phyregs->miimcfg = MIIMCFG_RESET;

	if (priv->flags & TSEC_SGMII)
	    phyregs->miimcfg = MIIMCFG_SGMII_INIT_VALUE;
	else
	    phyregs->miimcfg = MIIMCFG_INIT_VALUE;

	while (phyregs->miimind & MIIMIND_BUSY) ;

	for (i = 0; cmd->mii_reg != miim_end; i++) {
		if (cmd->mii_data == miim_read) {
			result = read_phy_reg(priv, cmd->mii_reg);

			if (cmd->funct != NULL)
				(*(cmd->funct)) (result, priv);

		} else {
			if (cmd->funct != NULL)
				result = (*(cmd->funct)) (cmd->mii_reg, priv);
			else
				result = cmd->mii_data;

			write_phy_reg(priv, cmd->mii_reg, result);

		}
		cmd++;
	}
}

/* Relocate the function pointers in the phy cmd lists */
static void relocate_cmds(void)
{
	struct phy_cmd **cmdlistptr;
	struct phy_cmd *cmd;
	int i, j, k;

	for (i = 0; phy_info[i]; i++) {
		/* First thing's first: relocate the pointers to the
		 * PHY command structures (the structs were done) */
		phy_info[i] = (struct phy_info *)((uint) phy_info[i]
						  + gd->reloc_off);
		phy_info[i]->name += gd->reloc_off;
		phy_info[i]->config =
		    (struct phy_cmd *)((uint) phy_info[i]->config
				       + gd->reloc_off);
		phy_info[i]->startup =
		    (struct phy_cmd *)((uint) phy_info[i]->startup
				       + gd->reloc_off);
		phy_info[i]->shutdown =
		    (struct phy_cmd *)((uint) phy_info[i]->shutdown
				       + gd->reloc_off);

		cmdlistptr = &phy_info[i]->config;
		j = 0;
		for (; cmdlistptr <= &phy_info[i]->shutdown; cmdlistptr++) {
			k = 0;
			for (cmd = *cmdlistptr;
			     cmd->mii_reg != miim_end;
			     cmd++) {
				/* Only relocate non-NULL pointers */
				if (cmd->funct)
					cmd->funct += gd->reloc_off;

				k++;
			}
			j++;
		}
	}

	relocated = 1;
}

#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII) \
	&& !defined(BITBANGMII)

struct tsec_private *get_priv_for_phy(unsigned char phyaddr)
{
	int i;

	for (i = 0; i < MAXCONTROLLERS; i++) {
		if (privlist[i]->phyaddr == phyaddr)
			return privlist[i];
	}

	return NULL;
}

/*
 * Read a MII PHY register.
 *
 * Returns:
 *  0 on success
 */
static int tsec_miiphy_read(char *devname, unsigned char addr,
			    unsigned char reg, unsigned short *value)
{
	unsigned short ret;
	struct tsec_private *priv = get_priv_for_phy(addr);

	if (NULL == priv) {
		printf("Can't read PHY at address %d\n", addr);
		return -1;
	}

	ret = (unsigned short)read_phy_reg(priv, reg);
	*value = ret;

	return 0;
}

/*
 * Write a MII PHY register.
 *
 * Returns:
 *  0 on success
 */
static int tsec_miiphy_write(char *devname, unsigned char addr,
			     unsigned char reg, unsigned short value)
{
	struct tsec_private *priv = get_priv_for_phy(addr);

	if (NULL == priv) {
		printf("Can't write PHY at address %d\n", addr);
		return -1;
	}

	write_phy_reg(priv, reg, value);

	return 0;
}

#endif /* defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
		&& !defined(BITBANGMII) */
		
#if (CONFIG_COMMANDS & CFG_CMD_BSP) && (CONFIG_COMMANDS & CFG_CMD_MII)


void tsec_dev(void)
{

    printf("TSEC devices: ");
#if defined(CONFIG_MPC85XX_TSEC1)
    printf("%s  ",CONFIG_MPC85XX_TSEC1_NAME);
#endif
#if defined(CONFIG_MPC85XX_TSEC2)
    printf("%s  ",CONFIG_MPC85XX_TSEC2_NAME);
#endif
#if defined(CONFIG_MPC85XX_TSEC3)
    printf("%s  ",CONFIG_MPC85XX_TSEC3_NAME);
#endif
#if defined(CONFIG_MPC85XX_TSEC4)
    printf("%s  ",CONFIG_MPC85XX_TSEC4_NAME);
#endif
    printf("\n");
}

/*
 * Read a TBI MII register.
 *
 * Returns:
 *  0 on success
 */
int tsec_tbimii_read(char *devname, 
			    unsigned char reg, unsigned short *value)
{
       struct eth_device *dev;
	unsigned short ret;

       dev = eth_get_dev_by_name(devname);
	if (NULL == dev) {
		printf("Can't read %s TBI MII register %d\n", devname, reg);
		return -1;
	}

	ret = (unsigned short)read_phy_reg_tbi(dev->priv, reg);
	*value = ret;

	return 0;
}


/*
 * Write a TBI MII register.
 *
 * Returns:
 *  0 on success
 */
int tsec_tbimii_write(char *devname, 
			     unsigned char reg, unsigned short value)
{
       struct eth_device *dev;

       dev = eth_get_dev_by_name(devname);
	if (NULL == dev) {
		printf("Can't write %s TBI MII register %d\n", devname, reg);
		return -1;
	}

	write_phy_reg_tbi(dev->priv, reg, value);

	return 0;
}

#endif /* (CONFIG_COMMANDS & CFG_CMD_BSP)  && (CONFIG_COMMANDS & CFG_CMD_MII)*/

#endif /* CONFIG_TSEC_ENET */
