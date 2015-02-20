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

/* i8250Sio.c - PC compatibles serial driver */

#include <errno.h>
#include <sys/types.h>
#include <vmx.h>
#include <arch/intArchLib.h>
#include <drv/sio/i8250Sio.h>

/* Defines */

#define I8250_IRR_READ_MAX          40
#define I8250_DEFAULT_BAUD        9600
#define I8250_SIO_OPEN          0x100a
#define I8250_SIO_HUP           0x100b

/* Locals */

LOCAL SIO_DRV_FUNCS  i8250SioDrvFuncs;
LOCAL BOOL           i8250SioInstalled = FALSE;
LOCAL BAUD           baudTable [] = {
    {     50, 2304 },
    {     75, 1536 },
    {    110, 1047 },
    {    134,  857 },
    {    150,  768 },
    {    300,  384 },
    {    600,  192 },
    {   1200,   96 },
    {   2000,   58 },
    {   2400,   48 },
    {   3600,   32 },
    {   4800,   24 },
    {   7200,   16 },
    {   9600,   12 },
    {  19200,    6 },
    {  38400,    3 },
    {  57600,    2 },
    { 115200,    1 }
};

LOCAL int  i8250Init (
    I8250_CHAN *  pChan
    );

LOCAL STATUS  i8250BaudSet (
    I8250_CHAN *  pChan,
    int           rate
    );

LOCAL int  i8250Ioctl (
    SIO_CHAN *  pSioChan,
    int         req,
    ARG         arg
    );

LOCAL int  i8250TxStartup (
    SIO_CHAN *  pSioChan
    );

LOCAL int  i8250CallbackInstall (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS    (*callback)(void *, ...),
    ARG         callbackArg
    );

LOCAL int  i8250RxChar (
    SIO_CHAN *  pSioChan,
    char *      pChar
    );

LOCAL int  i8250TxChar (
    SIO_CHAN *  pSioChan,
    char        outChar
    );

/******************************************************************************
 *
 * i8250HrdInit - Initialize serial communication device
 *
 * RETURNS: N/A
 */

void  i8250HrdInit (
    I8250_CHAN *  pChan
    ) {

    if (i8250SioInstalled == FALSE) {
        i8250SioDrvFuncs.ioctl           = i8250Ioctl;
        i8250SioDrvFuncs.txStartup       = i8250TxStartup;
        i8250SioDrvFuncs.callbackInstall = i8250CallbackInstall;
        i8250SioDrvFuncs.pollInput       = i8250RxChar;
        i8250SioDrvFuncs.pollOutput      = i8250TxChar;

        pChan->pDrvFuncs = &i8250SioDrvFuncs;

        i8250Init(pChan);

        i8250SioInstalled = TRUE;
    }
}

/******************************************************************************
 *
 * i8250Init - Initialize comminication channel
 *
 * RETURNS: N/A
 */

LOCAL int  i8250Init (
    I8250_CHAN *  pChan
    ) {
    int  level;

    INT_LOCK(level);

    /* Baud rate */
    i8250BaudSet(pChan, I8250_DEFAULT_BAUD);

    /* 8 data bits, 1 stop bit and no parity */
    (*pChan->outByte)(pChan->lcr, (I8250_LCR_CS8 | I8250_LCR_1_STB));
    (*pChan->outByte)(
        pChan->mdc,
        (I8250_MCR_RTS | I8250_MCR_DTR | I8250_MCR_OUT2)
        );

    /* Clear */
    (*pChan->inByte)(pChan->data);

    /* Disable interrupts */
    (*pChan->outByte)(pChan->ier, 0x00);

    /* Set options */
    pChan->options = (CLOCAL | CREAD | CS8);

    INT_UNLOCK(level);
}

/******************************************************************************
 *
 * i8250Open - Open connection
 *
 * RETURNS: OK
 */

LOCAL STATUS  i8250Open (
    I8250_CHAN *  pChan
    ) {
    int     level;
    int8_t  mask;

    mask = ((*pChan->inByte)(pChan->mdc)) & (I8250_MCR_RTS | I8250_MCR_DTR);

    if (mask != (I8250_MCR_RTS | I8250_MCR_DTR)) {
        INT_LOCK(level);

        (*pChan->outByte)(
            pChan->mdc,
            (I8250_MCR_RTS | I8250_MCR_DTR | I8250_MCR_OUT2)
            );

        INT_UNLOCK(level);
    }

    return OK;
}

