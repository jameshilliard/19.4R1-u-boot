/***********************license start***************
 * Copyright (c) 2003-2010  Cavium Networks (support@cavium.com). All rights
 * reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.

 *   * Neither the name of Cavium Networks nor the names of
 *     its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written
 *     permission.

 * This Software, including technical data, may be subject to U.S. export  control
 * laws, including the U.S. Export Administration Act and its  associated
 * regulations, and may be subject to export or import  regulations in other
 * countries.

 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND CAVIUM  NETWORKS MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY REPRESENTATION OR
 * DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT DEFECTS, AND CAVIUM
 * SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES OF TITLE,
 * MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF
 * VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK ARISING OUT OF USE OR
 * PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
 ***********************license end**************************************/


/**
 * cvmx-dpi-defs.h
 *
 * Configuration and status register (CSR) type definitions for
 * Octeon dpi.
 *
 * This file is auto generated. Do not edit.
 *
 * <hr>$Revision$<hr>
 *
 */
#ifndef __CVMX_DPI_TYPEDEFS_H__
#define __CVMX_DPI_TYPEDEFS_H__

#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_BIST_STATUS CVMX_DPI_BIST_STATUS_FUNC()
static inline uint64_t CVMX_DPI_BIST_STATUS_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX) || OCTEON_IS_MODEL(OCTEON_CN71XX)))
		cvmx_warn("CVMX_DPI_BIST_STATUS not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000000ull);
}
#else
#define CVMX_DPI_BIST_STATUS (CVMX_ADD_IO_SEG(0x0001DF0000000000ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_CTL CVMX_DPI_CTL_FUNC()
static inline uint64_t CVMX_DPI_CTL_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_CTL not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000040ull);
}
#else
#define CVMX_DPI_CTL (CVMX_ADD_IO_SEG(0x0001DF0000000040ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_DMAX_COUNTS(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 7)))))
		cvmx_warn("CVMX_DPI_DMAX_COUNTS(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000300ull) + ((offset) & 7) * 8;
}
#else
#define CVMX_DPI_DMAX_COUNTS(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000300ull) + ((offset) & 7) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_DMAX_DBELL(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 7)))))
		cvmx_warn("CVMX_DPI_DMAX_DBELL(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000200ull) + ((offset) & 7) * 8;
}
#else
#define CVMX_DPI_DMAX_DBELL(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000200ull) + ((offset) & 7) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_DMAX_IBUFF_SADDR(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 7)))))
		cvmx_warn("CVMX_DPI_DMAX_IBUFF_SADDR(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000280ull) + ((offset) & 7) * 8;
}
#else
#define CVMX_DPI_DMAX_IBUFF_SADDR(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000280ull) + ((offset) & 7) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_DMAX_NADDR(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 7)))))
		cvmx_warn("CVMX_DPI_DMAX_NADDR(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000380ull) + ((offset) & 7) * 8;
}
#else
#define CVMX_DPI_DMAX_NADDR(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000380ull) + ((offset) & 7) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_DMAX_REQBNK0(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 7)))))
		cvmx_warn("CVMX_DPI_DMAX_REQBNK0(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000400ull) + ((offset) & 7) * 8;
}
#else
#define CVMX_DPI_DMAX_REQBNK0(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000400ull) + ((offset) & 7) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_DMAX_REQBNK1(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 7)))))
		cvmx_warn("CVMX_DPI_DMAX_REQBNK1(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000480ull) + ((offset) & 7) * 8;
}
#else
#define CVMX_DPI_DMAX_REQBNK1(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000480ull) + ((offset) & 7) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_DMA_CONTROL CVMX_DPI_DMA_CONTROL_FUNC()
static inline uint64_t CVMX_DPI_DMA_CONTROL_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_DMA_CONTROL not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000048ull);
}
#else
#define CVMX_DPI_DMA_CONTROL (CVMX_ADD_IO_SEG(0x0001DF0000000048ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_DMA_ENGX_EN(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 5)))))
		cvmx_warn("CVMX_DPI_DMA_ENGX_EN(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000080ull) + ((offset) & 7) * 8;
}
#else
#define CVMX_DPI_DMA_ENGX_EN(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000080ull) + ((offset) & 7) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_ENGX_BUF(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 5)))))
		cvmx_warn("CVMX_DPI_ENGX_BUF(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000880ull) + ((offset) & 7) * 8;
}
#else
#define CVMX_DPI_ENGX_BUF(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000880ull) + ((offset) & 7) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_INFO_REG CVMX_DPI_INFO_REG_FUNC()
static inline uint64_t CVMX_DPI_INFO_REG_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_INFO_REG not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000980ull);
}
#else
#define CVMX_DPI_INFO_REG (CVMX_ADD_IO_SEG(0x0001DF0000000980ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_INT_EN CVMX_DPI_INT_EN_FUNC()
static inline uint64_t CVMX_DPI_INT_EN_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_INT_EN not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000010ull);
}
#else
#define CVMX_DPI_INT_EN (CVMX_ADD_IO_SEG(0x0001DF0000000010ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_INT_REG CVMX_DPI_INT_REG_FUNC()
static inline uint64_t CVMX_DPI_INT_REG_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_INT_REG not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000008ull);
}
#else
#define CVMX_DPI_INT_REG (CVMX_ADD_IO_SEG(0x0001DF0000000008ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_PINT_INFO CVMX_DPI_PINT_INFO_FUNC()
static inline uint64_t CVMX_DPI_PINT_INFO_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_PINT_INFO not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000830ull);
}
#else
#define CVMX_DPI_PINT_INFO (CVMX_ADD_IO_SEG(0x0001DF0000000830ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_PKT_ERR_RSP CVMX_DPI_PKT_ERR_RSP_FUNC()
static inline uint64_t CVMX_DPI_PKT_ERR_RSP_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_PKT_ERR_RSP not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000078ull);
}
#else
#define CVMX_DPI_PKT_ERR_RSP (CVMX_ADD_IO_SEG(0x0001DF0000000078ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_REQ_ERR_RSP CVMX_DPI_REQ_ERR_RSP_FUNC()
static inline uint64_t CVMX_DPI_REQ_ERR_RSP_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_REQ_ERR_RSP not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000058ull);
}
#else
#define CVMX_DPI_REQ_ERR_RSP (CVMX_ADD_IO_SEG(0x0001DF0000000058ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_REQ_ERR_RSP_EN CVMX_DPI_REQ_ERR_RSP_EN_FUNC()
static inline uint64_t CVMX_DPI_REQ_ERR_RSP_EN_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_REQ_ERR_RSP_EN not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000068ull);
}
#else
#define CVMX_DPI_REQ_ERR_RSP_EN (CVMX_ADD_IO_SEG(0x0001DF0000000068ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_REQ_ERR_RST CVMX_DPI_REQ_ERR_RST_FUNC()
static inline uint64_t CVMX_DPI_REQ_ERR_RST_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_REQ_ERR_RST not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000060ull);
}
#else
#define CVMX_DPI_REQ_ERR_RST (CVMX_ADD_IO_SEG(0x0001DF0000000060ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_REQ_ERR_RST_EN CVMX_DPI_REQ_ERR_RST_EN_FUNC()
static inline uint64_t CVMX_DPI_REQ_ERR_RST_EN_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_REQ_ERR_RST_EN not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000070ull);
}
#else
#define CVMX_DPI_REQ_ERR_RST_EN (CVMX_ADD_IO_SEG(0x0001DF0000000070ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
#define CVMX_DPI_REQ_GBL_EN CVMX_DPI_REQ_GBL_EN_FUNC()
static inline uint64_t CVMX_DPI_REQ_GBL_EN_FUNC(void)
{
	if (!(OCTEON_IS_MODEL(OCTEON_CN63XX)))
		cvmx_warn("CVMX_DPI_REQ_GBL_EN not supported on this chip\n");
	return CVMX_ADD_IO_SEG(0x0001DF0000000050ull);
}
#else
#define CVMX_DPI_REQ_GBL_EN (CVMX_ADD_IO_SEG(0x0001DF0000000050ull))
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_SLI_PRTX_CFG(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 1)))))
		cvmx_warn("CVMX_DPI_SLI_PRTX_CFG(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000900ull) + ((offset) & 1) * 8;
}
#else
#define CVMX_DPI_SLI_PRTX_CFG(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000900ull) + ((offset) & 1) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_SLI_PRTX_ERR(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 1)))))
		cvmx_warn("CVMX_DPI_SLI_PRTX_ERR(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000920ull) + ((offset) & 1) * 8;
}
#else
#define CVMX_DPI_SLI_PRTX_ERR(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000920ull) + ((offset) & 1) * 8)
#endif
#if CVMX_ENABLE_CSR_ADDRESS_CHECKING
static inline uint64_t CVMX_DPI_SLI_PRTX_ERR_INFO(unsigned long offset)
{
	if (!(
	      (OCTEON_IS_MODEL(OCTEON_CN63XX) && ((offset <= 1)))))
		cvmx_warn("CVMX_DPI_SLI_PRTX_ERR_INFO(%lu) is invalid on this chip\n", offset);
	return CVMX_ADD_IO_SEG(0x0001DF0000000940ull) + ((offset) & 1) * 8;
}
#else
#define CVMX_DPI_SLI_PRTX_ERR_INFO(offset) (CVMX_ADD_IO_SEG(0x0001DF0000000940ull) + ((offset) & 1) * 8)
#endif

