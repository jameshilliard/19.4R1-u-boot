/*
 * (C) Copyright 2004,2005
 * Cavium Networks
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

#ifndef __CVMX_CONFIG_H__
#define __CVMX_CONFIG_H__

/**
 * @file config/cvmx-config.h
 *
 * Auto generated config file for the Cavium Octeon Executive.
 * This file should not be edited, as it will be overwritten by cvmx-config.
 *
 * $Id: cvmx-config.h 29304 2007-10-11 17:49:59Z rfranz $
 *
 */

/************************* Config Specific Defines ************************/
#define CVMX_LLM_NUM_PORTS 1
#define CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 1               /**< PKO queues per port for interface 0 (ports 0-15) */
#define CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 1               /**< PKO queues per port for interface 1 (ports 16-31) */
#define CVMX_PKO_MAX_PORTS_INTERFACE0 CVMX_HELPER_PKO_MAX_PORTS_INTERFACE0 /**< Limit on the number of PKO ports enabled for inter
face 0 */
#define CVMX_PKO_MAX_PORTS_INTERFACE1 CVMX_HELPER_PKO_MAX_PORTS_INTERFACE1 /**< Limit on the number of PKO ports enabled for inter
face 1 */
#define CVMX_PKO_QUEUES_PER_PORT_PCI 1                      /**< PKO queues per port for PCI (ports 32-35) */
#define CVMX_PKO_QUEUES_PER_PORT_LOOP 1                     /**< PKO queues per port for Loop devices (ports 36-39) */

/************************* FPA allocation *********************************/
/* Pool sizes in bytes, must be multiple of a cache line */
#define CVMX_FPA_POOL_0_SIZE (16 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_1_SIZE (1 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_2_SIZE (8 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_3_SIZE (0 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_4_SIZE (0 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_5_SIZE (0 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_6_SIZE (0 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_7_SIZE (0 * CVMX_CACHE_LINE_SIZE)

/* Pools in use */
#define CVMX_FPA_PACKET_POOL                (0)             /**< Packet buffers */
#define CVMX_FPA_PACKET_POOL_SIZE           CVMX_FPA_POOL_0_SIZE
#define CVMX_FPA_WQE_POOL                   (1)             /**< Work queue entrys */
#define CVMX_FPA_WQE_POOL_SIZE              CVMX_FPA_POOL_1_SIZE
#define CVMX_FPA_OUTPUT_BUFFER_POOL         (2)             /**< PKO queue command buffers */
#define CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE    CVMX_FPA_POOL_2_SIZE

/*************************  FAU allocation ********************************/
/* The fetch and add registers are allocated here.  They are arranged
    in order of descending size so that all alignment constraints are
    automatically met.
    The enums are linked so that the following enum continues allocating
    where the previous one left off, so the numbering within each
    enum always starts with zero.  The macros take care of the address
    increment size, so the values entered always increase by 1.
    FAU registers are accessed with byte addresses. */

#define CVMX_FAU_REG_64_ADDR(x) ((x <<3) + CVMX_FAU_REG_64_START)
typedef enum
{
    CVMX_FAU_REG_64_START          = 0, 
    CVMX_FAU_REG_OQ_ADDR_INDEX     = CVMX_FAU_REG_64_ADDR(0), /**< FAU registers for the position in PKO command buffers */
        /* Array of 36 */
    CVMX_FAU_REG_64_END            = CVMX_FAU_REG_64_ADDR(36),
} cvmx_fau_reg_64_t;

#define CVMX_FAU_REG_32_ADDR(x) ((x <<2) + CVMX_FAU_REG_32_START)
typedef enum
{
    CVMX_FAU_REG_32_START          = CVMX_FAU_REG_64_END,
    CVMX_FAU_REG_32_END            = CVMX_FAU_REG_32_ADDR(0),
} cvmx_fau_reg_32_t;

#define CVMX_FAU_REG_16_ADDR(x) ((x <<1) + CVMX_FAU_REG_16_START)
typedef enum
{
    CVMX_FAU_REG_16_START          = CVMX_FAU_REG_32_END,
    CVMX_FAU_REG_16_END            = CVMX_FAU_REG_16_ADDR(0),
} cvmx_fau_reg_16_t;

#define CVMX_FAU_REG_8_ADDR(x) ((x) + CVMX_FAU_REG_8_START)
typedef enum {
    CVMX_FAU_REG_8_START           = CVMX_FAU_REG_16_END,
    CVMX_FAU_REG_8_END             = CVMX_FAU_REG_8_ADDR(0),
} cvmx_fau_reg_8_t;

/* The name CVMX_FAU_REG_AVAIL_BASE is provided to indicate the first available
   FAU address that is not allocated in cvmx-config.h. This is 64 bit aligned. */
#define CVMX_FAU_REG_AVAIL_BASE ((CVMX_FAU_REG_8_END + 0x7) & (~0x7ULL))
#define CVMX_FAU_REG_END (2048)

/********************** scratch memory allocation *************************/
/* Scratchpad memory allocation.  Note that these are byte memory addresses.
    Some uses of scratchpad (IOBDMA for example) require the use of 8-byte
    aligned addresses, so proper alignment needs to be taken into account. */

#define CVMX_SCR_OQ_BUF_PRE_ALLOC      (0)                  /**< Pre allocation for PKO queue command buffers */
#define CVMX_SCR_SCRATCH               (8)                  /**< Generic scratch iobdma area */
#define CVMX_SCR_REG_AVAIL_BASE        (16)                 /**< First location available after cvmx-config.h allocated region. */

#endif /* __CVMX_CONFIG_H__ */

