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

/* sysSerial.c - Serial driver initialization */

/* Includes */
#include <string.h>
#include <sys/types.h>
#include <vmx.h>

#include <drv/sio/i8250Sio.c>

/* Macros */

#define UART_REG(reg, chan) \
    (devParams[(chan)].baseAdrs + (reg) * devParams[chan].regSpace)

/* Types */

typedef struct {
    u_int16_t  vector;
    u_int32_t  baseAdrs;
    u_int16_t  regSpace;
    u_int16_t  intLevel;
} I8250_CHAN_PARAMS;

/* Locals */

LOCAL I8250_CHAN        i8250Chan[N_UART_CHANNELS];
LOCAL I8250_CHAN_PARAMS devParams[] = {
    { INT_NUM_COM1, COM1_BASE_ADR, UART_REG_ADDR_INTERVAL, COM1_INT_LVL },
    { INT_NUM_COM2, COM2_BASE_ADR, UART_REG_ADDR_INTERVAL, COM2_INT_LVL }
};

void  sysSerialHwInit (
    void
    ) {
    int  i;

    for (i = 0; i < N_UART_CHANNELS; i++) {
        memset(&i8250Chan[i], 0, sizeof(I8250_CHAN));
        i8250Chan[i].int_vec     = devParams[i].vector;
        i8250Chan[i].channelMode = 0x0000;
        i8250Chan[i].lcr         = UART_REG(UART_LCR, i);
        i8250Chan[i].data        = UART_REG(UART_RDR, i);
        i8250Chan[i].brdl        = UART_REG(UART_BRDL, i);
        i8250Chan[i].brdh        = UART_REG(UART_BRDH, i);
        i8250Chan[i].ier         = UART_REG(UART_IER, i);
        i8250Chan[i].iid         = UART_REG(UART_IID, i);
        i8250Chan[i].mdc         = UART_REG(UART_MDC, i);
        i8250Chan[i].lst         = UART_REG(UART_LST, i);
        i8250Chan[i].msr         = UART_REG(UART_MSR, i);

        i8250HrdInit(&i8250Chan[i]);
    }
}

void sysSerialHwInit2 (
    void
    ) {
    int  i;

    for (i = 0; i < N_UART_CHANNELS; i++) {
        if (i8250Chan[i].int_vec != 0) {
            intConnectDefault(i8250Chan[i].int_vec, i8250Int, &i8250Chan[i]);
            sysIntEnablePIC(devParams[i].intLevel);
        }
    }
}

SIO_CHAN *  sysSerialChanGet (
    int  channel
    ) {
    SIO_CHAN *  pSioChan;

    if (channel >= 0 && channel < N_UART_CHANNELS) {
        pSioChan = (SIO_CHAN *) &i8250Chan[channel];
    }
    else {
        pSioChan = NULL;
    }

    return pSioChan;
}