/**
 * cvmx_dpi_bist_status
 */
union cvmx_dpi_bist_status
{
	uint64_t u64;
	struct cvmx_dpi_bist_status_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_37_63               : 27;
	uint64_t bist                         : 37; /**< BIST Results.
                                                         HW sets a bit in BIST for for memory that fails
                                                         BIST. */
#else
	uint64_t bist                         : 37;
	uint64_t reserved_37_63               : 27;
#endif
	} s;
	struct cvmx_dpi_bist_status_s         cn63xx;
	struct cvmx_dpi_bist_status_s         cn63xxp1;
};
typedef union cvmx_dpi_bist_status cvmx_dpi_bist_status_t;

/**
 * cvmx_dpi_ctl
 */
union cvmx_dpi_ctl
{
	uint64_t u64;
	struct cvmx_dpi_ctl_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_2_63                : 62;
	uint64_t clk                          : 1;  /**< Status bit that indicates that the clks are running */
	uint64_t en                           : 1;  /**< Turns on the DMA and Packet state machines */
#else
	uint64_t en                           : 1;
	uint64_t clk                          : 1;
	uint64_t reserved_2_63                : 62;
#endif
	} s;
	struct cvmx_dpi_ctl_s                 cn63xx;
	struct cvmx_dpi_ctl_s                 cn63xxp1;
};
typedef union cvmx_dpi_ctl cvmx_dpi_ctl_t;

/**
 * cvmx_dpi_dma#_counts
 *
 * DPI_DMA[0..7]_COUNTS = DMA Instruction Counts
 *
 * Values for determing the number of instructions for DMA[0..7] in the DPI.
 */
union cvmx_dpi_dmax_counts
{
	uint64_t u64;
	struct cvmx_dpi_dmax_counts_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_39_63               : 25;
	uint64_t fcnt                         : 7;  /**< Number of words in the Instruction FIFO locally
                                                         cached within DPI. */
	uint64_t dbell                        : 32; /**< Number of available words of Instructions to read. */
#else
	uint64_t dbell                        : 32;
	uint64_t fcnt                         : 7;
	uint64_t reserved_39_63               : 25;
#endif
	} s;
	struct cvmx_dpi_dmax_counts_s         cn63xx;
	struct cvmx_dpi_dmax_counts_s         cn63xxp1;
};
typedef union cvmx_dpi_dmax_counts cvmx_dpi_dmax_counts_t;

/**
 * cvmx_dpi_dma#_dbell
 *
 * DPI_DMA_DBELL[0..7] = DMA Door Bell
 *
 * The door bell register for DMA[0..7] queue.
 */
