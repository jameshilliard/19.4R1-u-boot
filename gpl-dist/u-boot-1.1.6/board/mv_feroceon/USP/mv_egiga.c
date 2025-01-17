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

#include <common.h>
#include <command.h>
#include <net.h>
#include <malloc.h>

#if defined (MV_INCLUDE_GIG_ETH)
#include "sys/mvSysGbe.h"
#include "mvOs.h"
#include "mvSysHwConfig.h"
#include "eth/mvEth.h"
#include "eth/gbe/mvEthGbe.h"
#include "eth-phy/mvEthPhy.h"
#include "ethswitch/mvSwitch.h"
#include "mvBoardEnvLib.h"

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/****************************************************** 
 * driver internal definitions --                     *
 ******************************************************/ 
/* use only tx-queue0 and rx-queue0 */
#define EGIGA_DEF_TXQ 0
#define EGIGA_DEF_RXQ 0

/* rx buffer size */ 
#define ETH_HLEN       14
#define WRAP           (2 + ETH_HLEN + 4 + 32)  /* 2(HW hdr) 14(MAC hdr) 4(CRC) 32(extra for cache prefetch)*/
#define MTU            1500
#define RX_BUFFER_SIZE (MTU + WRAP)

/* rings length */
#define EGIGA_TXQ_LEN   20
#define EGIGA_RXQ_LEN   20


typedef struct _egigaPriv
{
	int port;
	MV_VOID *halPriv;
	MV_U32 rxqCount;
	MV_U32 txqCount;
	MV_BOOL devInit;
} egigaPriv; 



/****************************************************** 
 * functions prototype --                             *
 ******************************************************/
static int mvEgigaLoad( int port, char *name, char *enet_addr );

static int mvEgigaInit( struct eth_device *dev, bd_t *p );
static int mvEgigaHalt( struct eth_device *dev );
static int mvEgigaTx( struct eth_device *dev, volatile MV_VOID *packet, int len );
static int mvEgigaRx( struct eth_device *dev );

static MV_PKT_INFO* mvEgigaRxFill(MV_VOID);

#if defined(CONFIG_EX2200) || \
    defined(CONFIG_EX3300) || \
    defined(CONFIG_RPS200)
/* diagnostics */
MV_8 diag_eth0 = 0;
MV_8 tx_err = 0;
MV_8 rx_err = 0;
char tx_err_msg[20];
char rx_err_msg[20];

/* There is bug in dynamic malloc/free buffers.  Using pre-allocated buffers */
static int gPkt_init = 0;
static MV_PKT_INFO* gpPktInfo[EGIGA_RXQ_LEN] = { 0 };
#endif

/*********************************************************** 
 * mv_eth_initialize --                                    *
 *   main driver initialization. loading the interfaces.   *
 ***********************************************************/
int mv_eth_initialize( bd_t *bis ) 
{

	int port;
	MV_8 *enet_addr;
	MV_8 name[NAMESIZE+1];
	MV_8 enetvar[9];
	MV_8 ethPortNum = mvCtrlEthMaxPortGet();
	MV_U32 ctrlId = mvCtrlModelGet();

	mvEthInit();

	/* load interface(s) */
	for( port = BOARD_ETH_START_PORT_NUM; port < ethPortNum; port++ )
	{
#if !defined(CONFIG_EX2200) && \
    !defined(CONFIG_RPS200) && \
    !defined(CONFIG_EX3300)
		if (MV_FALSE == mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, port)) continue;
#endif
			
#if defined(CONFIG_EX2200) || \
    defined(CONFIG_RPS200) || \
    defined(CONFIG_EX3300)
		/* one management port */
		if (port >= 1) continue;
#endif
		
		/* interface name */
		sprintf( name, "egiga%d", port );
		/* interface MAC addr extract */
		sprintf( enetvar, port ? "eth%daddr" : "ethaddr", port );
		enet_addr = getenv( enetvar );
	
		if ( (MV_78100_DEV_ID == ctrlId) || (MV_78200_DEV_ID == ctrlId) ||
             (MV_6281_DEV_ID == ctrlId) || (MV_6192_DEV_ID == ctrlId) ||
             (MV_6190_DEV_ID == ctrlId) || (MV_6180_DEV_ID == ctrlId))
		mvEthPortPowerUp(port);

#if defined(CONFIG_RPS200)
		mvEthPortPowerUp(port);
#else
        MV_REG_WRITE(ETH_TX_QUEUE_COMMAND1_REG(port), 0x8);
#endif
		mvEgigaLoad( port, name, enet_addr );
	}

