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
 * This module provides system/board/application information obtained by the bootloader.
 *
 * <hr>$Revision: 1.2 $<hr>
 *
 */

#include "cvmx.h"
#include "cvmx-spinlock.h"
#include "cvmx-sysinfo.h"

/**
 * This structure defines the private state maintained by sysinfo module.
 *
 */
CVMX_SHARED static struct {

    cvmx_sysinfo_t   sysinfo;      /**< system information */
    cvmx_spinlock_t  lock;         /**< mutex spinlock */

} state = {
    .lock = CVMX_SPINLOCK_UNLOCKED_INITIALIZER
};

#ifdef CVMX_BUILD_FOR_LINUX_USER
/* Global variable with the processor ID since we can't read it directly */
CVMX_SHARED uint32_t cvmx_app_init_processor_id;
#endif

/* Global variables that define the min/max of the memory region set up for 32 bit userspace access */
uint64_t linux_mem32_min = 0;
uint64_t linux_mem32_max = 0;
uint64_t linux_mem32_wired = 0;
uint64_t linux_mem32_offset = 0;

/**
 * This function returns the application information as obtained
 * by the bootloader.  This provides the core mask of the cores
 * running the same application image, as well as the physical
 * memory regions available to the core.
 *
 * @return  Pointer to the boot information structure
 *
 */
cvmx_sysinfo_t * cvmx_sysinfo_get(void)
{
    return &(state.sysinfo);
}


/**
 * This function is used in non-simple executive environments (such as Linux kernel, u-boot, etc.)
 * to configure the minimal fields that are required to use
 * simple executive files directly.
 *
 * Locking (if required) must be handled outside of this
 * function
 *
 * @param phy_mem_desc_ptr
 *                   Pointer to global physical memory descriptor (bootmem descriptor)
 * @param board_type Octeon board type enumeration
 *
 * @param board_rev_major
 *                   Board major revision
 * @param board_rev_minor
 *                   Board minor revision
 * @param cpu_clock_hz
 *                   CPU clock freqency in hertz
 *
 * @return 0: Failure
 *         1: success
 */
int cvmx_sysinfo_minimal_initialize(void *phy_mem_desc_ptr, uint16_t board_type, uint8_t board_rev_major,
                                    uint8_t board_rev_minor, uint32_t cpu_clock_hz)
{

    /* The sysinfo structure was already initialized */
    if (state.sysinfo.board_type)
        return(0);

    memset(&(state.sysinfo), 0x0, sizeof(state.sysinfo));
    state.sysinfo.phy_mem_desc_ptr = phy_mem_desc_ptr;
    state.sysinfo.board_type = board_type;
    state.sysinfo.board_rev_major = board_rev_major;
    state.sysinfo.board_rev_minor = board_rev_minor;
    state.sysinfo.cpu_clock_hz = cpu_clock_hz;

    return(1);
}

#ifdef CVMX_BUILD_FOR_LINUX_USER
/**
 * Initialize the sysinfo structure when running on
 * Octeon under Linux userspace
 */
void cvmx_sysinfo_linux_userspace_initialize(void)
{
    cvmx_sysinfo_t *system_info = cvmx_sysinfo_get();
    memset(system_info, 0, sizeof(cvmx_sysinfo_t));

    system_info->core_mask = 0;
    system_info->init_core = -1;

    FILE *infile = fopen("/proc/octeon_info", "r");
    if (infile == NULL)
    {
        perror("Error opening /proc/octeon_info");
        exit(-1);
    }

    while (!feof(infile))
    {
        char buffer[80];
        if (fgets(buffer, sizeof(buffer), infile))
        {
            const char *field = strtok(buffer, " ");
            const char *valueS = strtok(NULL, " ");
            if (field == NULL)
                continue;
            if (valueS == NULL)
                continue;
            unsigned long long value;
            sscanf(valueS, "%lli", &value);

            if (strcmp(field, "dram_size:") == 0)
                system_info->system_dram_size = value;
            else if (strcmp(field, "phy_mem_desc_addr:") == 0)
                system_info->phy_mem_desc_ptr = cvmx_phys_to_ptr(value);
            else if (strcmp(field, "eclock_hz:") == 0)
                system_info->cpu_clock_hz = value;
            else if (strcmp(field, "dclock_hz:") == 0)
                system_info->dram_data_rate_hz = value * 2;
            else if (strcmp(field, "board_type:") == 0)
                system_info->board_type = value;
            else if (strcmp(field, "board_rev_major:") == 0)
                system_info->board_rev_major = value;
            else if (strcmp(field, "board_rev_minor:") == 0)
                system_info->board_rev_minor = value;
            else if (strcmp(field, "board_serial_number:") == 0)
                strncpy(system_info->board_serial_number, valueS, sizeof(system_info->board_serial_number));
            else if (strcmp(field, "mac_addr_base:") == 0)
            {
                int i;
                int m[6];
                sscanf(valueS, "%02x:%02x:%02x:%02x:%02x:%02x", m+0, m+1, m+2, m+3, m+4, m+5);
                for (i=0; i<6; i++)
                    system_info->mac_addr_base[i] = m[i];
            }
            else if (strcmp(field, "mac_addr_count:") == 0)
                system_info->mac_addr_count = value;
            else if (strcmp(field, "32bit_shared_mem_base:") == 0)
                linux_mem32_min = value;
            else if (strcmp(field, "32bit_shared_mem_size:") == 0)
                linux_mem32_max = linux_mem32_min + value - 1;
            else if (strcmp(field, "processor_id:") == 0)
                cvmx_app_init_processor_id = value;
            else if (strcmp(field, "32bit_shared_mem_wired:") == 0)
                linux_mem32_wired = value;
        }
    }
}
#endif