union cvmx_dpi_dmax_dbell
{
	uint64_t u64;
	struct cvmx_dpi_dmax_dbell_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_16_63               : 48;
	uint64_t dbell                        : 16; /**< The value written to this register is added to the
                                                         number of 8byte words to be read and processes for
                                                         the low priority dma queue. */
#else
	uint64_t dbell                        : 16;
	uint64_t reserved_16_63               : 48;
#endif
	} s;
	struct cvmx_dpi_dmax_dbell_s          cn63xx;
	struct cvmx_dpi_dmax_dbell_s          cn63xxp1;
};
typedef union cvmx_dpi_dmax_dbell cvmx_dpi_dmax_dbell_t;

/**
 * cvmx_dpi_dma#_ibuff_saddr
 *
 * DPI_DMA[0..7]_IBUFF_SADDR = DMA Instruction Buffer Starting Address
 *
 * The address to start reading Instructions from for DMA[0..7].
 */
union cvmx_dpi_dmax_ibuff_saddr
{
	uint64_t u64;
	struct cvmx_dpi_dmax_ibuff_saddr_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_62_63               : 2;
	uint64_t csize                        : 14; /**< The size in 8B words of the DMA Instruction Chunk.
                                                         This value should only be written at known times
                                                         in order to prevent corruption of the instruction
                                                         queue.  The minimum CSIZE is 16 (one cacheblock). */
	uint64_t reserved_41_47               : 7;
	uint64_t idle                         : 1;  /**< DMA Request Queue is IDLE */
	uint64_t reserved_36_39               : 4;
	uint64_t saddr                        : 29; /**< The 128 byte aligned starting or chunk address.
                                                         SADDR is address bit 35:7 of the starting
                                                         instructions address. When new chunks are fetched
                                                         by the HW, SADDR will be updated to reflect the
                                                         address of the current chunk.
                                                         A write to SADDR resets both the queue's doorbell
                                                         (DPI_DMAx_COUNTS[DBELL) and its tail pointer
                                                         (DPI_DMAx_NADDR[ADDR]). */
	uint64_t reserved_0_6                 : 7;
#else
	uint64_t reserved_0_6                 : 7;
	uint64_t saddr                        : 29;
	uint64_t reserved_36_39               : 4;
	uint64_t idle                         : 1;
	uint64_t reserved_41_47               : 7;
	uint64_t csize                        : 14;
	uint64_t reserved_62_63               : 2;
#endif
	} s;
	struct cvmx_dpi_dmax_ibuff_saddr_s    cn63xx;
	struct cvmx_dpi_dmax_ibuff_saddr_s    cn63xxp1;
};
typedef union cvmx_dpi_dmax_ibuff_saddr cvmx_dpi_dmax_ibuff_saddr_t;

/**
 * cvmx_dpi_dma#_naddr
 *
 * DPI_DMA[0..7]_NADDR = DMA Next Ichunk Address
 *
 * Place DPI will read the next Ichunk data from.
 */
union cvmx_dpi_dmax_naddr
{
	uint64_t u64;
	struct cvmx_dpi_dmax_naddr_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_36_63               : 28;
	uint64_t addr                         : 36; /**< The next L2C address to read DMA# instructions
                                                         from. */
#else
	uint64_t addr                         : 36;
	uint64_t reserved_36_63               : 28;
#endif
	} s;
	struct cvmx_dpi_dmax_naddr_s          cn63xx;
	struct cvmx_dpi_dmax_naddr_s          cn63xxp1;
};
typedef union cvmx_dpi_dmax_naddr cvmx_dpi_dmax_naddr_t;

/**
 * cvmx_dpi_dma#_reqbnk0
 *
 * DPI_DMA[0..7]_REQBNK0 = DMA Request State Bank0
 *
 * Current contents of the request state machine - bank0
 */
union cvmx_dpi_dmax_reqbnk0
{
	uint64_t u64;
	struct cvmx_dpi_dmax_reqbnk0_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t state                        : 64; /**< State */
#else
	uint64_t state                        : 64;
#endif
	} s;
	struct cvmx_dpi_dmax_reqbnk0_s        cn63xx;
	struct cvmx_dpi_dmax_reqbnk0_s        cn63xxp1;
};
typedef union cvmx_dpi_dmax_reqbnk0 cvmx_dpi_dmax_reqbnk0_t;

/**
 * cvmx_dpi_dma#_reqbnk1
 *
 * DPI_DMA[0..7]_REQBNK1 = DMA Request State Bank1
 *
 * Current contents of the request state machine - bank1
 */
union cvmx_dpi_dmax_reqbnk1
{
	uint64_t u64;
	struct cvmx_dpi_dmax_reqbnk1_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t state                        : 64; /**< State */
#else
	uint64_t state                        : 64;
#endif
	} s;
	struct cvmx_dpi_dmax_reqbnk1_s        cn63xx;
	struct cvmx_dpi_dmax_reqbnk1_s        cn63xxp1;
};
typedef union cvmx_dpi_dmax_reqbnk1 cvmx_dpi_dmax_reqbnk1_t;

/**
 * cvmx_dpi_dma_control
 *
 * DPI_DMA_CONTROL = DMA Control Register
 *
 * Controls operation of the DMA IN/OUT.
 */