#ifdef RD_MV78XX0_AMC
	/* Light on RDY led */
	mvEthPhyRegWrite(0, 0xb, 0x19, 0xc00);
#endif
	return 0;
}


/*********************************************************** 
 * mvEgigaLoad --                                          *
 *   load a network interface into uboot network core.     *
 *   initialize sw structures e.g. private, rings, etc.    *
 ***********************************************************/
static int mvEgigaLoad( int port, char *name, char *enet_addr ) 
{
	struct eth_device *dev = NULL;
	egigaPriv *priv = NULL;
	ETH_PORT_CTRL dummy_port_handle;

	DB( printf( "%s: %s load - ", __FUNCTION__, name ) );

	

	dev = malloc( sizeof(struct eth_device) );
	priv = malloc( sizeof(egigaPriv) );

	if( !dev ) {
		DB( printf( "%s: %s falied to alloc eth_device (error)\n", __FUNCTION__, name ) );
		goto error;
	}

	if( !priv ) {
		DB( printf( "%s: %s falied to alloc egiga_priv (error)\n", __FUNCTION__, name ) );
		goto error;
	}

	memset( priv, 0, sizeof(egigaPriv) );

	/* init device methods */
	memcpy( dev->name, name, NAMESIZE );
	mvMacStrToHex(enet_addr, (MV_8 *)(dev->enetaddr));

	/* set MAC addres even if port was not used yet. */
	dummy_port_handle.portNo = port;
	mvEthMacAddrSet( &dummy_port_handle, dev->enetaddr, EGIGA_DEF_RXQ);

	dev->init = (void *)mvEgigaInit;
	dev->halt = (void *)mvEgigaHalt;
	dev->send = (void *)mvEgigaTx;
	dev->recv = (void *)mvEgigaRx;
	dev->priv = priv;
	dev->iobase = 0;
	priv->port = port;

	/* register the interface */
	eth_register(dev);


	DB( printf( "%s: %s load ok\n", __FUNCTION__, name ) );
	return 0;

	error:
	printf( "%s: %s load failed\n", __FUNCTION__, name );
	if( priv ) free( dev->priv );
	if( dev ) free( dev );
	return -1;
}


static MV_PKT_INFO* mvEgigaRxFill(MV_VOID)
{
	MV_BUF_INFO *pBufInfo;
	MV_PKT_INFO *pPktInfo;
	MV_U8 *buf = (MV_U8 *)memalign( 32, RX_BUFFER_SIZE ); /* align on 32B */
	if( !buf ) {
		DB(printf("failed to alloc buffer\n"));
		return NULL;
	}

	if( ((MV_U32)buf) & 0xf ) 
        	printf( "un-align rx buffer %x\n", (MV_U32)buf );

	pPktInfo = malloc(sizeof(MV_PKT_INFO));
	if (pPktInfo == NULL) {
		printf("Error: cannot allocate memory for pktInfo\n");
		free(buf);
		return NULL;
	}

	pBufInfo = malloc(sizeof(MV_BUF_INFO));	
	if (pBufInfo == NULL) {
		printf("Error: cannot allocate memory for bufInfo\n");
		free(buf);
		free(pPktInfo);
		return NULL;
	}
        pBufInfo->bufPhysAddr = mvOsIoVirtToPhy(NULL, buf);
        pBufInfo->bufVirtPtr = buf;
        pBufInfo->bufSize = RX_BUFFER_SIZE;
        pBufInfo->dataSize = 0;
        pPktInfo->osInfo = (MV_ULONG)buf;
        pPktInfo->pFrags = pBufInfo;
        pPktInfo->pktSize = RX_BUFFER_SIZE; /* how much to invalidate */
        pPktInfo->numFrags = 1;
        pPktInfo->status = 0;
        pPktInfo->srcIdx = -1;
	return pPktInfo;
}

