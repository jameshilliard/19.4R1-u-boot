/*
 * bootcpld_tbb_re_sw_struct.h -- register and field definitions for bootcpld_tbb_re
 *
 * Generated by JSPEC 1.10.1.  Do not edit!
 *
 * Copyright (c) 2009-2010, Juniper Networks, Inc.
 * All rights reserved.
 *
 * $Id: bootcpld_tbb_re_sw_struct.h,v 1.1.2.1 2009-08-20 18:12:01 shiva Exp $
 */

#ifndef __BOOTCPLD_TBB_RE_SW_STRUCT_H__
#define __BOOTCPLD_TBB_RE_SW_STRUCT_H__

typedef struct bootcpld_tbb_re {
    volatile u_int8_t CPLD_REV;		/* 0x0 */
        #define   BOOTCPLD_TBB_RE_CPLD_REV_REVISION_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_CPLD_REV_REVISION_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t RESET_REGISTER;		/* 0x1 */
        #define   BOOTCPLD_TBB_RE_RESET_REGISTER_MAIN_RESET 0x00000080
        #define   BOOTCPLD_TBB_RE_RESET_REGISTER_POWER_CYCLE 0x00000040
        #define   BOOTCPLD_TBB_RE_RESET_REGISTER_UKERN_BOOTCPLD_RESET 0x00000020
        #define   BOOTCPLD_TBB_RE_RESET_REGISTER_DDR_RESET 0x00000010
        #define   BOOTCPLD_TBB_RE_RESET_REGISTER_ZL30312_RESET 0x00000008
        #define   BOOTCPLD_TBB_RE_RESET_REGISTER_ENET_0_RESET 0x00000004
        #define   BOOTCPLD_TBB_RE_RESET_REGISTER_DUART_RESET 0x00000002
        #define   BOOTCPLD_TBB_RE_RESET_REGISTER_FLASHB_RESET 0x00000001
    volatile u_int8_t RESET_REASON;		/* 0x2 */
        #define   BOOTCPLD_TBB_RE_RESET_REASON_TIMER_LEVEL2 0x00000020
        #define   BOOTCPLD_TBB_RE_RESET_REASON_TIMER_LEVEL3 0x00000010
        #define   BOOTCPLD_TBB_RE_RESET_REASON_TIMER_LEVEL4 0x00000008
        #define   BOOTCPLD_TBB_RE_RESET_REASON_SW_RST_REASON 0x00000001
    volatile u_int8_t CONTROL_REGISTER;		/* 0x3 */
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_CPU_BOOTED_LED 0x00000080
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_P8572_I2C_EEPROM_EN 0x00000040
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_RE_UKERN_INT 0x00000020
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_RE_UKERN_NMI 0x00000010
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_FLASH_CSELECT_SWIZZLED 0x00000008
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_FLASH_SWIZZLE_ENA 0x00000004
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_WDT_ENA 0x00000002
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_RTC_ENA 0x00000001
    volatile u_int8_t SYS_TIMER_CNT;		/* 0x4 */
        #define   BOOTCPLD_TBB_RE_SYS_TIMER_CNT_TIMER_COUNT_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SYS_TIMER_CNT_TIMER_COUNT_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t WTCHDG_HBYTE_REG;		/* 0x5 */
        #define   BOOTCPLD_TBB_RE_WTCHDG_HBYTE_REG_WTCHDG_HBYTE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_WTCHDG_HBYTE_REG_WTCHDG_HBYTE_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t WTCHDG_LBYTE_REG;		/* 0x6 */
        #define   BOOTCPLD_TBB_RE_WTCHDG_LBYTE_REG_WTCHDG_LBYTE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_WTCHDG_LBYTE_REG_WTCHDG_LBYTE_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t TBB_STATUS1;		/* 0x7 */
        #define   BOOTCPLD_TBB_RE_TBB_STATUS1_PS1_POWER_GOOD 0x00000020
        #define   BOOTCPLD_TBB_RE_TBB_STATUS1_PS0_POWER_GOOD 0x00000010
        #define   BOOTCPLD_TBB_RE_TBB_STATUS1_PS1_PRESENT 0x00000008
        #define   BOOTCPLD_TBB_RE_TBB_STATUS1_PS0_PRESENT 0x00000004
    volatile u_int8_t TIMER_IRQ_ST;		/* 0x8 */
        #define   BOOTCPLD_TBB_RE_TIMER_IRQ_ST_TIMER_IRQ 0x00000080
        #define   BOOTCPLD_TBB_RE_TIMER_IRQ_ST_SOFTWARE_IRQ_0 0x00000040
        #define   BOOTCPLD_TBB_RE_TIMER_IRQ_ST_SOFTWARE_IRQ_1 0x00000020
    volatile u_int8_t TIMER_IRQ_EN;		/* 0x9 */
        #define   BOOTCPLD_TBB_RE_TIMER_IRQ_EN_TIMER_IRQ 0x00000080
        #define   BOOTCPLD_TBB_RE_TIMER_IRQ_EN_SOFTWARE_IRQ 0x00000040
    unsigned char    FILLER_0xa[0x1];
    volatile u_int8_t VOLTAGE_FAIL_A;		/* 0xb */
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_A_VOTAGE_FAIL_A_0 0x00000080
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_A_VOTAGE_FAIL_A_1 0x00000040
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_A_VOTAGE_FAIL_A_2 0x00000020
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_A_VOTAGE_FAIL_A_3 0x00000010
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_A_VOTAGE_FAIL_A_4 0x00000008
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_A_VOTAGE_FAIL_A_5 0x00000004
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_A_VOTAGE_FAIL_A_6 0x00000002
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_A_VOTAGE_FAIL_A_7 0x00000001
    volatile u_int8_t VOLTAGE_FAIL_B;		/* 0xc */
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_B_VOTAGE_FAIL_B_0 0x00000080
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_B_VOTAGE_FAIL_B_1 0x00000040
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_B_VOTAGE_FAIL_B_2 0x00000020
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_B_VOTAGE_FAIL_B_3 0x00000010
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_B_VOTAGE_FAIL_B_4 0x00000008
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_B_VOTAGE_FAIL_B_5 0x00000004
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_B_VOTAGE_FAIL_B_6 0x00000002
        #define   BOOTCPLD_TBB_RE_VOLTAGE_FAIL_B_VOTAGE_FAIL_B_7 0x00000001
    volatile u_int8_t VOLTAGE_TRIM_UP;		/* 0xd */
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_UP_VOTAGE_UP_0 0x00000080
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_UP_VOTAGE_UP_1 0x00000040
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_UP_VOTAGE_UP_2 0x00000020
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_UP_VOTAGE_UP_3 0x00000010
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_UP_VOTAGE_UP_4 0x00000008
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_UP_VOTAGE_UP_5 0x00000004
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_UP_VOTAGE_UP_6 0x00000002
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_UP_VOTAGE_UP_7 0x00000001
    volatile u_int8_t VOLTAGE_TRIM_DOWN;		/* 0xe */
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_DOWN_VOTAGE_DOWN_0 0x00000080
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_DOWN_VOTAGE_DOWN_1 0x00000040
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_DOWN_VOTAGE_DOWN_2 0x00000020
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_DOWN_VOTAGE_DOWN_3 0x00000010
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_DOWN_VOTAGE_DOWN_4 0x00000008
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_DOWN_VOTAGE_DOWN_5 0x00000004
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_DOWN_VOTAGE_DOWN_6 0x00000002
        #define   BOOTCPLD_TBB_RE_VOLTAGE_TRIM_DOWN_VOTAGE_DOWN_7 0x00000001
    volatile u_int8_t FAN_STATUS;		/* 0xf */
        #define   BOOTCPLD_TBB_RE_FAN_STATUS_VALUE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x0000001f))
        #define   BOOTCPLD_TBB_RE_FAN_STATUS_VALUE_GET(CUR) ((CUR) & 0x0000001f)
    volatile u_int8_t FAN_SPEED;		/* 0x10 */
        #define   BOOTCPLD_TBB_RE_FAN_SPEED_VALUE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x0000000f))
        #define   BOOTCPLD_TBB_RE_FAN_SPEED_VALUE_GET(CUR) ((CUR) & 0x0000000f)
    volatile u_int8_t FAN_SPEED_HIGHEST;		/* 0x11 */
        #define   BOOTCPLD_TBB_RE_FAN_SPEED_HIGHEST_VALUE 0x00000001
    volatile u_int8_t FAN_SELECT;		/* 0x12 */
        #define   BOOTCPLD_TBB_RE_FAN_SELECT_VALUE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x00000007))
        #define   BOOTCPLD_TBB_RE_FAN_SELECT_VALUE_GET(CUR) ((CUR) & 0x00000007)
    volatile u_int8_t FAN_TACH;		/* 0x13 */
        #define   BOOTCPLD_TBB_RE_FAN_TACH_VALUE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_FAN_TACH_VALUE_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t FAN_FAILURE_THRESHOLD;		/* 0x14 */
        #define   BOOTCPLD_TBB_RE_FAN_FAILURE_THRESHOLD_VALUE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_FAN_FAILURE_THRESHOLD_VALUE_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t RED_YELLOW_ALARM;		/* 0x15 */
        #define   BOOTCPLD_TBB_RE_RED_YELLOW_ALARM_RED_ALARM 0x00000002
        #define   BOOTCPLD_TBB_RE_RED_YELLOW_ALARM_YELLOW_ALARM 0x00000001
    unsigned char    FILLER_0x16[0x1];
    volatile u_int8_t FAILURE_ALARM_INT;		/* 0x17 */
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_PEM0_FAIL 0x00000080
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_PEM1_FAIL 0x00000040
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_PEM0_TEMP_NOK 0x00000020
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_PEM1_TEMP_NOK 0x00000010
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_FAN_FAIL 0x00000008
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_FAN_PRESENCE 0x00000004
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_TEMP_ALARM_EXHAUST_A 0x00000002
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_TEMP_ALARM_EXHAUST_B 0x00000001
    volatile u_int8_t FAILURE_ALARM_INT_MASK;		/* 0x18 */
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_MASK_PEM0_FAIL_CHANGE_MASK 0x00000080
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_MASK_PEM1_FAIL_CHANGE_MASK 0x00000040
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_MASK_PEM0_TEMP_NOK_CHANGE_MASK 0x00000020
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_MASK_PEM1_TEMP_NOK_CHANGE_MASK 0x00000010
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_MASK_FAN_FAIL_CHANGE_MASK 0x00000008
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_MASK_FAN_PRESENCE_CHANGE_MASK 0x00000004
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_MASK_TEMP_ALARM_EXHAUST_A_CHANGE_MASK 0x00000002
        #define   BOOTCPLD_TBB_RE_FAILURE_ALARM_INT_MASK_TEMP_ALARM_EXHAUST_B_CHANGE_MASK 0x00000001
    volatile u_int8_t MISC_CPU_INT;		/* 0x19 */
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_UKERN_RE_CPU_INT 0x00000020
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_USB_CNTRL_INT 0x00000010
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_USB_PCI_INT 0x00000008
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_NMI_SWITCH_INT 0x00000004
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_UKERN_RE_CPU_NMI 0x00000002
    volatile u_int8_t MISC_CPU_INT_MASK;		/* 0x1a */
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_MASK_UKERN_RE_CPU_INT_MASK 0x00000020
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_MASK_USB_CNTRL_INT_MASK 0x00000010
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_MASK_USB_PCI_INT_MASK 0x00000008
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_MASK_NMI_SWITCH_INT_MASK 0x00000004
    volatile u_int8_t MISC_CPU_INT_1;		/* 0x1b */
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_LOS_LIU_A 0x00000010
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_LOS_LIU_B 0x00000008
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_LOS_LIU_B_RESET_VALUE 0x00000008
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_ZL30312_INT0 0x00000004
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_ZL30312_INT1 0x00000002
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_ZL_PHY_INT 0x00000001
    volatile u_int8_t MISC_CPU_INT_1_MASK;		/* 0x1c */
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_MASK_LOS_LIU_A_MASK 0x00000010
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_MASK_LOS_LIU_B_MASK 0x00000008
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_MASK_ZL30312_INT0_MASK 0x00000004
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_MASK_ZL30312_INT1_MASK 0x00000002
        #define   BOOTCPLD_TBB_RE_MISC_CPU_INT_1_MASK_ZL_PHY_INT_MASK 0x00000001
    volatile u_int8_t LED_SYSTEM_OK;		/* 0x1d */
        #define   BOOTCPLD_TBB_RE_LED_SYSTEM_OK_SYSTEM_OK_GREEN 0x00000008
        #define   BOOTCPLD_TBB_RE_LED_SYSTEM_OK_SYSTEM_OK_RED 0x00000004
        #define   BOOTCPLD_TBB_RE_LED_SYSTEM_OK_RE_UP 0x00000002
        #define   BOOTCPLD_TBB_RE_LED_SYSTEM_OK_RE_COMING_UP 0x00000001
    volatile u_int8_t SYNC_ETHERNET_CONTROL;		/* 0x1e */
        #define   BOOTCPLD_TBB_RE_SYNC_ETHERNET_CONTROL_CLOCK_BITS_B_PRI 0x00000080
        #define   BOOTCPLD_TBB_RE_SYNC_ETHERNET_CONTROL_CLOCK_BITS_A_SEC 0x00000040
        #define   BOOTCPLD_TBB_RE_SYNC_ETHERNET_CONTROL_MIC_CLK_SEC_0_SET(CUR, NEWVAL) (((NEWVAL) << 4) | ((CUR) & ~0x00000030))
        #define   BOOTCPLD_TBB_RE_SYNC_ETHERNET_CONTROL_MIC_CLK_SEC_0_GET(CUR) (((CUR) >> 4) & 0x00000003)
        #define   BOOTCPLD_TBB_RE_SYNC_ETHERNET_CONTROL_MIC_CLK_PRI_0_SET(CUR, NEWVAL) (((NEWVAL) << 2) | ((CUR) & ~0x0000000c))
        #define   BOOTCPLD_TBB_RE_SYNC_ETHERNET_CONTROL_MIC_CLK_PRI_0_GET(CUR) (((CUR) >> 2) & 0x00000003)
    volatile u_int8_t CONTROL_REGISTER_2;		/* 0x1f */
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_2_PS0_DISABLE 0x00000080
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_2_PS1_DISABLE 0x00000040
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_2_P8572_PCI_RST_L 0x00000008
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_2_FLASH_B_ACC 0x00000004
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_2_LIU_A_RESET 0x00000002
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_2_LIU_B_RESET 0x00000001
    volatile u_int8_t CONTROL_REGISTER_3;		/* 0x20 */
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_3_EXT_CLK_A_LED_ENABLE 0x00000080
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_3_EXT_CLK_B_LED_ENABLE 0x00000040
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_3_E1_MODE 0x00000020
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_3_PLL_8K_REF_CLK 0x00000010
        #define   BOOTCPLD_TBB_RE_CONTROL_REGISTER_3_GPS_CLKOUT_EN 0x00000008
    volatile u_int8_t GPIN_REGISTER;		/* 0x21 */
        #define   BOOTCPLD_TBB_RE_GPIN_REGISTER_GPIN_0 0x00000080
        #define   BOOTCPLD_TBB_RE_GPIN_REGISTER_GPIN_1 0x00000040
        #define   BOOTCPLD_TBB_RE_GPIN_REGISTER_GPIN_2 0x00000020
        #define   BOOTCPLD_TBB_RE_GPIN_REGISTER_GPIN_3 0x00000010
    volatile u_int8_t GPOUT_REGISTER;		/* 0x22 */
        #define   BOOTCPLD_TBB_RE_GPOUT_REGISTER_GPOUT_0 0x00000080
        #define   BOOTCPLD_TBB_RE_GPOUT_REGISTER_GPOUT_1 0x00000040
        #define   BOOTCPLD_TBB_RE_GPOUT_REGISTER_GPOUT_2 0x00000020
        #define   BOOTCPLD_TBB_RE_GPOUT_REGISTER_GPOUT_3 0x00000010
        #define   BOOTCPLD_TBB_RE_GPOUT_REGISTER_GPOUT_4 0x00000008
    volatile u_int8_t SCRATCH_1;		/* 0x23 */
        #define   BOOTCPLD_TBB_RE_SCRATCH_1_SCRATCH_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SCRATCH_1_SCRATCH_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t SCRATCH_2;		/* 0x24 */
        #define   BOOTCPLD_TBB_RE_SCRATCH_2_SCRATCH_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SCRATCH_2_SCRATCH_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t WTCHDG_UBYTE_REG;		/* 0x25 */
        #define   BOOTCPLD_TBB_RE_WTCHDG_UBYTE_REG_WTCHDG_UBYTE_SET(CUR, NEWVAL) (((NEWVAL) << 4) | ((CUR) & ~0x000000f0))
        #define   BOOTCPLD_TBB_RE_WTCHDG_UBYTE_REG_WTCHDG_UBYTE_GET(CUR) (((CUR) >> 4) & 0x0000000f)
    unsigned char    FILLER_0x26[0xa];
    volatile u_int8_t SPI_CNTRL;		/* 0x30 */
        #define   BOOTCPLD_TBB_RE_SPI_CNTRL_WRITE 0x00000004
        #define   BOOTCPLD_TBB_RE_SPI_CNTRL_READ 0x00000002
        #define   BOOTCPLD_TBB_RE_SPI_CNTRL_WDATCNT 0x00000001
    volatile u_int8_t SPI_ADDR;		/* 0x31 */
        #define   BOOTCPLD_TBB_RE_SPI_ADDR_ADDR_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SPI_ADDR_ADDR_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t SPI_DBG_RD;		/* 0x32 */
        #define   BOOTCPLD_TBB_RE_SPI_DBG_RD_RD_CNT_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SPI_DBG_RD_RD_CNT_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t SPI_DBG_WR;		/* 0x33 */
        #define   BOOTCPLD_TBB_RE_SPI_DBG_WR_WR_CNT_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SPI_DBG_WR_WR_CNT_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t SPI_STATUS;		/* 0x34 */
        #define   BOOTCPLD_TBB_RE_SPI_STATUS_READ_RDY 0x00000002
        #define   BOOTCPLD_TBB_RE_SPI_STATUS_WRITE_DONE 0x00000001
    volatile u_int8_t SPI_RDAT;		/* 0x35 */
        #define   BOOTCPLD_TBB_RE_SPI_RDAT_RDAT_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SPI_RDAT_RDAT_GET(CUR) ((CUR) & 0x000000ff)
    unsigned char    FILLER_0x36[0x4];
    volatile u_int8_t SPI_RE_WDATL;		/* 0x3a */
        #define   BOOTCPLD_TBB_RE_SPI_RE_WDATL_VALUE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SPI_RE_WDATL_VALUE_GET(CUR) ((CUR) & 0x000000ff)
    volatile u_int8_t SPI_RE_WDATH;		/* 0x3b */
        #define   BOOTCPLD_TBB_RE_SPI_RE_WDATH_VALUE_SET(CUR, NEWVAL) ((NEWVAL) | ((CUR) & ~0x000000ff))
        #define   BOOTCPLD_TBB_RE_SPI_RE_WDATH_VALUE_GET(CUR) ((CUR) & 0x000000ff)
    unsigned char    FILLER_0x3c[0x6];
} bootcpld_tbb_re;


#endif /* __BOOTCPLD_TBB_RE_SW_STRUCT_H__ */