/******************************************************************************
 *
 * i8250Hup - Hang up connection
 *
 * RETURNS: OK
 */

LOCAL STATUS  i8250Hup (
    I8250_CHAN *  pChan
    ) {
    int  level;

    INT_LOCK(level);

    (*pChan->outByte)(pChan->mdc, I8250_MCR_OUT2);

    INT_UNLOCK(level);

    return OK;
}

/******************************************************************************
 *
 * i8250BaudSet - Set baud rate
 *
 * RETURNS: OK or ERROR
 */

LOCAL STATUS  i8250BaudSet (
    I8250_CHAN *  pChan,
    int           rate
    ) {
    STATUS    status = ERROR;
    int       level;
    int       i;
    u_int8_t  lcr;

    INT_LOCK(level);

    for (i = 0; i < NELEMENTS(baudTable); i++) {
        if (baudTable[i].rate == rate) {
            lcr = (*pChan->inByte)(pChan->lcr);
            (*pChan->outByte)(pChan->lcr, (int8_t) (I8250_LCR_DLAB | lcr));
            (*pChan->outByte)(pChan->brdh, MSB(baudTable[i].preset));
            (*pChan->outByte)(pChan->brdl, LSB(baudTable[i].preset));
            (*pChan->outByte)(pChan->lcr, lcr);
            status = OK;
            break;
        }
    }

    INT_UNLOCK(level);
}

/******************************************************************************
 *
 * i8250ModeSet - Setup channel mode
 *
 * RETURNS: OK or ERROR
 */

LOCAL STATUS  i8250ModeSet (
    I8250_CHAN *  pChan,
    u_int16_t     mode
    ) {
    STATUS  status;
    int     level;
    int8_t  ier;
    int8_t  mask;

    if (mode != SIO_MODE_POLL && mode != SIO_MODE_INT) {
        status = ERROR;
    }
    else {
        INT_LOCK(level);

        if (mode == SIO_MODE_POLL) {
            ier = 0x00;
        }
        else {
            if ((pChan->options & CLOCAL) != 0x00) {
                ier = I8250_IER_RXRDY;
            }
            else {
                mask = ((*pChan->inByte)(pChan->msr)) & I8250_MSR_CTS;
                if ((mask & I8250_MSR_CTS) != 0x00) {
                    ier = (I8250_IER_TBE | I8250_IER_MSI);
                }
                else {
                    ier = I8250_IER_MSI;
                }
            }
        }

        (*pChan->outByte)(pChan->ier, ier);

        pChan->channelMode = mode;

        INT_UNLOCK(level);
        status = OK;
    }

    return status;
}

/******************************************************************************
 *
 * i8250OptSet - Setup channel options
 *
 * RETURNS: OK or ERROR
 */

LOCAL STATUS  i8250OptSet (
    I8250_CHAN *  pChan,
    u_int32_t     options
    ) {
    STATUS  status;
    int     level;
    int8_t  mask;
    int8_t  ier;
    int8_t  lcr;
    int8_t  mcr = 0;

    ier = (*pChan->inByte)(pChan->ier);
    if ((options & 0xffffff00) != 0x00) {
        status = ERROR;
    }
    else {
        switch(options & CSIZE) {
            case CS5:
                lcr = I8250_LCR_CS5;
                break;

            case CS6:
                lcr = I8250_LCR_CS6;
                break;

            case CS7:
                lcr = I8250_LCR_CS7;
                break;

            case CS8:
                lcr = I8250_LCR_CS8;
                break;

            default:
                lcr = I8250_LCR_CS8;
                break;
        }

        if ((options & STOPB) != 0x00) {
            lcr |= I8250_LCR_2_STB;
        }
        else {
            lcr |= I8250_LCR_1_STB;
        }

        switch (options & (PARENB | PARODD)) {
            case 0:
                lcr |= I8250_LCR_PDIS;
                break;

            case PARENB | PARODD:
                lcr |= I8250_LCR_PEN;
                break;

            case PARENB:
                lcr |= (I8250_LCR_PEN | I8250_LCR_EPS);
                break;

            default:
                lcr |= I8250_LCR_PDIS;
                break;
        }

        (*pChan->outByte)(pChan->ier, 0x00);

        if ((options & CLOCAL) == 0x00) {
            mcr |= (I8250_MCR_DTR | I8250_MCR_RTS);
            ier |= I8250_IER_MSI;
            mask = ((*pChan->inByte)(pChan->msr)) & I8250_MSR_CTS;
            if ((mask & I8250_MSR_CTS) != 0x00) {
                ier |= I8250_IER_TBE;
            }
            else {
                ier &= ~I8250_IER_TBE;
            }
        }
        else {
            ier &= ~I8250_IER_MSI;
        }

        INT_LOCK(level);

        (*pChan->inByte)(pChan->data);
        if ((options & CREAD) != 0x00) {
            ier |= I8250_IER_RXRDY;
        }

        if (pChan->channelMode == SIO_MODE_INT) {
            (*pChan->outByte)(pChan->ier, ier);
        }

        pChan->options = options;

        INT_UNLOCK(level);
    }
}