#if defined(CONFIG_EX2200) || \
    defined(CONFIG_RPS200) || \
    defined(CONFIG_EX3300)
static int mvEgigaRxClear(MV_PKT_INFO* pPktInfo)
{
	MV_BUF_INFO *pBufInfo;

	if (pPktInfo == NULL) {
		return MV_FAIL;
	}
       pPktInfo->numFrags = 1;
       pPktInfo->status = 0;
       pPktInfo->srcIdx = -1;

       pBufInfo = pPktInfo->pFrags;       
	if (pBufInfo == NULL) {
		return MV_FAIL;
	}
    
       pBufInfo->dataSize = 0;
	return MV_OK;
}
#endif

unsigned int egiga_init=0;

static int mvEgigaInit( struct eth_device *dev, bd_t *p )
{
	egigaPriv *priv = dev->priv;
	MV_ETH_PORT_INIT halInitStruct;
       	MV_PKT_INFO *pktInfo;
	MV_STATUS status;
	int i;
#if 0
	uint32_t mac43c, mac444, mac44c;
#endif
	DB( printf( "%s: %s init - ", __FUNCTION__, dev->name ) );

	/* egiga not ready */
	DB( printf ("mvBoardPhyAddrGet()=0x%x , priv->port =0x%x\n",mvBoardPhyAddrGet(priv->port),priv->port));

#if !defined(CONFIG_EX3300) &&  !defined(CONFIG_RPS200)
	/* If speed is not auto then link is force */
	if (BOARD_MAC_SPEED_AUTO == mvBoardMacSpeedGet(priv->port))
	{
		/* Check Link status on phy */
		if( mvEthPhyCheckLink( mvBoardPhyAddrGet(priv->port) ) == MV_FALSE ) {
			printf( "%s no link\n", dev->name );
			return 0;
		}
		else DB( printf( "link up\n" ) );
	}
#endif

	egiga_init = 1;

	/* init the hal -- create internal port control structure and descriptor rings, */
	/* open address decode windows, disable rx and tx operations. mask interrupts.  */
	halInitStruct.maxRxPktSize = RX_BUFFER_SIZE;
	halInitStruct.rxDefQ = EGIGA_DEF_RXQ;

	halInitStruct.txDescrNum[0] = EGIGA_TXQ_LEN;
	halInitStruct.rxDescrNum[0] = EGIGA_RXQ_LEN;
	halInitStruct.osHandle = NULL;

	priv->halPriv = mvEthPortInit( priv->port, &halInitStruct );

	if( !priv->halPriv ) {
		DB( printf( "falied to init eth port (error)\n" ) );
		goto error;
	}

    /* set new addr in hw */
    if( mvEthMacAddrSet( priv->halPriv, dev->enetaddr, EGIGA_DEF_RXQ) != MV_OK ) {
        printf("%s: ethSetMacAddr failed\n", dev->name );
	    goto error;
    }

	priv->devInit = MV_TRUE;

	/* fill rx ring with buffers */
	for( i=0; i<EGIGA_RXQ_LEN; i++ ) {

#if defined(CONFIG_EX2200) || \
    defined(CONFIG_RPS200) || \
    defined(CONFIG_EX3300)
		if (gPkt_init == 0) {
			gpPktInfo[i] = pktInfo = mvEgigaRxFill();
		}
		else {
			pktInfo = gpPktInfo[i];
			mvEgigaRxClear(pktInfo);
		}
#else
		pktInfo = mvEgigaRxFill();
#endif
		if (pktInfo == NULL)
			goto error;

		/* give the buffer to hal */
		status = mvEthPortRxDone( priv->halPriv, EGIGA_DEF_RXQ, pktInfo );
		if( status == MV_OK ) {
			priv->rxqCount++;
		}
		else if( status == MV_FULL ) {
			/* the ring is full */
			priv->rxqCount++;
			DB( printf( "ring full\n" ) );
			break;
		} 
		else {
			printf( "error\n" );
			goto error;
		}
	}

#if defined(CONFIG_EX2200) || \
    defined(CONFIG_RPS200) || \
    defined(CONFIG_EX3300)
	gPkt_init = 1;
#endif

#ifdef MV_DEBUG
	ethPortQueues(priv->port, EGIGA_DEF_RXQ, EGIGA_DEF_TXQ, 1);

	printf("%s : after calling ethPortQueues\n",__FUNCTION__);

#endif


	/* start the hal - rx/tx activity */
	/* Check if link is up for 2 Sec */
	for (i = 1; i < 100 ; i ++)
	{
		status = mvEthPortEnable( priv->halPriv );
		if (status == MV_OK)
			break;
		mvOsDelay(20);
	}
	
	if( status != MV_OK ) {
#if 0
		mac43c = MV_REG_READ(ETH_PORT_SERIAL_CTRL_REG(priv->port));
		mac444 = MV_REG_READ(ETH_PORT_STATUS_REG(priv->port));
		mac44c = MV_REG_READ(ETH_PORT_SERIAL_CTRL_1_REG(priv->port));
		printf("port = %d, MAC_43C = %08x, MAC_444 = %08x, MAC_44C = %08x\n", 
               priv->port, mac43c, mac444, mac44c);
#endif
		printf( "%s: %s mvEthPortEnable failed (error)\n", __FUNCTION__, dev->name );
		goto error;
	}

#ifdef MV_DEBUG
	ethRegs(priv->port);
	ethPortRegs(priv->port);
	ethPortStatus(priv->port);

	ethPortQueues(priv->port, EGIGA_DEF_RXQ, -1/*EGIGA_DEF_TXQ*/, 0);
#endif

	DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );
	return 1;

	error:
	if( priv->devInit )
		mvEgigaHalt( dev );
	printf( "%s: %s failed\n", __FUNCTION__, dev->name );
	return 0;
}

