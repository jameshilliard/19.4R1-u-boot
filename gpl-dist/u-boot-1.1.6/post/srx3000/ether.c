/*
 * Copyright (c) 2009-2010, Juniper Networks, Inc.
 * All rights reserved.
 *
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#include <common.h>

/*
 * Ethernet test
 *
 * The eTSEC controller  listed in ctlr_list array below
 * are tested in the internal and external loopback  mode.
 * The controllers are configured accordingly and several packets
 * are transmitted and recevied back both in internal and external loopback.
 * The configurable test parameters are:
 * MIN_PACKET_LENGTH - minimum size of packet to transmit
 * MAX_PACKET_LENGTH - maximum size of packet to transmit
 */
#ifdef  CONFIG_POST
#include "miiphy.h"
#include <config.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include "miiphy.h"
#include <post.h>
#include "ether.h"

#if CONFIG_POST & CFG_POST_ETHER
DECLARE_GLOBAL_DATA_PTR;

#define MIN_PACKET_LENGTH            64
#define MAX_PACKET_LENGTH            1518
#define POST_MAX_PACKET_LENGTH       256
#define LB_STRING_LEN                50
#define PKT_TEST_LOOP                5
#define TEST_64_BYTES                64
#define TSEC_MAC_OFFSET              500
#define CHKSM_LEN                    4 


/* capability */
#define LB_MAC                       0
#define LB_PHY                       1
#define MAX_LB_TYPES                 2
#define LB_EXT_10                    3

#define NUM_ETSEC_DEVICE             3
#define NUM_POST_ETSEC_DEVICE        1
#define MPC8548E_SECURITY_ENABLED    0x80390020
#define MPC8548E_SECURITY_DISABLED   0x80310020

struct tsec_device {
	char *name;
	u32 capability;
};


struct tsec_device etsec_dev[] =
                     {{CONFIG_MPC85XX_TSEC2_NAME, 1 << LB_MAC},
                     {CONFIG_MPC85XX_TSEC3_NAME, 1 << LB_MAC | 1 << LB_PHY},
                     {CONFIG_MPC85XX_TSEC4_NAME, 1 << LB_MAC | 1 << LB_PHY},
                     {NULL, 0}
                     };


struct tsec_device post_etsec_dev[] =
                     {{CONFIG_MPC85XX_TSEC3_NAME, 1 << LB_MAC | 1 << LB_PHY},
                     {CONFIG_MPC85XX_TSEC4_NAME, 1 << LB_MAC | 1 << LB_PHY},
                     {NULL , 0}
};

/* Ethernet Transmit and Receive Buffers */
#define TX_BUF_CNT 2
/* SGMII selection bits */
#define TSEC_SGMII (1 << 2)
#define MIIMCFG_SGMII_INIT_VALUE      0x00000005

struct tsec_info_struct {
	unsigned int phyaddr;
	u32 flags;
	unsigned int phyregidx;
};

static uint rxIdx;            /* index of the current RX buffer */
static uint txIdx;            /* index of the current TX buffer */
static int toggle_rx_errata = 0;
static int ether_debug_flag;
static int boot_flag_post;
int post_result_ether;
int lb_test = 0;

/*
 * TSEC Ethernet Tx and Rx buffer descriptors allocated at the
 * immr->udata_bd address on Dual-Port RAM
 * Provide for Double Buffering
 */
typedef volatile struct rtxbd {
	txbd8_t txbd[TX_BUF_CNT];
	rxbd8_t rxbd[PKTBUFSRX];
}  RTXBD;

#ifdef __GNUC__
static RTXBD rtx __attribute__ ((aligned(8)));
#else
#error "rtx must be 64-bit aligned"
#endif
char mode_test_string[MAX_LB_TYPES][LB_STRING_LEN] = 
                                        {{" Mac Loopback"},
                                         {" Phy Loopback"},
                                        };
static int ether_etsec_send(struct eth_device *dev,
                            volatile void *packet, int length);
static int ether_etsec_recv(struct eth_device *dev, void *packet, int len);
static int ether_etsec_init(struct eth_device *dev, int loop_back,
                           int debug_flag);
static void ether_etsec_halt(struct eth_device *dev);
static void startup_tsec(struct eth_device *dev, int loop_back, int debug_flag);
static void adjust_link(struct eth_device *dev, int debug_flag);
static void phy_loopback(struct tsec_private *priv, int loop_back, 
                         int debug_flag);
