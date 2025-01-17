/***********************license start***************
 * Copyright (c) 2003-2008  Cavium Networks (support@cavium.com). All rights
 * reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of Cavium Networks nor the names of
 *       its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 *
 * This Software, including technical data, may be subject to U.S.  export
 * control laws, including the U.S.  Export Administration Act and its
 * associated regulations, and may be subject to export or import regulations
 * in other countries.  You warrant that You will comply strictly in all
 * respects with all such regulations and acknowledge that you have the
 * responsibility to obtain licenses to export, re-export or import the
 * Software.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND CAVIUM NETWORKS MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 * RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
 * REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
 * DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
 * PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET
 * POSSESSION OR CORRESPONDENCE TO DESCRIPTION.  THE ENTIRE RISK ARISING OUT
 * OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
 *
 *
 * For any questions regarding licensing please contact marketing@caviumnetworks.com
 *
 ***********************license end**************************************/






/**
 * @file
 *
 * Functions for RGMII/GMII/MII initialization, configuration,
 * and monitoring.
 *
 * <hr>$Revision: 1.2 $<hr>
 */
#include "executive-config.h"
#include "cvmx-config.h"
#ifdef CVMX_ENABLE_PKO_FUNCTIONS

#include "cvmx.h"
#include "cvmx-sysinfo.h"
#include "cvmx-mdio.h"
#include "cvmx-helper.h"
#include "cvmx-helper-board.h"

/**
 * @INTERNAL
 * Probe RGMII ports and determine the number present
 *
 * @param interface Interface to probe
 *
 * @return Number of RGMII/GMII/MII ports (0-4).
 */
int __cvmx_helper_rgmii_probe(int interface)
{
    int num_ports = 0;
    cvmx_gmxx_inf_mode_t mode;
    mode.u64 = cvmx_read_csr(CVMX_GMXX_INF_MODE(interface));

    if (mode.s.type)
    {
        if (OCTEON_IS_MODEL(OCTEON_CN38XX) || OCTEON_IS_MODEL(OCTEON_CN58XX))
        {
            cvmx_dprintf("ERROR: RGMII initialize called in SPI interface\n");
        }
        else if (OCTEON_IS_MODEL(OCTEON_CN31XX) || OCTEON_IS_MODEL(OCTEON_CN30XX) || OCTEON_IS_MODEL(OCTEON_CN50XX))
        {
            /* On these chips "type" says we're in GMII/MII mode. This
                limits us to 2 ports */
            num_ports = 2;
        }
        else
        {
            cvmx_dprintf("ERROR: Unsupported Octeon model in %s\n", __FUNCTION__);
        }
    }
    else
    {
        if (OCTEON_IS_MODEL(OCTEON_CN38XX) || OCTEON_IS_MODEL(OCTEON_CN58XX))
        {
            num_ports = 4;
        }
        else if (OCTEON_IS_MODEL(OCTEON_CN31XX) || OCTEON_IS_MODEL(OCTEON_CN30XX) || OCTEON_IS_MODEL(OCTEON_CN50XX))
        {
            num_ports = 3;
        }
        else
        {
            cvmx_dprintf("ERROR: Unsupported Octeon model in %s\n", __FUNCTION__);
        }
    }
    return num_ports;
}


/**
 * Put an RGMII interface in loopback mode. Internal packets sent
 * out will be received back again on the same port. Externally
 * received packets will echo back out.
 *
 * @param port   IPD port number to loop.
 */
void cvmx_helper_rgmii_internal_loopback(int port)
{
    int interface = (port >> 4) & 1;
    int index = port & 0xf;
    uint64_t tmp;

    cvmx_gmxx_prtx_cfg_t gmx_cfg;
    gmx_cfg.u64 = 0;
    gmx_cfg.s.duplex = 1;
    gmx_cfg.s.slottime = 1;
    gmx_cfg.s.speed = 1;
    cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 1);
    cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x200);
    cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0x2000);
    cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
    tmp = cvmx_read_csr(CVMX_ASXX_PRT_LOOP(interface));
    cvmx_write_csr(CVMX_ASXX_PRT_LOOP(interface), (1 << index) | tmp);
    tmp = cvmx_read_csr(CVMX_ASXX_TX_PRT_EN(interface));
    cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(interface), (1 << index) | tmp);
    tmp = cvmx_read_csr(CVMX_ASXX_RX_PRT_EN(interface));
    cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(interface), (1 << index) | tmp);
    gmx_cfg.s.en = 1;
    cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
}


/**
 * @INTERNAL
 * Configure all of the ASX, GMX, and PKO regsiters required
 * to get RGMII to function on the supplied interface.
 *
 * @param interface PKO Interface to configure (0 or 1)
 *
 * @return Zero on success
 */
