/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real VMX.
*   Copyright (C) 2008 Surplus Users Ham Society
*
*   Real VMX is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   Real VMX is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with Real VMX.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/* config/x86/config.h - arch configuration settings */

#ifndef _config_h_
#define _config_h_

/* Include arch independant config */
#include "../all/configAll.h"
#include "configInum.h"

#define VM_INVALID_ADDR         -1
#define VM_PAGE_SIZE            PAGE_SIZE_4KB
#define VM_PAGE_OFFSET          VM_PAGE_SIZE
#define VM_STATE_MASK_FOR_ALL \
  VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE
#define VM_STATE_FOR_IO \
  VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_NOT_CACHEABLE
#define VM_STATE_FOR_MEM_OS \
  VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE
#define VM_STATE_FOR_MEM_APPLICATION \
  VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE

#define USER_RESERVED_MEM       0x00100000
#define LOCAL_MEM_LOCAL_ADRS    0x00100000
#define LOCAL_MEM_SIZE_OS       0x00300000
#define SYSTEM_RAM_SIZE         0x00900000
#define FREE_MEM_START_ADRS     0x00400000
#define LOCAL_MEM_SIZE          (SYSTEM_RAM_SIZE - LOCAL_MEM_LOCAL_ADRS)

#define GDT_BASE_OFFSET		(1 * VM_PAGE_SIZE)

/* PC interrupt controller related */
#define PIC_REG_ADDR_INTERVAL   1

/* PC timer related */
#define PIT0_INT_LVL            0x00
#define PIT_BASE_ADR            0x40
#define PIT_REG_ADDR_INTERVAL   1
#define PIT_CLOCK               1193180

#define SYS_CLOCK_RATE_MIN      19
#define SYS_CLOCK_RATE_MAX      8192

/* PC serial ports related */
#define COM1_BASE_ADR           0x03f8
#define COM2_BASE_ADR           0x02f8
#define COM3_BASE_ADR           0x03e8
#define COM4_BASE_ADR           0x02e8

#define COM1_INT_LVL            0x04
#define COM2_INT_LVL            0x03
#define COM3_INT_LVL            0x05
#define COM4_INT_LVL            0x09

#define UART_REG_ADDR_INTERVAL  1

#define N_UART_CHANNELS         2

/* PC vga related */
#define DIAG_CTRL               0x61

#define BEEP_PITCH_L            1280
#define BEEP_TIME_L             18
#define BEEP_PITCH_S            1280
#define BEEP_TIME_S             9

#define COLOR                   1
#define VGA_MEM_BASE            (u_int8_t *) 0xb8000
#define VGA_SEL_REG             (u_int8_t *) 0x3d4
#define VGA_VAL_REG             (u_int8_t *) 0x3d5

#define DEFAULT_FG              ATRB_FG_BRIGHTWHITE
#define DEFAULT_BG              ATRB_BG_BLUE
#define DEFAULT_ATR             DEFAULT_FG | DEFAULT_BG
#define COLOR_MODE              COLOR
#define CTRL_MEM_BASE           VGA_MEM_BASE
#define CTRL_SEL_REG            VGA_SEL_REG
#define CTRL_VAL_REG            VGA_VAL_REG
#define CHR                     2

/* PC keyboard related */
#define KBD_INT_LVL             0x01
#define COMMAND_8042            0x64
#define DATA_8042               0x60
#define STATUS_8042             COMMAND_8042
#define COMMAND_8048            0x61
#define DATA_8048               0x60
#define STATUS_8048             COMMAND_8048

/* For sysDelay */
#define UNUSED_ISA_IO_ADDRESS   0x84

/* Pc floppy related */
#define FD_DMA_BUF_ADDR         (0x2000 + (VM_PAGE_OFFSET * 2))
#define FD_DMA_BUF_SIZE         (0x1000)

/* For console */
#define PC_CONSOLE              0
#define N_VIRTUAL_CONSOLES      3

/* Configuration */
#define MAX_EVENT_CATEGORIES    20
#define MAX_EVENT_TYPES         20
#define MAX_XBD_DEVICES         64

#define MAX_FS_BUFFERS          16
#define MAX_FS_FILES            16
#define MAX_FS_DIR_ENTRIES     100

#define INCLUDE_CONSTANT_RDY_Q
#define INCLUDE_CACHE_SUPPORT
#define INCLUDE_MMU
#define INCLUDE_PC_CONSOLE
#define INCLUDE_PC_MOUSE
#define INCLUDE_RAM_DRV
#define INCLUDE_FD_DRV
#define INCLUDE_NET

#define INCLUDE_USR_TEST
#define INCLUDE_FS_DEMO
#define INCLUDE_NET_DEMO
#define INCLUDE_UGL_DEMO

#endif