static int mvEgigaHalt( struct eth_device *dev )
{

	egigaPriv *priv = dev->priv;
	MV_PKT_INFO *pktInfo;

	DB( printf( "%s: %s halt - ", __FUNCTION__, dev->name ) );
	if( priv->devInit == MV_TRUE ) {

		/* stop the port activity, mask all interrupts */
		if( mvEthPortDisable( priv->halPriv ) != MV_OK ) 
			printf( "mvEthPortDisable failed (error)\n" );
    
#if !defined(CONFIG_EX2200)  && \
    !defined(CONFIG_RPS200) && \
    !defined(CONFIG_EX3300)
		/* free the buffs in the rx ring */
		while( (pktInfo = mvEthPortForceRx( priv->halPriv, EGIGA_DEF_RXQ )) != NULL ) {
			priv->rxqCount--;
			if( pktInfo->osInfo )
				free( (void *)pktInfo->osInfo );
			else 
				printf( "mvEthPortForceRx failed (error)\n" );
			if( pktInfo->pFrags )
				free( (void *)pktInfo->pFrags );
			else
 				printf( "mvEthPortForceRx failed (error)\n" );
            free( (void *)pktInfo );
		}
#endif

   		/* Clear Cause registers (must come before mvEthPortFinish) */
    	MV_REG_WRITE(ETH_INTR_CAUSE_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo),0);
    	MV_REG_WRITE(ETH_INTR_CAUSE_EXT_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo),0);

		mvEthPortFinish( priv->halPriv );
		priv->devInit = MV_FALSE;

	}
	egiga_init = 0;

	DB( printf( "%s: %s complete\n", __FUNCTION__, dev->name ) );
	return 0;
}