union cvmx_dpi_dma_control
{
	uint64_t u64;
	struct cvmx_dpi_dma_control_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_61_63               : 3;
	uint64_t pkt_en1                      : 1;  /**< Enables the 2nd packet interface.
                                                         When the packet interface is enabled, engine 4
                                                         is used for packets and is not available for DMA.
                                                         The packet interfaces must be enabled in order.
                                                         When PKT_EN1=1, then PKT_EN=1.
                                                         When PKT_EN1=1, then DMA_ENB<4>=0. */
	uint64_t ffp_dis                      : 1;  /**< Force forward progress disable
                                                         The DMA engines will compete for shared resources.
                                                         If the HW detects that particular engines are not
                                                         able to make requests to an interface, the HW
                                                         will periodically trade-off throughput for
                                                         fairness. */
	uint64_t commit_mode                  : 1;  /**< DMA Engine Commit Mode

                                                         When COMMIT_MODE=0, DPI considers an instruction
                                                         complete when the HW internally generates the
                                                         final write for the current instruction.

                                                         When COMMIT_MODE=1, DPI additionally waits for
                                                         the final write to reach the interface coherency
                                                         point to declare the instructions complete.

                                                         Please note: when COMMIT_MODE == 0, DPI may not
                                                         follow the HRM ordering rules.

                                                         DPI hardware performance may be better with
                                                         COMMIT_MODE == 0 than with COMMIT_MODE == 1 due
                                                         to the relaxed ordering rules.

                                                         If the HRM ordering rules are required, set
                                                         COMMIT_MODE == 1. */
	uint64_t pkt_hp                       : 1;  /**< High-Priority Mode for Packet Interface.
                                                         Engine 5 will be serviced more frequently to
                                                         deliver more bandwidth to packet interface.
                                                         When PKT_EN=0, then PKT_HP=0. */
	uint64_t pkt_en                       : 1;  /**< Enables the packet interface.
                                                         When the packet interface is enabled, engine 5
                                                         is used for packets and is not available for DMA.
                                                         When PKT_EN=1, then DMA_ENB<5>=0.
                                                         When PKT_EN=0, then PKT_HP=0. */
	uint64_t reserved_54_55               : 2;
	uint64_t dma_enb                      : 6;  /**< DMA engine enable. Enables the operation of the
                                                         DMA engine. After being enabled an engine should
                                                         not be disabled while processing instructions.
                                                         When PKT_EN=1, then DMA_ENB<5>=0. */
	uint64_t reserved_34_47               : 14;
	uint64_t b0_lend                      : 1;  /**< When set '1' and the DPI is in the mode to write
                                                         0 to L2C memory when a DMA is done, the address
                                                         to be written to will be treated as a Little
                                                         Endian address. */
	uint64_t dwb_denb                     : 1;  /**< When set '1', DPI will send a value in the DWB
                                                         field for a free page operation for the memory
                                                         that contained the data. */
	uint64_t dwb_ichk                     : 9;  /**< When Instruction Chunks for DMA operations are
                                                         freed this value is used for the DWB field of the
                                                         operation. */
	uint64_t fpa_que                      : 3;  /**< The FPA queue that the instruction-chunk page will
                                                         be returned to when used. */
	uint64_t o_add1                       : 1;  /**< When set '1' 1 will be added to the DMA counters,
                                                         if '0' then the number of bytes in the dma
                                                         transfer will be added to the count register. */
	uint64_t o_ro                         : 1;  /**< Relaxed Ordering Mode for DMA. */
	uint64_t o_ns                         : 1;  /**< Nosnoop For DMA. */
	uint64_t o_es                         : 2;  /**< Endian Swap Mode for DMA. */
	uint64_t o_mode                       : 1;  /**< Select PCI_POINTER MODE to be used.
                                                         0=DPTR format 1 is used
                                                           use register values for address and pointer
                                                           values for ES, NS, RO
                                                         1=DPTR format 0 is used
                                                           use pointer values for address and register
                                                           values for ES, NS, RO */
	uint64_t reserved_0_13                : 14;
#else
	uint64_t reserved_0_13                : 14;
	uint64_t o_mode                       : 1;
	uint64_t o_es                         : 2;
	uint64_t o_ns                         : 1;
	uint64_t o_ro                         : 1;
	uint64_t o_add1                       : 1;
	uint64_t fpa_que                      : 3;
	uint64_t dwb_ichk                     : 9;
	uint64_t dwb_denb                     : 1;
	uint64_t b0_lend                      : 1;
	uint64_t reserved_34_47               : 14;
	uint64_t dma_enb                      : 6;
	uint64_t reserved_54_55               : 2;
	uint64_t pkt_en                       : 1;
	uint64_t pkt_hp                       : 1;
	uint64_t commit_mode                  : 1;
	uint64_t ffp_dis                      : 1;
	uint64_t pkt_en1                      : 1;
	uint64_t reserved_61_63               : 3;
#endif
	} s;
	struct cvmx_dpi_dma_control_s         cn63xx;
	struct cvmx_dpi_dma_control_cn63xxp1
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_59_63               : 5;
	uint64_t commit_mode                  : 1;  /**< DMA Engine Commit Mode

                                                         When COMMIT_MODE=0, DPI considers an instruction
                                                         complete when the HW internally generates the
                                                         final write for the current instruction.

                                                         When COMMIT_MODE=1, DPI additionally waits for
                                                         the final write to reach the interface coherency
                                                         point to declare the instructions complete.

                                                         Please note: when COMMIT_MODE == 0, DPI may not
                                                         follow the HRM ordering rules.

                                                         DPI hardware performance may be better with
                                                         COMMIT_MODE == 0 than with COMMIT_MODE == 1 due
                                                         to the relaxed ordering rules.

                                                         If the HRM ordering rules are required, set
                                                         COMMIT_MODE == 1. */
	uint64_t pkt_hp                       : 1;  /**< High-Priority Mode for Packet Interface.
                                                         Engine 5 will be serviced more frequently to
                                                         deliver more bandwidth to packet interface.
                                                         When PKT_EN=0, then PKT_HP=0. */
	uint64_t pkt_en                       : 1;  /**< Enables the packet interface.
                                                         When the packet interface is enabled, engine 5
                                                         is used for packets and is not available for DMA.
                                                         When PKT_EN=1, then DMA_ENB<5>=0.
                                                         When PKT_EN=0, then PKT_HP=0. */
	uint64_t reserved_54_55               : 2;
	uint64_t dma_enb                      : 6;  /**< DMA engine enable. Enables the operation of the
                                                         DMA engine. After being enabled an engine should
                                                         not be disabled while processing instructions.
                                                         When PKT_EN=1, then DMA_ENB<5>=0. */
	uint64_t reserved_34_47               : 14;
	uint64_t b0_lend                      : 1;  /**< When set '1' and the DPI is in the mode to write
                                                         0 to L2C memory when a DMA is done, the address
                                                         to be written to will be treated as a Little
                                                         Endian address. */
	uint64_t dwb_denb                     : 1;  /**< When set '1', DPI will send a value in the DWB
                                                         field for a free page operation for the memory
                                                         that contained the data. */
	uint64_t dwb_ichk                     : 9;  /**< When Instruction Chunks for DMA operations are
                                                         freed this value is used for the DWB field of the
                                                         operation. */
	uint64_t fpa_que                      : 3;  /**< The FPA queue that the instruction-chunk page will
                                                         be returned to when used. */
	uint64_t o_add1                       : 1;  /**< When set '1' 1 will be added to the DMA counters,
                                                         if '0' then the number of bytes in the dma
                                                         transfer will be added to the count register. */
	uint64_t o_ro                         : 1;  /**< Relaxed Ordering Mode for DMA. */
	uint64_t o_ns                         : 1;  /**< Nosnoop For DMA. */
	uint64_t o_es                         : 2;  /**< Endian Swap Mode for DMA. */
	uint64_t o_mode                       : 1;  /**< Select PCI_POINTER MODE to be used.
                                                         0=DPTR format 1 is used
                                                           use register values for address and pointer
                                                           values for ES, NS, RO
                                                         1=DPTR format 0 is used
                                                           use pointer values for address and register
                                                           values for ES, NS, RO */
	uint64_t reserved_0_13                : 14;