static void mac_reg_dump(struct eth_device *dev);
static void tsec_reg_dump(struct eth_device *dev);
extern void init_registers(volatile tsec_t *regs);
extern void write_phy_reg(struct tsec_private *priv, uint regnum, uint value);
extern uint read_phy_reg(struct tsec_private *priv, uint regnum);
extern void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd);
extern int init_phy(struct eth_device *dev);
extern  int ether_tsec_reinit(struct eth_device *dev);

typedef struct _reg_desc_t {
    ushort regno;
    char * name;
} reg_desc_t;

reg_desc_t mac_desc_tbl[] = {
    {  0x0,   "MACCFG1"      },
    {  0x1,   "MACCFG2"      },
    {  0x2,   "IPGIFG"       },
    {  0x3,   "HAFDUP"       },
    {  0x4,   "MAXFRM"       },
    {  0x8,   "MIIMCFG"      },
    {  0x9,   "MIIMCOM"      },
    {  0xA,   "MIIMADD"      },
    {  0xC,   "MIIMSTAT"     },
    {  0xD,   "MIIMIND"      },
    {  0xF,   "IFSTAT"       },
};

reg_desc_t tsec_desc_tbl[] = {
    {  0x0,   "TSEC_ID"      },
    {  0x1,   "TSEC_ID2"     },
    {  0x4,   "IEVENT"       },
    {  0x5,   "IMASK"        },
    {  0x6,   "EDIS"         },
    {  0x8,   "ECNTRL"       },
    {  0x9,   "PTV"          },
    {  0xA,   "DMACTRL"      },
    {  0xB,   "MIIMSTAT"     },
    {  0xC,   "TBIPA"        },
};

static void
toggle_tsec_rx (struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
	regs->maccfg1 &= ~MACCFG1_RX_EN;
	regs->maccfg1 |= MACCFG1_RX_EN;
}

/*
 * Initializes data structures and registers for the controller,
 * and brings the interface up.Returns the link status, meaning
 * that it returns success if the link is up, failure otherwise.
 * This allows u-boot to find the first active controller.
 */
int
ether_etsec_init (struct eth_device *dev, int loop_back, int debug_flag)
{
	uint tempval;
	char tmpbuf[MAC_ADDR_LEN];
	int i;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
	uint value;

	/* Init MACCFG2.  Defaults to GMII */
	regs->maccfg2 = MACCFG2_INIT_SETTINGS;

	/* Init ECNTRL */
	regs->ecntrl = ECNTRL_INIT_SETTINGS;

	if (loop_back == LB_MAC) {
		value = regs->maccfg1;
		value |= MACCFG1_LOOPBACK;
		regs->maccfg1 = value;
	}

	/*
	 * Copy the station address into the address registers.
	 * Backwards, because little endian MACS are dumb 
	 */
	for (i = 0; i < MAC_ADDR_LEN; i++) {
		tmpbuf[MAC_ADDR_LEN - 1 - i] = dev->enetaddr[i];
	};
	regs->macstnaddr1 = *((uint *) (tmpbuf));

	tempval = *((uint *) (tmpbuf + 4));

	regs->macstnaddr2 = tempval;

	/* reset the indices to zero */
	rxIdx = 0;
	txIdx = 0;

	/* Clear out (for the most part) the other registers */
	init_registers(regs);

	/* Ready the device for tx/rx */
	startup_tsec(dev, loop_back, debug_flag);

	/* If there's no link, fail */
	return priv->link;

}

int
ether_tsec_reinit (struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;

	/* Reset the MAC */
	priv->regs->maccfg1 |= MACCFG1_SOFT_RESET;
	priv->regs->maccfg1 &= ~(MACCFG1_SOFT_RESET);

	return init_phy(dev);
}

/* Configure maccfg2 based on negotiated speed and duplex
 * reported by PHY handling code
 */
static void
adjust_link (struct eth_device *dev, int debug_flag)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	if (priv->link) {
		if (priv->duplexity != 0) {
			regs->maccfg2 |= MACCFG2_FULL_DUPLEX;
		} else {
			regs->maccfg2 &= ~(MACCFG2_FULL_DUPLEX);
		}

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
                && (priv->flags & TSEC_REDUCED)) {
					regs->ecntrl |= ECNTRL_R100;
				} else {
					regs->ecntrl &= ~(ECNTRL_R100);
				}
				break;
			default:
				printf("%s: Speed was bad\n", dev->name);
				break;
		}
		if(ether_debug_flag) {
			printf("Speed: %d, %s duplex\n", priv->speed,
            (priv->duplexity) ? "full" : "half");
		}
		/* Wait for 10msec */
		udelay(10000);
	} else {
		if(debug_flag) {
			printf("%s: No link.\n", dev->name);
			/* Wait for 10msec */
			udelay(10000);
		}
	}
}


