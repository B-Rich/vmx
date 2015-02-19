/******************************************************************************
 *   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 *   This file is part of Real VMX.
 *   Copyright (C) 2015 Surplus Users Ham Society
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
 */

/* i8250Sio.h - PC compatibles serial driver header */

#ifndef _i8250Sio_h
#define _i8250Sio_h

/* Includes */

#include <sys/types.h>
#include <vmx.h>
#include <os/sioLib.h>

/* Defines */

/* Registers */
#define UART_THR                0x00
#define UART_RDR                0x00
#define UART_BRDL               0x00
#define UART_BRDH               0x01
#define UART_IER                0x01
#define UART_IID                0x02
#define UART_LCR                0x03
#define UART_MDC                0x04
#define UART_LST                0x05
#define UART_MSR                0x06

/* Interrupt enable register */
#define I8250_IER_RXDY          0x01
#define I8250_IER_TBE           0x02
#define I8250_IER_LST           0x04
#define I8250_IER_MSI           0x08

/* Interrupt identification register */
#define I8250_IRR_MSTAT         0x00
#define I8250_IIR_IP            0x01
#define I8250_IIR_THRE          0x02
#define I8250_IIR_RBRF          0x04
#define I8250_IIR_SEOB          0x06
#define I8250_IIR_MASK          0x07

/* Line control register */
#define I8250_LCR_CS5           0x00
#define I8250_LCR_CS6           0x01
#define I8250_LCR_CS7           0x02
#define I8250_LCR_CS8           0x03
#define I8250_LCR_1_STB         0x00
#define I8250_LCR_2_STB         0x04
#define I8250_LCR_PEN           0x08
#define I8250_LCR_PDIS          0x00
#define I8250_LCR_EPS           0x10
#define I8250_LCR_SP            0x20
#define I8250_LCR_SBRK          0x40
#define I8250_LCR_DLAB          0x80

/* Line status register */
#define I8250_LSR_RXRDY         0x01
#define I8250_LSR_OE            0x02
#define I8250_LSR_PE            0x04
#define I8250_LSR_FE            0x08
#define I8250_LSR_BI            0x10
#define I8250_LSR_THRE          0x20
#define I8250_LSR_TEMT          0x40

/* Modem control register */
#define I8250_MCR_DTR           0x01
#define I8250_MCR_RTS           0x02
#define I8250_MCR_OUT1          0x04
#define I8250_MCR_OUT2          0x08
#define I8250_MCR_LOOP          0x10

/* Modem status register */
#define I8250_MSR_DCTS          0x01
#define I8250_MSR_DDSR          0x02
#define I8250_MSR_DRI           0x04
#define I8250_MSR_DDCD          0x08
#define I8250_MSR_CTS           0x10
#define I8250_MSR_DSR           0x20
#define I8250_MSR_RI            0x40
#define I8250_MSR_DCD           0x80

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct {
    SIO_DRV_FUNCS  * pDrvFuncs;

    STATUS         (*getTxChar) (
        void
        );

    STATUS         (*putRcvChar) (
        void
        );

    ARG              getTxArg;
    ARG              putRcvArg;

    u_int16_t        int_vec;
    u_int16_t        channelMode;

    u_int8_t        (*inByte) (
        int     reg
        );

    void            (*outByte) (
        int     reg,
        int8_t  c
        );

    u_int32_t         lcr;
    u_int32_t         lst;
    u_int32_t         mdc;
    u_int32_t         msr;
    u_int32_t         ier;
    u_int32_t         iid;
    u_int32_t         brdl;
    u_int32_t         brdh;
    u_int32_t         data;
    u_int32_t         options;
} I8250_CHAN;

typedef struct {
    int               rate;
    int               preset;
} BAUD;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _i8250Sio_h */