#else
	uint64_t reserved_0_13                : 14;
	uint64_t o_mode                       : 1;
	uint64_t o_es                         : 2;
	uint64_t o_ns                         : 1;
	uint64_t o_ro                         : 1;
	uint64_t o_add1                       : 1;
	uint64_t fpa_que                      : 3;
	uint64_t dwb_ichk                     : 9;
	uint64_t dwb_denb                     : 1;
	uint64_t b0_lend                      : 1;
	uint64_t reserved_34_47               : 14;
	uint64_t dma_enb                      : 6;
	uint64_t reserved_54_55               : 2;
	uint64_t pkt_en                       : 1;
	uint64_t pkt_hp                       : 1;
	uint64_t commit_mode                  : 1;
	uint64_t reserved_59_63               : 5;
#endif
	} cn63xxp1;
};
typedef union cvmx_dpi_dma_control cvmx_dpi_dma_control_t;

/**
 * cvmx_dpi_dma_eng#_en
 */
union cvmx_dpi_dma_engx_en
{
	uint64_t u64;
	struct cvmx_dpi_dma_engx_en_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_8_63                : 56;
	uint64_t qen                          : 8;  /**< Controls which logical instruction queues can be
                                                         serviced by the DMA engine. Setting QEN==0
                                                         effectively disables the engine.
                                                         When DPI_DMA_CONTROL[PKT_EN] = 1, then
                                                         DPI_DMA_ENG5_EN[QEN] must be zero. */
#else
	uint64_t qen                          : 8;
	uint64_t reserved_8_63                : 56;
#endif
	} s;
	struct cvmx_dpi_dma_engx_en_s         cn63xx;
	struct cvmx_dpi_dma_engx_en_s         cn63xxp1;
};
typedef union cvmx_dpi_dma_engx_en cvmx_dpi_dma_engx_en_t;

/**
 * cvmx_dpi_eng#_buf
 *
 * Notes:
 * The total amount of storage allocated to the 6 DPI DMA engines (via DPI_ENG*_BUF[BLKS]) must not exceed 8KB.
 *
 */
union cvmx_dpi_engx_buf
{
	uint64_t u64;
	struct cvmx_dpi_engx_buf_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_8_63                : 56;
	uint64_t base                         : 4;  /**< The base address in 512B blocks of the engine fifo */
	uint64_t blks                         : 4;  /**< The size in 512B blocks of the engine fifo
                                                         Legal values are 0-8.
                                                         0 = Engine is disabled
                                                         1 = 0.5KB buffer
                                                         2 = 1.0KB buffer
                                                         3 = 1.5KB buffer
                                                         4 = 2.0KB buffer
                                                         5 = 2.5KB buffer
                                                         6 = 3.0KB buffer
                                                         7 = 3.5KB buffer
                                                         8 = 4.0KB buffer */
#else
	uint64_t blks                         : 4;
	uint64_t base                         : 4;
	uint64_t reserved_8_63                : 56;
#endif
	} s;
	struct cvmx_dpi_engx_buf_s            cn63xx;
	struct cvmx_dpi_engx_buf_s            cn63xxp1;
};
typedef union cvmx_dpi_engx_buf cvmx_dpi_engx_buf_t;

/**
 * cvmx_dpi_info_reg
 */
union cvmx_dpi_info_reg
{
	uint64_t u64;
	struct cvmx_dpi_info_reg_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_8_63                : 56;
	uint64_t ffp                          : 4;  /**< Force Forward Progress Indicator */
	uint64_t reserved_2_3                 : 2;
	uint64_t ncb                          : 1;  /**< NCB Register Access
                                                         This interrupt will fire in normal operation
                                                         when SW reads a DPI register through the NCB
                                                         interface. */
	uint64_t rsl                          : 1;  /**< RSL Register Access
                                                         This interrupt will fire in normal operation
                                                         when SW reads a DPI register through the RSL
                                                         interface. */
#else
	uint64_t rsl                          : 1;
	uint64_t ncb                          : 1;
	uint64_t reserved_2_3                 : 2;
	uint64_t ffp                          : 4;
	uint64_t reserved_8_63                : 56;
#endif
	} s;
	struct cvmx_dpi_info_reg_s            cn63xx;
	struct cvmx_dpi_info_reg_cn63xxp1
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_2_63                : 62;
	uint64_t ncb                          : 1;  /**< NCB Register Access
                                                         This interrupt will fire in normal operation
                                                         when SW reads a DPI register through the NCB
                                                         interface. */
	uint64_t rsl                          : 1;  /**< RSL Register Access
                                                         This interrupt will fire in normal operation
                                                         when SW reads a DPI register through the RSL
                                                         interface. */
#else
	uint64_t rsl                          : 1;
	uint64_t ncb                          : 1;
	uint64_t reserved_2_63                : 62;
#endif
	} cn63xxp1;
};
typedef union cvmx_dpi_info_reg cvmx_dpi_info_reg_t;

/**
 * cvmx_dpi_int_en
 */