static int mvEgigaTx( struct eth_device *dev, volatile void *buf, int len )
{
	egigaPriv *priv = dev->priv;
	MV_BUF_INFO bufInfo;
    	MV_PKT_INFO pktInfo;
    	MV_PKT_INFO *pPktInfo;
	MV_STATUS status;
#ifdef MV_DEBUG
char * data = (char *)buf;
int i;
#endif

	DB( printf( "mvEgigaTx start\n" ) );
	/* if no link exist */
	if(!egiga_init) return 0;

#ifdef MV_DEBUG
for (i=0; i<len; i++) {
    if ((i % 16) == 0) printf("\n");

    printf("%02x ", data[i]);
}
printf("\n");
#endif

#if defined(CONFIG_EX2200) || \
    defined(CONFIG_EX3300) || \
    defined(CONFIG_RPS200)
	if (diag_eth0) {
	    tx_err = 0;
	    tx_err_msg[0] = 0;
	}
#endif

    pktInfo.osInfo = (MV_ULONG)0x44CAFE44;
    pktInfo.pktSize = len;
    pktInfo.pFrags = &bufInfo;
    pktInfo.status = 0;
    pktInfo.numFrags = 1;
    bufInfo.bufVirtPtr = (MV_U8*)buf;
    bufInfo.bufPhysAddr = mvOsIoVirtToPhy(NULL, buf);
    bufInfo.dataSize = len;

	/* send the packet */
	status = mvEthPortTx( priv->halPriv, EGIGA_DEF_TXQ, &pktInfo );

	if( status != MV_OK ) {
#if defined(CONFIG_EX2200) || \
    defined(CONFIG_EX3300) || \
    defined(CONFIG_RPS200)        
		if( status == MV_NO_RESOURCE ) {
			DB( printf( "ring is full (error)\n" ) );
			if (diag_eth0) {
			    tx_err = 0x1;
                         sprintf (tx_err_msg, "buffers full");
			}
		}
		else if( status == MV_ERROR ) {
			DB( printf( "error\n" ) );
			if (diag_eth0) {
			    tx_err = 0x2;
                         sprintf (tx_err_msg, "error");
			}
		}
		else {
			DB( printf( "unrecognize status (error) ethPortSend\n" ) );
			if (diag_eth0) {
			    tx_err = 0x4;
                         sprintf (tx_err_msg, "unknown error");
			}
		}
		goto error;
#else
		if( status == MV_NO_RESOURCE )
			DB( printf( "ring is full (error)\n" ) );
		else if( status == MV_ERROR )
			DB( printf( "error\n" ) );
		else
			DB( printf( "unrecognize status (error) ethPortSend\n" ) );
		goto error;
#endif
	} 
	else DB( printf( "ok\n" ) );

	priv->txqCount++;

	/* release the transmitted packet(s) */
	while( 1 ) {

		DB( printf( "%s: %s tx-done - ", __FUNCTION__, dev->name ) );

		/* get a packet */
		pPktInfo = mvEthPortTxDone( priv->halPriv, EGIGA_DEF_TXQ);

		if( pPktInfo != NULL ) {

			priv->txqCount--;

			/* validate skb */
			if( (pPktInfo != &pktInfo) || (pPktInfo->osInfo != 0x44CAFE44 ) ) {
				printf( "error\n" );
				goto error;
			}

			/* handle tx error */
			if( pPktInfo->status & (ETH_ERROR_SUMMARY_BIT) ) {
				printf( "bad status (error)\n" );
				goto error;
			}
			DB( printf( "ok\n" ) );
			break;
		}
		else 
			DB(printf( "NULL pPktInfo\n" ));
	}

	DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );
	return 0;

	error:
	printf( "%s: %s failed\n", __FUNCTION__, dev->name );
	return 1;
}


