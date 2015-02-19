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

/* sioLib.h - Serial input output library header*/

#ifndef _sioLib_h
#define _sioLib_h

/* Includes */

#include <vmx.h>

/* Defines */

/* Control commands */
#define SIO_BAUD_SET            0x1003
#define SIO_BAUD_GET            0x1004
#define SIO_HW_OPTS_SET         0x1005
#define SIO_HW_OPTS_GET         0x1006
#define SIO_MODE_SET            0x1007
#define SIO_MODE_GET            0x1008
#define SIO_AVAIL_MODES_GET     0x1009
#define SIO_OPEN                0x100a
#define SIO_HUP                 0x100b
#define SIO_MSTAT_GET           0x100c
#define SIO_MCTRL_BITS_SET      0x100d
#define SIO_MCTRL_BITS_CLR      0x100e
#define SIO_MCTRL_ISIG_MASK     0x100f
#define SIO_MCTRL_OSIG_MASK     0x1010
#define SIO_KYBD_MODE_SET       0x1011
#define SIO_KYBD_MODE_GET       0x1012
#define SIO_KYBD_LED_SET        0x1013
#define SIO_KYBD_LED_GET        0x1014

/* Callback types */
#define SIO_CALLBACK_GET_TX_CHAR     1
#define SIO_CALLBACK_PUT_RCV_CHAR    2
#define SIO_CALLBACK_ERROR           3

/* Error codes */
#define SIO_ERROR_NONE             (-1)
#define SIO_ERROR_FRAMING            0
#define SIO_ERROR_PARITY             1
#define SIO_ERROR_OFLOW              2
#define SIO_ERROR_UFLOW              3
#define SIO_ERROR_CONNECT            4
#define SIO_ERROR_DISCONNECT         5
#define SIO_ERROR_NO_CLK             6
#define SIO_ERROR_UNKNWN             7

/* Options for SIO_MODE_SET */
#define SIO_MODE_POLL                1
#define SIO_MODE_INT                 2

/* Options for SIO_HW_OPTS_SET */
#define CLOCAL                     0x1
#define CREAD                      0x2
#define CSIZE                      0xc

#define CS5                        0x0
#define CS6                        0x4
#define CS7                        0x8
#define CS8                        0xc

#define HUPCL                     0x10
#define STOPB                     0x20
#define PARENB                    0x40
#define PARODD                    0x80

/* Modem signals */
#define SIO_MODEM_DTR             0x01
#define SIO_MODEM_RTS             0x02
#define SIO_MODEM_CTS             0x04
#define SIO_MODEM_CD              0x08
#define SIO_MODEM_RI              0x10
#define SIO_MODEM_DSR             0x20

/* Options for SIO_KYBD_MODE_SET */
#define SIO_KYBD_MODE_RAW            1
#define SIO_KYBD_MODE_ASCII          2
#define SIO_KYBD_MODE_UNICODE        3

/* Options for SIO_KYBD_LED_SET */
#define SIO_KYBD_LED_NUM             1
#define SIO_KYBD_LED_CAP             2
#define SIO_KYBD_LED_SCR             4

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct sio_drv_funcs SIO_DRV_FUNCS;

typedef struct sio_chan {
    SIO_DRV_FUNCS  * pDrvFuncs;
} SIO_CHAN;

struct sio_drv_funcs {
    int  (*ioctl) (
        SIO_CHAN  * pSioChan,
        int         cmd,
        ARG         arg
        );

    int  (*txStartup) (
        SIO_CHAN  * pSioChan
        );

    int  (*callbackInstall) (
        SIO_CHAN  * pSioChan,
        int         callbackType,
        STATUS      (*callback)(void *, ...),
        ARG         callbackArg
        );

    int  (*pollInput) (
        SIO_CHAN  * pSioChan,
        char  *     inChar
        );

    int  (*pollOutput) (
        SIO_CHAN  * pSioChan,
        char  *     outChar
        );
};

/* Macros */

/******************************************************************************
 *
 * SIO_IOCTL - Call ioctl method for driver
 *
 * RETURNS: Return code from ioctl
 */

#define SIO_IOCTL(pSioChan, cmd, arg)                                         \
    ((*(pSioChan)->pDrvFuncs->ioctl)((pSioChan), (cmd), (arg)))

/******************************************************************************
 *
 * SIO_TX_STARTUP - Call TX startup method for driver
 *
 * RETURNS: Return code from txStartup
 */

#define SIO_TX_STARTUP(pSioChan)                                              \
    ((*(pSioChan)->pDrvFuncs->txStartup)((pSioChan)))

/******************************************************************************
 *
 * SIO_CALLBACK_INSTALL - Call callback install method for driver
 *
 * RETURNS: Return code from callback install
 */

#define SIO_CALLBACK_INSTALL(pSioChan, callbackType, callback, callbackArg)   \
    ((*(pSioChan)->pDrvFuncs->callbackInstall)((pSioChan), (callbackType),    \
                                               (callback), (callbackArg)))

/******************************************************************************
 *
 * SIO_POLL_INPUT - Call poll input method for driver
 *
 * RETURNS: Return code from poll input
 */

#define SIO_POLL_INPUT(pSioChan, inChar)                                      \
    ((*(pSioChan)->pDrvFuncs->pollInput)((pSioChan), (inChar)))

/******************************************************************************
 *
 * SIO_POLL_OUTPUT - Call poll output method for driver
 *
 * RETURNS: Return code from poll output
 */

#define SIO_POLL_OUTPUT(pSioChan, outChar)                                    \
    ((*(pSioChan)->pDrvFuncs->pollOutput)((pSioChan), (outChar)))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _sioLib_h */