union cvmx_dpi_int_en
{
	uint64_t u64;
	struct cvmx_dpi_int_en_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_26_63               : 38;
	uint64_t sprt1_rst                    : 1;  /**< DMA instruction was dropped because the source or
                                                          destination port was in reset.
                                                         this bit is set. */
	uint64_t sprt0_rst                    : 1;  /**< DMA instruction was dropped because the source or
                                                          destination port was in reset.
                                                         this bit is set. */
	uint64_t reserved_23_23               : 1;
	uint64_t req_badfil                   : 1;  /**< DMA instruction unexpected fill */
	uint64_t req_inull                    : 1;  /**< DMA instruction filled with NULL pointer */
	uint64_t req_anull                    : 1;  /**< DMA instruction filled with bad instruction */
	uint64_t req_undflw                   : 1;  /**< DMA instruction FIFO underflow */
	uint64_t req_ovrflw                   : 1;  /**< DMA instruction FIFO overflow */
	uint64_t req_badlen                   : 1;  /**< DMA instruction fetch with length */
	uint64_t req_badadr                   : 1;  /**< DMA instruction fetch with bad pointer */
	uint64_t dmadbo                       : 8;  /**< DMAx doorbell overflow. */
	uint64_t reserved_2_7                 : 6;
	uint64_t nfovr                        : 1;  /**< CSR Fifo Overflow */
	uint64_t nderr                        : 1;  /**< NCB Decode Error */
#else
	uint64_t nderr                        : 1;
	uint64_t nfovr                        : 1;
	uint64_t reserved_2_7                 : 6;
	uint64_t dmadbo                       : 8;
	uint64_t req_badadr                   : 1;
	uint64_t req_badlen                   : 1;
	uint64_t req_ovrflw                   : 1;
	uint64_t req_undflw                   : 1;
	uint64_t req_anull                    : 1;
	uint64_t req_inull                    : 1;
	uint64_t req_badfil                   : 1;
	uint64_t reserved_23_23               : 1;
	uint64_t sprt0_rst                    : 1;
	uint64_t sprt1_rst                    : 1;
	uint64_t reserved_26_63               : 38;
#endif
	} s;
	struct cvmx_dpi_int_en_s              cn63xx;
	struct cvmx_dpi_int_en_s              cn63xxp1;
};
typedef union cvmx_dpi_int_en cvmx_dpi_int_en_t;

/**
 * cvmx_dpi_int_reg
 */
union cvmx_dpi_int_reg
{
	uint64_t u64;
	struct cvmx_dpi_int_reg_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_26_63               : 38;
	uint64_t sprt1_rst                    : 1;  /**< DMA instruction was dropped because the source or
                                                          destination port was in reset.
                                                         this bit is set. */
	uint64_t sprt0_rst                    : 1;  /**< DMA instruction was dropped because the source or
                                                          destination port was in reset.
                                                         this bit is set. */
	uint64_t reserved_23_23               : 1;
	uint64_t req_badfil                   : 1;  /**< DMA instruction unexpected fill
                                                         Instruction fill when none outstanding. */
	uint64_t req_inull                    : 1;  /**< DMA instruction filled with NULL pointer
                                                         Next pointer was NULL. */
	uint64_t req_anull                    : 1;  /**< DMA instruction filled with bad instruction
                                                         Fetched instruction word was 0. */
	uint64_t req_undflw                   : 1;  /**< DMA instruction FIFO underflow
                                                         DPI tracks outstanding instructions fetches.
                                                         Interrupt will fire when FIFO underflows. */
	uint64_t req_ovrflw                   : 1;  /**< DMA instruction FIFO overflow
                                                         DPI tracks outstanding instructions fetches.
                                                         Interrupt will fire when FIFO overflows. */
	uint64_t req_badlen                   : 1;  /**< DMA instruction fetch with length
                                                         Interrupt will fire if DPI forms an instruction
                                                         fetch with length of zero. */
	uint64_t req_badadr                   : 1;  /**< DMA instruction fetch with bad pointer
                                                         Interrupt will fire if DPI forms an instruction
                                                         fetch to the NULL pointer. */
	uint64_t dmadbo                       : 8;  /**< DMAx doorbell overflow.
                                                         DPI has a 32-bit counter for each request's queue
                                                         outstanding doorbell counts. Interrupt will fire
                                                         if the count overflows. */
	uint64_t reserved_2_7                 : 6;
	uint64_t nfovr                        : 1;  /**< CSR Fifo Overflow
                                                         DPI can store upto 16 CSR request.  The FIFO will
                                                         overflow if that number is exceeded. */
	uint64_t nderr                        : 1;  /**< NCB Decode Error
                                                         DPI received a NCB transaction on the outbound
                                                         bus to the DPI deviceID, but the command was not
                                                         recognized. */
#else
	uint64_t nderr                        : 1;
	uint64_t nfovr                        : 1;
	uint64_t reserved_2_7                 : 6;
	uint64_t dmadbo                       : 8;
	uint64_t req_badadr                   : 1;
	uint64_t req_badlen                   : 1;
	uint64_t req_ovrflw                   : 1;
	uint64_t req_undflw                   : 1;
	uint64_t req_anull                    : 1;
	uint64_t req_inull                    : 1;
	uint64_t req_badfil                   : 1;
	uint64_t reserved_23_23               : 1;
	uint64_t sprt0_rst                    : 1;
	uint64_t sprt1_rst                    : 1;
	uint64_t reserved_26_63               : 38;
#endif
	} s;
	struct cvmx_dpi_int_reg_s             cn63xx;
	struct cvmx_dpi_int_reg_s             cn63xxp1;
};
typedef union cvmx_dpi_int_reg cvmx_dpi_int_reg_t;

/**
 * cvmx_dpi_pint_info
 *
 * DPI_PINT_INFO = DPI Packet Interrupt Info
 *
 * DPI Packet Interrupt Info.
 */
union cvmx_dpi_pint_info
{
	uint64_t u64;
	struct cvmx_dpi_pint_info_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_14_63               : 50;
	uint64_t iinfo                        : 6;  /**< Packet Instruction Doorbell count overflow info */
	uint64_t reserved_6_7                 : 2;
	uint64_t sinfo                        : 6;  /**< Packet Scatterlist Doorbell count overflow info */
#else
	uint64_t sinfo                        : 6;
	uint64_t reserved_6_7                 : 2;
	uint64_t iinfo                        : 6;
	uint64_t reserved_14_63               : 50;
#endif
	} s;
	struct cvmx_dpi_pint_info_s           cn63xx;
	struct cvmx_dpi_pint_info_s           cn63xxp1;
};
typedef union cvmx_dpi_pint_info cvmx_dpi_pint_info_t;