int __cvmx_helper_rgmii_enable(int interface)
{
    int num_ports = cvmx_helper_ports_on_interface(interface);
    int port;
    cvmx_sysinfo_t *sys_info_ptr = cvmx_sysinfo_get();
    cvmx_gmxx_inf_mode_t mode;
    cvmx_asxx_tx_prt_en_t asx_tx;
    cvmx_asxx_rx_prt_en_t asx_rx;

    mode.u64 = cvmx_read_csr(CVMX_GMXX_INF_MODE(interface));

    if (mode.s.en == 0)
        return -1;
    if ((OCTEON_IS_MODEL(OCTEON_CN38XX) || OCTEON_IS_MODEL(OCTEON_CN58XX)) && mode.s.type == 1)   /* Ignore SPI interfaces */
        return -1;

    /* Configure the ASX registers needed to use the RGMII ports */
    asx_tx.u64 = 0;
    asx_tx.s.prt_en = cvmx_build_mask(num_ports);
    cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(interface), asx_tx.u64);

    asx_rx.u64 = 0;
    asx_rx.s.prt_en = cvmx_build_mask(num_ports);
    cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(interface), asx_rx.u64);

    /* Configure the GMX registers needed to use the RGMII ports */
    for (port=0; port<num_ports; port++)
    {
        cvmx_write_csr(CVMX_GMXX_TXX_THRESH(port, interface), 32);

        if (cvmx_octeon_is_pass1())
            __cvmx_helper_errata_asx_pass1(interface, port, sys_info_ptr->cpu_clock_hz);
        else
        {
            /* Configure more flexible RGMII preamble checking. Pass 1 doesn't
                support this feature. */
            cvmx_gmxx_rxx_frm_ctl_t frm_ctl;
            frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(port, interface));
            frm_ctl.s.pre_free = 1;  /* New field, so must be compile time */
            cvmx_write_csr(CVMX_GMXX_RXX_FRM_CTL(port, interface), frm_ctl.u64);
        }

        /* Each pause frame transmitted will ask for about 10M bit times
            before resume.  If buffer space comes available before that time
            has expired, an XON pause frame (0 time) will be transmitted to
            restart the flow. */
        cvmx_write_csr(CVMX_GMXX_TXX_PAUSE_PKT_TIME(port, interface), 20000);
        cvmx_write_csr(CVMX_GMXX_TXX_PAUSE_PKT_INTERVAL(port, interface), 19000);

        if (OCTEON_IS_MODEL(OCTEON_CN50XX))
        {
            /*
             * H/W puts the RGMII delay on JSRXNLE boards, so remove the default
             * S/W delay put in by Octeon
             */
#if CONFIG_JSRXNLE
            cvmx_write_csr(CVMX_ASXX_TX_CLK_SETX(port, interface), 0);
            cvmx_write_csr(CVMX_ASXX_RX_CLK_SETX(port, interface), 0);
#else
            cvmx_write_csr(CVMX_ASXX_TX_CLK_SETX(port, interface), 16);
            cvmx_write_csr(CVMX_ASXX_RX_CLK_SETX(port, interface), 16);
#endif
        }
        else
        {
            cvmx_write_csr(CVMX_ASXX_TX_CLK_SETX(port, interface), 24);
            cvmx_write_csr(CVMX_ASXX_RX_CLK_SETX(port, interface), 24);
        }
    }

    __cvmx_helper_setup_gmx(interface, num_ports);

    /* enable the ports now */
    for (port=0; port<num_ports; port++)
    {
        cvmx_gmxx_prtx_cfg_t gmx_cfg;
        cvmx_helper_link_autoconf(cvmx_helper_get_ipd_port(interface, port));
        gmx_cfg.u64 = cvmx_read_csr(CVMX_GMXX_PRTX_CFG(port, interface));
        gmx_cfg.s.en = 1;
        cvmx_write_csr(CVMX_GMXX_PRTX_CFG(port, interface), gmx_cfg.u64);
    }

    return 0;
}


/**
 * @INTERNAL
 * Return the link state of an IPD/PKO port as returned by
 * auto negotiation. The result of this function may not match
 * Octeon's link config if auto negotiation has changed since
 * the last call to cvmx_helper_link_set().
 *
 * @param ipd_port IPD/PKO port to query
 *
 * @return Link state
 */
cvmx_helper_link_info_t __cvmx_helper_rgmii_link_get(int ipd_port)
{
    return __cvmx_helper_board_link_get(ipd_port);
}


/**
 * @INTERNAL
 * Configure an IPD/PKO port for the specified link state. This
 * function does not influence auto negotiation at the PHY level.
 * The passed link state must always match the link state returned
 * by cvmx_helper_link_get(). It is normally best to use
 * cvmx_helper_link_autoconf() instead.
 *
 * @param ipd_port  IPD/PKO port to configure
 * @param link_info The new link state
 *
 * @return Zero on success, negative on failure
 */