static int mvEgigaRx( struct eth_device *dev )
{
	egigaPriv*  priv = dev->priv;
    	MV_PKT_INFO *pktInfo;
	MV_STATUS   status;
#ifdef MV_DEBUG
char * data;
int i, len;
#endif

	/* if no link exist */
	if(!egiga_init) return 0;

#if defined(CONFIG_EX2200) || \
    defined(CONFIG_EX3300) || \
    defined(CONFIG_RPS200)

	if (diag_eth0) {
	    rx_err = 0;
	    rx_err_msg[0] = 0;
	}
#endif

	while( 1 ) {

		/* get rx packet from hal */
		pktInfo = mvEthPortRx( priv->halPriv, EGIGA_DEF_RXQ);

		if( pktInfo != NULL ) {
			DB( printf( "good rx\n" ) );
			priv->rxqCount--;

#ifdef MV_DEBUG
data = (char *)(((MV_U8 *)pktInfo->osInfo) + 2);
len = (int)pktInfo->pFrags->dataSize;
for (i=0; i<len; i++) {
    if ((i % 16) == 0) printf("\n");

    printf("%02x ", data[i]);
}
printf("\n");
#endif
			/* check rx error status */
//			if (( pktInfo->status & (ETH_ERROR_SUMMARY_MASK)) && 
//                          (pktInfo->status & ETH_RX_ERROR_CODE_MASK)){
			if( pktInfo->status & (ETH_ERROR_SUMMARY_MASK) ) {
				MV_U32 err = pktInfo->status & ETH_RX_ERROR_CODE_MASK;
				DB( printf( "bad rx status %08x, ", (MV_U32)pktInfo->status ) );
#if defined(CONFIG_EX2200) || \
    defined(CONFIG_EX3300) || \
    defined(CONFIG_RPS200)                
				if( err == ETH_RX_RESOURCE_ERROR ) {
					DB( printf( "(resource error)" ) );
	                            if (diag_eth0) {
					    tx_err = 0x1;
					    sprintf (tx_err_msg, "resource");
	                            }
				}
				else if( err == ETH_RX_MAX_FRAME_LEN_ERROR ) {
					DB( printf( "(max frame length error)" ) );
	                            if (diag_eth0) {
					    rx_err = 0x2;
					    sprintf (rx_err_msg, "length");
	                            }
				}
				else if( err == ETH_RX_OVERRUN_ERROR ) {
					DB( printf( "(overrun error)" ) );
	                            if (diag_eth0) {
					    rx_err = 0x4;
					    sprintf (rx_err_msg, "over run");
	                            }
				}
				else if( err == ETH_RX_CRC_ERROR ) {
					DB( printf( "(crc error)" ) );
	                            if (diag_eth0) {
					    rx_err = 0x8;
					    sprintf (rx_err_msg, "crc");
	                            }
				}
				else {
					DB( printf( "(unknown error)" ) );
	                            if (diag_eth0) {
					    rx_err = 0x10;
					    sprintf (rx_err_msg, "unknown");
	                            }
					goto error;
				}
				DB( printf( "\n" ) );
#else
				if( err == ETH_RX_RESOURCE_ERROR )
					DB( printf( "(resource error)" ) );
				else if( err == ETH_RX_MAX_FRAME_LEN_ERROR )
					DB( printf( "(max frame length error)" ) );
				else if( err == ETH_RX_OVERRUN_ERROR )
					DB( printf( "(overrun error)" ) );
				else if( err == ETH_RX_CRC_ERROR )
					DB( printf( "(crc error)" ) );
				else {
					DB( printf( "(unknown error)" ) );
					goto error;
				}
				DB( printf( "\n" ) );
#endif
			}
			else {

				DB( printf( "%s: %s calling NetRecieve ", __FUNCTION__, dev->name) );
				DB( printf( "%s: calling NetRecieve pkInfo = 0x%x\n", __FUNCTION__, pktInfo) );
				DB( printf( "%s: calling NetRecieve osInfo = 0x%x\n", __FUNCTION__, pktInfo->osInfo) );
				DB( printf( "%s: calling NetRecieve pktSize = 0x%x\n", __FUNCTION__, pktInfo->pFrags->dataSize) );
				/* good rx - push the packet up (skip on two first empty bytes) */
				ETH_PACKET_CACHE_INVALIDATE(pktInfo->osInfo, pktInfo->pFrags->dataSize);
				NetReceive( ((MV_U8 *)pktInfo->osInfo) + 2, (int)pktInfo->pFrags->dataSize);
			}


			DB( printf( "%s: %s refill rx buffer - ", __FUNCTION__, dev->name) );

			/* give the buffer back to hal (re-init the buffer address) */
	        	pktInfo->pktSize = RX_BUFFER_SIZE; /* how much to invalidate */
			status = mvEthPortRxDone( priv->halPriv, EGIGA_DEF_RXQ, pktInfo );
	
			if( status == MV_OK ) {
				priv->rxqCount++;
			}
			else if( status == MV_FULL ) {
				/* this buffer made the ring full */
				priv->rxqCount++;
				DB( printf( "ring full\n" ) );
				break;
			} 
			else {
				printf( "error\n" );
				goto error;
			}

		} else {
			/* no more rx packets ready */
			/* DB( printf( "no more work\n" ) ); */
			break;
		}
	}

	/*DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );*/
	return 0;

	error:
	DB( printf( "%s: %s failed\n", __FUNCTION__, dev->name ) );
	return 1;
}