/**
 * cvmx_dpi_pkt_err_rsp
 */
union cvmx_dpi_pkt_err_rsp
{
	uint64_t u64;
	struct cvmx_dpi_pkt_err_rsp_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_1_63                : 63;
	uint64_t pkterr                       : 1;  /**< Indicates that an ErrorResponse was received from
                                                         the I/O subsystem. */
#else
	uint64_t pkterr                       : 1;
	uint64_t reserved_1_63                : 63;
#endif
	} s;
	struct cvmx_dpi_pkt_err_rsp_s         cn63xx;
	struct cvmx_dpi_pkt_err_rsp_s         cn63xxp1;
};
typedef union cvmx_dpi_pkt_err_rsp cvmx_dpi_pkt_err_rsp_t;

/**
 * cvmx_dpi_req_err_rsp
 */
union cvmx_dpi_req_err_rsp
{
	uint64_t u64;
	struct cvmx_dpi_req_err_rsp_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_8_63                : 56;
	uint64_t qerr                         : 8;  /**< Indicates which instruction queue received an
                                                         ErrorResponse from the I/O subsystem.
                                                         SW must clear the bit before the the cooresponding
                                                         instruction queue will continue processing
                                                         instructions if DPI_REQ_ERR_RSP_EN[EN] is set. */
#else
	uint64_t qerr                         : 8;
	uint64_t reserved_8_63                : 56;
#endif
	} s;
	struct cvmx_dpi_req_err_rsp_s         cn63xx;
	struct cvmx_dpi_req_err_rsp_s         cn63xxp1;
};
typedef union cvmx_dpi_req_err_rsp cvmx_dpi_req_err_rsp_t;

/**
 * cvmx_dpi_req_err_rsp_en
 */
union cvmx_dpi_req_err_rsp_en
{
	uint64_t u64;
	struct cvmx_dpi_req_err_rsp_en_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_8_63                : 56;
	uint64_t en                           : 8;  /**< Indicates which instruction queues should stop
                                                         dispatching instructions when an  ErrorResponse
                                                         is received from the I/O subsystem. */
#else
	uint64_t en                           : 8;
	uint64_t reserved_8_63                : 56;
#endif
	} s;
	struct cvmx_dpi_req_err_rsp_en_s      cn63xx;
	struct cvmx_dpi_req_err_rsp_en_s      cn63xxp1;
};
typedef union cvmx_dpi_req_err_rsp_en cvmx_dpi_req_err_rsp_en_t;

/**
 * cvmx_dpi_req_err_rst
 */
union cvmx_dpi_req_err_rst
{
	uint64_t u64;
	struct cvmx_dpi_req_err_rst_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_8_63                : 56;
	uint64_t qerr                         : 8;  /**< Indicates which instruction queue dropped an
                                                         instruction because the source or destination
                                                         was in reset.
                                                         SW must clear the bit before the the cooresponding
                                                         instruction queue will continue processing
                                                         instructions if DPI_REQ_ERR_RST_EN[EN] is set. */
#else
	uint64_t qerr                         : 8;
	uint64_t reserved_8_63                : 56;
#endif
	} s;
	struct cvmx_dpi_req_err_rst_s         cn63xx;
	struct cvmx_dpi_req_err_rst_s         cn63xxp1;
};
typedef union cvmx_dpi_req_err_rst cvmx_dpi_req_err_rst_t;

/**
 * cvmx_dpi_req_err_rst_en
 */
union cvmx_dpi_req_err_rst_en
{
	uint64_t u64;
	struct cvmx_dpi_req_err_rst_en_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_8_63                : 56;
	uint64_t en                           : 8;  /**< Indicates which instruction queues should stop
                                                         dispatching instructions when an instruction
                                                         is dropped because the source or destination port
                                                         is in reset. */
#else
	uint64_t en                           : 8;
	uint64_t reserved_8_63                : 56;
#endif
	} s;
	struct cvmx_dpi_req_err_rst_en_s      cn63xx;
	struct cvmx_dpi_req_err_rst_en_s      cn63xxp1;
};
typedef union cvmx_dpi_req_err_rst_en cvmx_dpi_req_err_rst_en_t;

/**
 * cvmx_dpi_req_gbl_en
 */
union cvmx_dpi_req_gbl_en
{
	uint64_t u64;
	struct cvmx_dpi_req_gbl_en_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_8_63                : 56;
	uint64_t qen                          : 8;  /**< Indicates which instruction queues are enabled and
                                                         can dispatch instructions to a requesting engine. */
#else
	uint64_t qen                          : 8;
	uint64_t reserved_8_63                : 56;
#endif
	} s;
	struct cvmx_dpi_req_gbl_en_s          cn63xx;
	struct cvmx_dpi_req_gbl_en_s          cn63xxp1;
};
typedef union cvmx_dpi_req_gbl_en cvmx_dpi_req_gbl_en_t;

/**
 * cvmx_dpi_sli_prt#_cfg
 *
 * DPI_SLI_PRTx_CFG = DPI SLI Port Configuration
 *
 * Configures the Max Read Request Size, Max Paylod Size, and Max Number of SLI Tags in use
 */