/******************************************************************************
 *
 * i8250Ioctl - Serial device input/output control
 *
 * RETURNS: OK or error code
 */

LOCAL int  i8250Ioctl (
    SIO_CHAN *  pSioChan,
    int         req,
    ARG         arg
    ) {
    int           status;
    int           i;
    int           level;
    int8_t        baudH;
    int8_t        baudL;
    int8_t        lcr;
    I8250_CHAN *  pChan = (I8250_CHAN *) pSioChan;

    switch (req) {
        case SIO_BAUD_SET:
            if (i8250BaudSet(pChan, (int) arg) != OK) {
                status = EIO;
            }
            else {
                status = OK;
            }
            break;

        case SIO_BAUD_GET:
            INT_LOCK(level);

            lcr = (*pChan->inByte)(pChan->lcr);
            (*pChan->outByte)(pChan->lcr, (int8_t) (I8250_LCR_DLAB | lcr));
            baudH = (*pChan->inByte)(pChan->brdh);
            baudL = (*pChan->inByte)(pChan->brdl);
            (*pChan->outByte)(pChan->lcr, lcr);

            status = EIO;
            for (i = 0; i < NELEMENTS(baudTable); i++) {
                if (baudH == MSB(baudTable[i].preset) &&
                    baudL == LSB(baudTable[i].preset)) {

                    *((int *) arg) = baudTable[i].rate;
                    status = OK;
                }
            }

            INT_UNLOCK(level);
            break;

        case SIO_MODE_SET:
            if (i8250ModeSet(pChan, (int) arg) != OK) {
                status = EIO;
            }
            else {
                status = OK;
            }
            break;

        case SIO_MODE_GET:
            *((int *) arg) = pChan->channelMode;
            status = OK;
            break;

        case SIO_AVAIL_MODES_GET:
            *((int *) arg) = SIO_MODE_INT | SIO_MODE_POLL;
            status = OK;
            break;

        case SIO_HW_OPTS_SET:
            if (i8250OptSet(pChan, (int) arg) != OK) {
                status = EIO;
            }
            else {
                status = OK;
            }
            break;

        case SIO_HW_OPTS_GET:
            *((int *) arg) = pChan->options;
            status = OK;
            break;

        case SIO_OPEN:
            if ((pChan->options & HUPCL) == 0x00) {
                status = ERROR;
            }
            else {
                status = i8250Open(pChan);
            }
            break;

        case SIO_HUP:
            if ((pChan->options & HUPCL) == 0x00) {
                status = ERROR;
            }
            else {
                status = i8250Hup(pChan);
            }
            break;

        default:
            status = ENOSYS;
            break;
    }

    return status;
}

/******************************************************************************
 *
 * i8250TxStartup - Transmit startup
 *
 * RETURNS: OK
 */

LOCAL int  i8250TxStartup (
    SIO_CHAN *  pSioChan
    ) {
    int8_t        ier;
    int8_t        mask;
    I8250_CHAN *  pChan = (I8250_CHAN *) pSioChan;

    ier = I8250_IER_RXRDY;
    if (pChan->channelMode == SIO_MODE_INT) {
        if ((pChan->options & CLOCAL) != 0x00) {
            ier |= I8250_IER_TBE;
        }
        else {
            mask = ((*pChan->inByte)(pChan->msr)) & I8250_MSR_CTS;
            if ((mask & I8250_MSR_CTS) != 0x00) {
                ier |= (I8250_IER_TBE | I8250_IER_MSI);
            }
            else {
                ier |= (I8250_IER_MSI);
            }
        }

        (*pChan->outByte)(pChan->ier, ier);
    }

    return OK;
}