int __cvmx_helper_rgmii_link_set(int ipd_port, cvmx_helper_link_info_t link_info)
{
    int interface = cvmx_helper_get_interface_num(ipd_port);
    int index = cvmx_helper_get_interface_index_num(ipd_port);
    uint64_t dbg;
    cvmx_gmxx_prtx_cfg_t gmx_cfg;
    cvmx_gmxx_prtx_cfg_t new_gmx_cfg;

    gmx_cfg.u64 = cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
    new_gmx_cfg = gmx_cfg;

    if (cvmx_octeon_is_pass1())
        new_gmx_cfg.s.duplex = 1;   /* Half duplex is broken for Pass 1 */
    else
        new_gmx_cfg.s.duplex = link_info.s.full_duplex;

    if (link_info.s.speed == 10)
    {
        new_gmx_cfg.s.slottime = 0;
        new_gmx_cfg.s.speed = 0;
    }
    else if (link_info.s.speed == 100)
    {
        new_gmx_cfg.s.slottime = 0;
        new_gmx_cfg.s.speed = 0;
    }
    else
    {
        new_gmx_cfg.s.slottime = 1;
        new_gmx_cfg.s.speed = 1;
    }

    /* Return if the new config matches the old one */
    if (new_gmx_cfg.u64 == gmx_cfg.u64)
        return 0;

    /* Disable the lowest level input from the port. Note that this also
        disables inband status */
    cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(interface),
                   cvmx_read_csr(CVMX_ASXX_RX_PRT_EN(interface)) & ~(1<<index));
    cvmx_read_csr(CVMX_ASXX_RX_PRT_EN(interface));

    /* Disable the port before we make any changes */
    gmx_cfg.s.en = 0;
    cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
    cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));

    /* Disable the TX path so it should be idle */
    // FIXME

    /* Poll the GMX state machine. If it isn't idle, we can't reconfigure
        yet. The current state is avaialble through the magic debug register */
    cvmx_write_csr(CVMX_NPI_DBG_SELECT, interface*0x800 + index*0x100 + 0x880);
    dbg = cvmx_read_csr(CVMX_DBG_DATA);
    if (((dbg & 0x7) != 0))
    {
        cvmx_dprintf("cvmx_helper_rgmii_link_set waiting for GMX state machine 1 to be idle\n");
        while ((dbg & 0x7) != 0)
        {
            cvmx_write_csr(CVMX_NPI_DBG_SELECT, interface*0x800 + index*0x100 + 0x880);
            dbg = cvmx_read_csr(CVMX_DBG_DATA);
            if (((dbg & 0x7) != 0))
                cvmx_wait(100);
        }
    }

    cvmx_write_csr(CVMX_NPI_DBG_SELECT, interface*0x800 + index*0x100 + 0x8c1);
    dbg = cvmx_read_csr(CVMX_DBG_DATA);
    if (((dbg & 0xf) != 0))
    {
        cvmx_dprintf("cvmx_helper_rgmii_link_set waiting for GMX state machine 2 to be idle\n");
        while ((dbg & 0xf) != 0)
        {
            cvmx_write_csr(CVMX_NPI_DBG_SELECT, interface*0x800 + index*0x100 + 0x8c1);
            dbg = cvmx_read_csr(CVMX_DBG_DATA);
            if (((dbg & 0xf) != 0))
                cvmx_wait(100);
        }
    }

    /* Do all the port setup while it is disabled and forced idle */
    if (link_info.s.speed == 10)
    {
        cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 50);
        cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x40);
        cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0);
    }
    else if (link_info.s.speed == 100)
    {
        cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 5);
        cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x40);
        cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0);
    }
    else
    {
        cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 1);
        cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x200);
        cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0x2000);
    }

    /* Save the new GMX setting without enabling the port */
    gmx_cfg.s.en = new_gmx_cfg.s.en;
    new_gmx_cfg.s.en = 0;
    cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), new_gmx_cfg.u64);
    cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));

    /* Re-enable the TX path so */
    // FIXME

    /* Enable the lowest level input from the port now that the speed is set
        properly. Note that this also enables inband status */
    cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(interface),
                   cvmx_read_csr(CVMX_ASXX_RX_PRT_EN(interface)) | (1<<index));
    cvmx_read_csr(CVMX_ASXX_RX_PRT_EN(interface));

    /* Restore teh GMX enable state. Port config is complete */
    new_gmx_cfg.s.en = gmx_cfg.s.en;
    cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), new_gmx_cfg.u64);

    return 0;
}

#endif /* CVMX_ENABLE_PKO_FUNCTIONS */