/*
 * Set up the buffers and their descriptors, and bring up the
 * interface
 */
static void
startup_tsec (struct eth_device *dev, int loop_back, int debug_flag)
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
	if(priv->phyinfo) {
		if(loop_back != LB_MAC) {
			phy_loopback(priv, loop_back, debug_flag);
			if (LB_EXT_10 == loop_back) {
				adjust_link(dev, debug_flag);
			}
		}
	}

	/* Enable Transmit and Receive */
	regs->maccfg1 |= (MACCFG1_RX_EN | MACCFG1_TX_EN);

	/* Tell the DMA it is clear to go */
	regs->dmactrl |= DMACTRL_INIT_SETTINGS;
	regs->tstat = TSTAT_CLEAR_THALT;
	regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);
}

/*
 * etsec transmit routine
 */
static int
ether_etsec_send (struct eth_device *dev, volatile void *packet, int length)
{
	int i;
	int result = 0;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	/* Wait for 1msec */
	udelay(1000);

	/* Find an empty buffer descriptor */
	for (i = 0; rtx.txbd[txIdx].status & TXBD_READY; i++) {
		if (i >= TOUT_LOOP) {
			post_log("%s: tsec: tx buffers full\n", dev->name);
			return result;
		}
	}

	rtx.txbd[txIdx].bufPtr = (uint) packet;
	rtx.txbd[txIdx].length = length;
	rtx.txbd[txIdx].status |=
		(TXBD_READY | TXBD_LAST | TXBD_CRC | TXBD_INTERRUPT);

	/* Tell the DMA to go */
	regs->tstat = TSTAT_CLEAR_THALT;

	/* Wait for buffer to be transmitted */
	for (i = 0; rtx.txbd[txIdx].status & TXBD_READY; i++) {
		if (i >= TOUT_LOOP) {
			post_log("%s: tsec: tx error\n", dev->name);
			return result;
		}
	}

	txIdx = (txIdx + 1) % TX_BUF_CNT;
	result = rtx.txbd[txIdx].status & TXBD_STATS;

	return result;
}

/*
 * etsec recv routine
 */