union cvmx_dpi_sli_prtx_cfg
{
	uint64_t u64;
	struct cvmx_dpi_sli_prtx_cfg_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_25_63               : 39;
	uint64_t halt                         : 1;  /**< When set, HALT indicates that the MAC has detected
                                                         a reset condition. No further instructions that
                                                         reference the MAC from any instruction Q will be
                                                         issued until the MAC comes out of reset and HALT
                                                         is cleared in SLI_CTL_PORTx[DIS_PORT]. */
	uint64_t reserved_21_23               : 3;
	uint64_t qlm_cfg                      : 1;  /**< Read only copy of the QLM CFG pin
                                                         0= MAC is PCIe
                                                         1= MAC is SRIO */
	uint64_t reserved_17_19               : 3;
	uint64_t rd_mode                      : 1;  /**< Read Mode
                                                         0=Exact Read Mode
                                                           If the port is a PCIe port, the HW reads on a
                                                           4B granularity.  In this mode, the HW may break
                                                           a given read into 3 operations to satisify
                                                           PCIe rules.
                                                           If the port is a SRIO port, the HW follows the
                                                           SRIO read rules from the SRIO specification and
                                                            only issues 32*n, 16, and 8 byte  operations
                                                            on the SRIO bus.
                                                         1=Block Mode
                                                           The HW will read more data than requested in
                                                           order to minimize the number of operations
                                                           necessary to complete the operation.
                                                           The memory region must be memory like. */
	uint64_t reserved_14_15               : 2;
	uint64_t molr                         : 6;  /**< Max Outstanding Load Requests
                                                         Limits the number of oustanding load requests on
                                                         the port by restricting the number of tags
                                                         used by the SLI to track load responses.  This
                                                         value can range from 1 to 32. Setting a value of
                                                         0 will halt all read traffic to the port.  There
                                                         are no restrictions on when this value
                                                         can be changed. */
	uint64_t mps_lim                      : 1;  /**< MAC memory space write requests cannot cross the
                                                         (naturally-aligned) MPS boundary.
                                                         When clear, DPI is allowed to issue a MAC memory
                                                         space read that crosses the naturally-aligned
                                                         boundary of size defined by MPS. (DPI will still
                                                         only cross the boundary when it would eliminate a
                                                         write by doing so.)
                                                         When set, DPI will never issue a MAC memory space
                                                         write that crosses the naturally-aligned boundary
                                                         of size defined by MPS. */
	uint64_t reserved_5_6                 : 2;
	uint64_t mps                          : 1;  /**< Max Payload Size
                                                                 0 = 128B
                                                                 1 = 256B
                                                         For PCIe MACs, this MPS size must not exceed
                                                               the size selected by PCIE*_CFG030[MPS].
                                                         For sRIO MACs, all MPS values are allowed. */
	uint64_t mrrs_lim                     : 1;  /**< MAC memory space read requests cannot cross the
                                                         (naturally-aligned) MRRS boundary.
                                                         When clear, DPI is allowed to issue a MAC memory
                                                         space read that crosses the naturally-aligned
                                                         boundary of size defined by MRRS. (DPI will still
                                                         only cross the boundary when it would eliminate a
                                                         read by doing so.)
                                                         When set, DPI will never issue a MAC memory space
                                                         read that crosses the naturally-aligned boundary
                                                         of size defined by MRRS. */
	uint64_t reserved_2_2                 : 1;
	uint64_t mrrs                         : 2;  /**< Max Read Request Size
                                                                 0 = 128B
                                                                 1 = 256B
                                                                 2 = 512B
                                                                 3 = 1024B
                                                         For PCIe MACs, this MRRS size must not exceed
                                                               the size selected by PCIE*_CFG030[MRRS].
                                                         For sRIO MACs, this MRRS size must be <= 256B. */
#else
	uint64_t mrrs                         : 2;
	uint64_t reserved_2_2                 : 1;
	uint64_t mrrs_lim                     : 1;
	uint64_t mps                          : 1;
	uint64_t reserved_5_6                 : 2;
	uint64_t mps_lim                      : 1;
	uint64_t molr                         : 6;
	uint64_t reserved_14_15               : 2;
	uint64_t rd_mode                      : 1;
	uint64_t reserved_17_19               : 3;
	uint64_t qlm_cfg                      : 1;
	uint64_t reserved_21_23               : 3;
	uint64_t halt                         : 1;
	uint64_t reserved_25_63               : 39;
#endif
	} s;
	struct cvmx_dpi_sli_prtx_cfg_s        cn63xx;
	struct cvmx_dpi_sli_prtx_cfg_s        cn63xxp1;
};
typedef union cvmx_dpi_sli_prtx_cfg cvmx_dpi_sli_prtx_cfg_t;

/**
 * cvmx_dpi_sli_prt#_err
 *
 * DPI_SLI_PRTx_ERR = DPI SLI Port Error Info
 *
 * Logs the Address and Request Queue associated with the reported SLI error response
 */
union cvmx_dpi_sli_prtx_err
{
	uint64_t u64;
	struct cvmx_dpi_sli_prtx_err_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t addr                         : 61; /**< Address of the failed load request. */
	uint64_t reserved_0_2                 : 3;
#else
	uint64_t reserved_0_2                 : 3;
	uint64_t addr                         : 61;
#endif
	} s;
	struct cvmx_dpi_sli_prtx_err_s        cn63xx;
	struct cvmx_dpi_sli_prtx_err_s        cn63xxp1;
};
typedef union cvmx_dpi_sli_prtx_err cvmx_dpi_sli_prtx_err_t;

/**
 * cvmx_dpi_sli_prt#_err_info
 *
 * DPI_SLI_PRTx_ERR_INFO = DPI SLI Port Error Info
 *
 * Logs the Address and Request Queue associated with the reported SLI error response
 */
union cvmx_dpi_sli_prtx_err_info
{
	uint64_t u64;
	struct cvmx_dpi_sli_prtx_err_info_s
	{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint64_t reserved_9_63                : 55;
	uint64_t lock                         : 1;  /**< DPI_SLI_PRTx_ERR and DPI_SLI_PRTx_ERR_INFO have
                                                         captured and locked contents. */
	uint64_t reserved_5_7                 : 3;
	uint64_t type                         : 1;  /**< Type of transaction that caused the ErrorResponse.
                                                         0=DMA Instruction
                                                         1=PKT Instruction */
	uint64_t reserved_3_3                 : 1;
	uint64_t reqq                         : 3;  /**< Request queue that made the failed load request. */
#else
	uint64_t reqq                         : 3;
	uint64_t reserved_3_3                 : 1;
	uint64_t type                         : 1;
	uint64_t reserved_5_7                 : 3;
	uint64_t lock                         : 1;
	uint64_t reserved_9_63                : 55;
#endif
	} s;
	struct cvmx_dpi_sli_prtx_err_info_s   cn63xx;
	struct cvmx_dpi_sli_prtx_err_info_s   cn63xxp1;
};
typedef union cvmx_dpi_sli_prtx_err_info cvmx_dpi_sli_prtx_err_info_t;

#endif