/******************************************************************************
 *
 * i8250CallbackInstall - Install ISR callback
 *
 * RETURNS: OK or error code
 */

LOCAL int  i8250CallbackInstall (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS    (*callback)(void *, ...),
    ARG         callbackArg
    ) {
    int           status;
    I8250_CHAN *  pChan = (I8250_CHAN *) pSioChan;

    switch (callbackType) {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar = callback;
            pChan->getTxArg  = callbackArg;
            status = OK;
            break;

        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar = callback;
            pChan->putRcvArg  = callbackArg;
            status = OK;
            break;

        default:
            status = ENOSYS;
            break;
    }

    return status;
}

/******************************************************************************
 *
 * i8250RxChar - Receive character from device
 *
 * RETURNS: OK or EAGAIN
 */

LOCAL int  i8250RxChar (
    SIO_CHAN *  pSioChan,
    char *      pChar
    ) {
    int           status;
    int8_t        lst;
    I8250_CHAN *  pChan = (I8250_CHAN *) pSioChan;

    lst = (*pChan->inByte)(pChan->lst);
    if ((lst & I8250_LSR_RXRDY) == 0x00) {
        status = EAGAIN;
    }
    else {
        *pChar = (*pChan->inByte)(pChan->data);
        status = OK;
    }

    return status;
}

/******************************************************************************
 *
 * i8250TxChar - Transmit character to device
 *
 * RETURNS: OK or EAGAIN
 */

LOCAL int  i8250TxChar (
    SIO_CHAN *  pSioChan,
    char        outChar
    ) {
    int           status;
    int8_t        lst;
    int8_t        msr;
    I8250_CHAN *  pChan = (I8250_CHAN *) pSioChan;

    lst = (*pChan->inByte)(pChan->lst);
    msr  = (*pChan->inByte)(pChan->msr);
    if ((lst & I8250_LSR_TEMT) == 0x00) {
        status = EAGAIN;
    }
    else {
        if ((pChan->options & CLOCAL) == 0x00) {
            if ((msr & I8250_MSR_CTS) != 0x00) {
                (*pChan->outByte)(pChan->data, outChar);
                status = OK;
            }
            else {
                status = EAGAIN;
            }
        }
        else {
            (*pChan->outByte)(pChan->data, outChar);
            status = OK;
        }
    }

    return status;
}

/******************************************************************************
 *
 * i8250Int - Interrupt sertice routine from serial device
 *
 * RETURNS: N/A
 */

void  i8250Int (
    I8250_CHAN *  pChan
    ) {
    char    outChar;
    int8_t  intId;
    int8_t  lineStatus;
    int8_t  ier;
    int8_t  msr;
    int     i = 0;

    ier = (*pChan->inByte)(pChan->ier);
    while (TRUE) {
        intId = ((*pChan->inByte)(pChan->iid) & I8250_IIR_MASK);
        if (intId == I8250_IIR_IP || (++i > I8250_IRR_READ_MAX)) {
            break;
        }

        intId &= 0x06;
        if (intId == I8250_IIR_SEOB) {
            lineStatus = (*pChan->inByte)(pChan->lst);
        }
        else if (intId == I8250_IIR_RBRF) {
            if (pChan->putRcvChar != NULL) {
                (*pChan->putRcvChar)(
                    pChan->putRcvArg,
                    (*pChan->inByte)(pChan->data)
                    );
            }
            else {
                (*pChan->inByte)(pChan->data);
            }
        }
        else if (intId == I8250_IIR_THRE) {
            if (pChan->getTxChar != NULL &&
                (*pChan->getTxChar)(pChan->getTxArg, &outChar) == OK) {

                (*pChan->outByte)(pChan->data, outChar);
            }
        }
        else if (intId == I8250_IRR_MSTAT) {
            msr  = (*pChan->inByte)(pChan->msr);
            ier |= (I8250_IER_RXRDY | I8250_IER_MSI);
            if ((msr & I8250_MSR_CTS) != 0x00 &&
                (msr & I8250_MSR_DCTS) != 0x00) {

                (*pChan->outByte)(pChan->ier, (I8250_IER_TBE | ier));
            }
            else {
                (*pChan->outByte)(pChan->ier, (ier & ~I8250_IER_TBE));
            }
        }
    }
}