static int
ether_etsec_recv (struct eth_device *dev, void *recv_pack, int max_len)
{
	int length = 0;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	/* Wait for 10msec */
	udelay(10000);

	while(!(rtx.rxbd[rxIdx].status & RXBD_EMPTY)) {
		length = rtx.rxbd[rxIdx].length + length;

		/* Send the packet up if there were no errors */
		if (!(rtx.rxbd[rxIdx].status & RXBD_STATS)) {

			length = length - CHKSM_LEN;
			memcpy(recv_pack, (void*) NetRxPackets[rxIdx], length < max_len
					? length : max_len); 
		} else {
			if (toggle_rx_errata) {
				if (ether_debug_flag) {
				printf("For %s unrecoverable Rx error from self test loopback "
                        "mode\n", dev->name);
				}
			} else {
				post_log("Got error %x\n",
						(rtx.rxbd[rxIdx].status & RXBD_STATS));
			}
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

/* Stop the interface */
static void
ether_etsec_halt (struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);
	regs->dmactrl |= (DMACTRL_GRS | DMACTRL_GTS);

	while (!(regs->ievent & (IEVENT_GRSC | IEVENT_GTSC))) ;

	regs->maccfg1 &= ~(MACCFG1_TX_EN | MACCFG1_RX_EN | MACCFG1_LOOPBACK);

	/* Shut down the PHY, as needed */
	if(priv->phyinfo) {
		phy_run_commands(priv, priv->phyinfo->shutdown);
	}
}


/*
 * Execute the given series of commands on the given device's
 * PHY, running functions as necessary
 */
static void
phy_loopback (struct tsec_private *priv, int loop_back, int debug_flag)
{
	volatile uint value;
	volatile tsec_t *phyregs = priv->phyregs;

	phyregs->miimcfg = MIIMCFG_RESET;
	phyregs->miimcfg = MIIMCFG_INIT_VALUE;

	if (loop_back == LB_PHY) {
		/* Read the mii ctrl reg */
		value = read_phy_reg(priv, 0x00);
		/* Disable autonegotiation bit. */
		value &= ~(0x1000);
		/* Set Full Duplex Bit */
		value |= 0x0100;
		/* Clear the "power down" bit */
		value  &= ~(0x800);
		/* Set speed to 1Gbps */
		value |= 0x40;
		/* Set internal loopback mode */
		value |= 0x4000;

		/* write to the mii ctrl reg */
		write_phy_reg(priv, 0, value);
		/* After write wait for 30msec */
		udelay(30000);

		if (priv->flags & TSEC_SGMII) {
			phyregs->miimcfg = MIIMCFG_SGMII_INIT_VALUE;
		} else {
			phyregs->miimcfg = MIIMCFG_INIT_VALUE;
		}
		while (phyregs->miimind & MIIMIND_BUSY);
	}

	return;
}


/*
 * Test routines
 */
static void
packet_fill (char *packet, int length)
{
	char c = (char) length;
	int i;

	packet[0] = 0xFF;
	packet[1] = 0xFF;
	packet[2] = 0xFF;
	packet[3] = 0xFF;
	packet[4] = 0xFF;
	packet[5] = 0xFF;

	for (i = 6; i < length; i++) {
		packet[i] = c++;
	}
}

static int
packet_check (char *packet, int length)
{
	char c = (char) length;
	int i;
	for (i = 6; i < length; i++) {
		if (packet[i] != c++) {
			if(ether_debug_flag) {
				for( i = 0 ; i < length; i++) {
					post_log("%d ", packet[i]);
				}
			}
			return -1;
		}
	}
	return 0;
}

static void
mac_reg_dump (struct eth_device *dev)
{
	int i;
	volatile uint32_t *regs = NULL;

	if (!strcmp(dev->name, "eTSEC1")) {

		regs = (volatile uint32_t *)(TSEC_BASE_ADDR + 1 * TSEC_SIZE
                            + TSEC_MAC_OFFSET);

	} else if (!strcmp(dev->name, "eTSEC2")) {

		regs = (volatile uint32_t *)(TSEC_BASE_ADDR + 2 * TSEC_SIZE
                            + TSEC_MAC_OFFSET);

	} else if (!strcmp(dev->name, "eTSEC3")) {

		regs = (volatile uint32_t *)(TSEC_BASE_ADDR + 3 * TSEC_SIZE
                            + TSEC_MAC_OFFSET);
	} 
	if (regs != NULL) {
		printf("%s MAC register dump:\n",dev->name);
		for (i = 0; i < sizeof(mac_desc_tbl) / sizeof(mac_desc_tbl[0]); i++)
			printf("0x%02x %-10s %08x\n",
                mac_desc_tbl[i].regno, mac_desc_tbl[i].name,
                regs[mac_desc_tbl[i].regno]);
	}
}

static void
tsec_reg_dump (struct eth_device *dev)
{
	int i;
	volatile uint32_t *regs = NULL;

	if (!strcmp(dev->name, "eTSEC1")) {

		regs = (volatile uint32_t *)(TSEC_BASE_ADDR + 1 * TSEC_SIZE);

	} else if (!strcmp(dev->name, "eTSEC2")) {

		regs = (volatile uint32_t *)(TSEC_BASE_ADDR + 2 * TSEC_SIZE);

	} else if (!strcmp(dev->name, "eTSEC3")) {

		regs = (volatile uint32_t *)(TSEC_BASE_ADDR + 3 * TSEC_SIZE);
	} 
	if (regs != NULL) {
		printf("%s General control/status register dump:\n",dev->name);
		for (i = 0; i < sizeof(tsec_desc_tbl) / sizeof(tsec_desc_tbl[0]); i++) {
			printf("0x%02x %-10s %08x\n",
                tsec_desc_tbl[i].regno, tsec_desc_tbl[i].name,
                regs[tsec_desc_tbl[i].regno]);
		}
	}
}

/*
 *This routine checks for mac,phy loop back mode
 */
int
ether_loopback (struct eth_device *dev, int loopback, int max_pkt_len,
               int debug_flag, int reg_dump)
{
	int res = -1;
	char packet_send[MAX_PACKET_LENGTH];
	char packet_recv[MAX_PACKET_LENGTH];
	int length = 0;
	int i = 0;

	if (max_pkt_len > MAX_PACKET_LENGTH) {
		max_pkt_len = MAX_PACKET_LENGTH;
	}

	/* setup loopback */
	ether_etsec_init(dev, loopback, debug_flag);
	/* Wait for 20msec */
	udelay(20000);

	/*
	 * 8548 Rev 1.0 errata eTSEC 27: rx_en synchronization may cause unrecoverable
	 * RX error at startup.
	 * Workaround is to test one loopback packet. If failed, toggle rx_en
	 * bit.
	 */
	if (toggle_rx_errata) {
		for (i = 0; i < PKT_TEST_LOOP; i++) {
			packet_fill(packet_send, TEST_64_BYTES);
			ether_etsec_send(dev,packet_send, TEST_64_BYTES);
			length = ether_etsec_recv(dev, packet_recv, TEST_64_BYTES);
			if (length == 0) {
				toggle_tsec_rx(dev);
			} else {
				break;
			}
		}
	}

	for (i = MIN_PACKET_LENGTH; i <= max_pkt_len; i++) {
		toggle_rx_errata = 0;
		packet_fill(packet_send, i);
		ether_etsec_send(dev,packet_send, i);
		length = ether_etsec_recv(dev, packet_recv, i);
		if (length != i || packet_check(packet_recv, length) < 0) {
			if (!reg_dump) {
				if (lb_test == 1 && ether_debug_flag == 1) {
				tsec_reg_dump(dev);
				mac_reg_dump(dev);
				}
			}
			goto Done;
		}
	}
	res = 0;
Done: 
	toggle_rx_errata = 1;
	ether_etsec_halt(dev);
	if (loopback == LB_PHY) {
		/* Wait for 50msec */
		udelay(50000);
		ether_tsec_reinit(dev);
		/* Wait for 50msec */
		udelay(50000);
	}
	ether_etsec_halt(dev);
	return res;
}


/*
 * Test ctlr routine for etsec0 and etsec1
 */
static int
test_loopback (struct tsec_device* eth_dev)
{
	struct eth_device *dev;
	int ether_result[NUM_ETSEC_DEVICE][MAX_LB_TYPES];
	int i = 0;
	int j = 0;
	int fail = 0;

	memset(ether_result, 0, NUM_ETSEC_DEVICE * MAX_LB_TYPES * sizeof(int));

	for (i = 0; eth_dev[i].name; i++) {
		dev = eth_get_dev_by_name(eth_dev[i].name);
		for (j = 0; j < MAX_LB_TYPES; j++) {
			if (eth_dev[i].capability & (1 << j)) {
				for (lb_test = 0; lb_test < 2; lb_test++) {
					ether_result[i][j] = ether_loopback(dev, j,
							POST_MAX_PACKET_LENGTH,
							ether_debug_flag, 0);
				}
				if ((ether_result[i][j] == -1)) {
					fail = 1;
					printf(" Fail\n");
				}
			}
		}
	}


	if (boot_flag_post) {
		if (!fail) {
			return 0;
		} else {
			post_result_ether = -1;
			return -1;
		}
	}

	/* tsec diag result printing */
	if (!fail) {
		post_log("\n\n-------------  The tsec diag test passed  -------\n\n");
		if (ether_debug_flag) {
			for (i = 0; eth_dev[i].name; i++) {
				for (j = 0; j < MAX_LB_TYPES; j++) {
					if (eth_dev[i].capability & (1 << j))
						post_log(" %s :   %s  Passed\n",eth_dev[i].name, mode_test_string[j]);
				}
			}
		}
	} else {
		post_log("\n\n-------------  The tsec diag test failed  -------\n\n");
		if (ether_debug_flag) {
			for (i = 0; eth_dev[i].name; i++) {
				for (j = 0; j < MAX_LB_TYPES; j++) {
					if (eth_dev[i].capability & (1 << j)) {
						if (ether_result[i][j] == -1) {
							post_log(" %s :   %s  Failed\n",eth_dev[i].name, mode_test_string[j]);
						} else {
							if (ether_debug_flag) {
								post_log(" %s :   %s  Passed\n",eth_dev[i].name, mode_test_string[j]);
							}
						}
					}
				}
			}
		}
	}

	if (!fail) {
		return 0;
	}
	return -1;
}

int
ether_post_test (int flags)
{
	int res = 0;
	uint svr;
	boot_flag_post = 0;
	ether_debug_flag =0;
	post_result_ether = 0;
	lb_test = 0;

	if (flags & POST_DEBUG) {
		ether_debug_flag = 1;
	}
	if (flags & BOOT_POST_FLAG) {
		boot_flag_post = 1;
	}

	/*
	 * 8548 Rev 1.0 errata eTSEC 27: rx_en synchronization may cause unrecoverable
	 * RX error at startup. Workaround is to test one loopback packet.
	 * If failed, toggle rx_en bit. This problem will be fix in Rev 1.1.
	 */
	svr = get_svr();

	if (svr == MPC8548E_SECURITY_ENABLED || svr == MPC8548E_SECURITY_DISABLED) {
		toggle_rx_errata = 1;
	}

	if (!boot_flag_post) {
		post_log("This tests the functionality of eTSEC in MAC and PHY"
             " wait for 5 to 10 seconds\n");
		res = test_loopback(etsec_dev);
	} else {
		res = test_loopback(post_etsec_dev);
	}

	return res;
}

#endif /* CONFIG_POST & CFG_POST_ETHER */
#endif /* CONFIG_POST */