/*******************************************************************************
* Init the PHY or Switch of the board 			   *
 ***********************************************************/
MV_VOID mvBoardEgigaPhySwitchInit(void) 
{
#if EX2200
	mvEthE3018PhyBasicInit(0);
	return;
#elif EX3300
	mvEthE1310PhyBasicInit(0);
	return;
#elif RPS 
	mvEthE1310PhyBasicInit(0);
	return;
#else
	if ((DB_88F6281A_BP_ID == mvBoardIdGet()) ||
		(DB_88F6192A_BP_ID == mvBoardIdGet()))
	{
		if ( !(mvBoardIsPortInGmii()))
		{
			mvEthE1116PhyBasicInit(0);
			mvEthE1116PhyBasicInit(1);
		}
	}
	else if ((DB_88F6190A_BP_ID == mvBoardIdGet()) ||
            (DB_88F6180A_BP_ID == mvBoardIdGet()) ||
            (RD_88F6192A_ID == mvBoardIdGet()) ||
            (RD_88F6190A_ID == mvBoardIdGet()) ||
             (RD_88F6281A_PCAC_ID == mvBoardIdGet()))
	{
		mvEthE1116PhyBasicInit(0);
	}
	else if (RD_88F6281A_ID == mvBoardIdGet())
	{
		mvEthE6161SwitchBasicInit(0);
        	mvEthE1116PhyBasicInit(1);
	}
	else if (RD_98DX3121_ID == mvBoardIdGet())
	{
		mvEthE1116PhyBasicInit(0);
	}
	else if (DB_98DX4122_ID == mvBoardIdGet())
	{
#if 0
		char linebuf[10];
		if (i2c_read(0/*channel*/, 0x18/*chip*/, 1/*addr*/, 1/*alen*/, linebuf, 2/*linebytes*/)!=0)
		{
			mvEthE1111PhyBasicInit(0);
		}

		/*printf("%x %x\n",linebuf[0],linebuf[1]);*/

		if((linebuf[0]) & 0x4 == 0x4)	/* check bit 2*/
			mvEthE1116PhyBasicInit(0); /* Rev 2 */
		else
			mvEthE1111PhyBasicInit(0); /* Rev 1 */
#else
	#if defined(DB_98DX_REV1)
		mvEthE1111PhyBasicInit(0);
	#elif defined(DB_98DX_REV2)
		mvEthE1116PhyBasicInit(0);
	#endif
#endif
	}
#endif
}

#endif /* #if defined (MV_INCLUDE_GIG_ETH) */
