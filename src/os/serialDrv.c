/******************************************************************************
 *   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 *   This file is part of Real VMX.
 *   Copyright (C) 2013 Surplus Users Ham Society
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

/* serivalDrv.c - Serial device driver */

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <vmx.h>
#include <arch/intArchLib.h>
#include <os/serialDrv.h>

/* Locals */
LOCAL int serialDrvNum = ERROR;

LOCAL int serialCreate(
    SERIAL_DEV *pSerialDev,
    const char *name,
    mode_t mode,
    const char *symlink
    );

LOCAL int serialDelete(
    SERIAL_DEV *pSerialDev,
    const char *name,
    mode_t mode
    );

LOCAL int serialOpen(
    SERIAL_DEV *pSerialDev,
    const char *name,
    int flags,
    mode_t mode
    );

LOCAL int serialClose(
    SERIAL_DEV *pSerialDev
    );

LOCAL int serialRead(
    SERIAL_DEV *pSerialDev,
    char *buf,
    unsigned int maxBytes
    );

LOCAL int serialWrite(
    SERIAL_DEV *pSerialDev,
    char *buf,
    int maxBytes
    );

LOCAL serialIoctl(
    SERIAL_DEV *pSerialDev,
    int func, ARG arg
    );

/* Functions */

/******************************************************************************
 * serialDrvInit- Install serial device
 *
 * RETURNS: OK or ERROR
 */

STATUS serialDrvInit(
    void
    )
{
    STATUS status;

    if (serialDrvNum == ERROR)
    {
        serialDrvNum = iosDrvInstall(
                         (FUNCPTR) serialCreate,
                         (FUNCPTR) serialDelete,
                         (FUNCPTR) serialOpen,
                         (FUNCPTR) serialClose,
                         (FUNCPTR) serialRead,
                         (FUNCPTR) serialWrite,
                         (FUNCPTR) serialIoctl
                         );
        if (serialDrvNum == ERROR)
        {
            status = ERROR;
        }
        else
        {
            status =  OK;
        }
    }
    else
    {
        status = OK;
    }

    return status;
}

/*******************************************************************************
 * serialDevCreate - Create serial device
 *
 * RETURNS: OK or ERROR
 */

STATUS serialDevCreate(
    char *name,
    SIO_CHAN *pChan,
    int readBufferSize,
    int writeBufferSize
    )
{
    STATUS status;
    SERIAL_DEV *pSerialDev;

    /* Not callable from interrupts */
    if (INT_RESTRICT() != OK)
    {
        status = ERROR;
    }
    else
    {
        /* If invalid driver number */
        if (serialDrvNum == ERROR)
        {
            status = ERROR;
        }
        else
        {
            /* Allocate memory for serial */
            pSerialDev = (SERIAL_DEV *) malloc(sizeof(SERIAL_DEV));
            if (pSerialDev == NULL)
            {
                status = ERROR;
            }
            else
            {
                /* Setup sio channel */
                pSerialDev->pChan = pChan;

                /* Setup driver */
                SIO_CALLBACK_INSTALL(
                    pChan,
                    SIO_CALLBACK_GET_TX_CHAR,
                    (FUNCPTR) tyIntTx,
                    &pSerialDev->tyDev
                    );

                SIO_CALLBACK_INSTALL(
                    pChan,
                    SIO_CALLBACK_PUT_RCV_CHAR,
                    (FUNCPTR) tyIntRd,
                    &pSerialDev->tyDev
                    );

                SIO_IOCTL(pChan, SIO_MODE_SET, (ARG) SIO_MODE_INT);

                /* Initialize typewriter */
                if (tyDevInit(
                    &pSerialDev->tyDev,
                    readBufferSize,
                    writeBufferSize,
                    (FUNCPTR) pChan->pDrvFuncs->txStartup
                    ) != OK)
                {
                    status = ERROR;
                }
                else
                {
                    /* Install driver */
                    if (iosDevAdd(
                            &pSerialDev->devHeader,
                            name,
                            serialDrvNum
                            ) != OK)
                    {
                        free(pSerialDev);
                        status = ERROR;
                    }
                    else
                    {
                        status = OK;
                    }
                }
            }
        }
    }

    return status;
}

/******************************************************************************
 * serialDevDelete - Delete serial device
 *
 * RETURNS: OK or ERROR
 */

STATUS serialDevDelete(
    char *name
    )
{
    STATUS status;

    SERIAL_DEV *pSerialDev;
    char *pTail;

    /* Not callable from interrupts */
    if (INT_RESTRICT() != OK)
    {
        status = ERROR;
    }
    else
    {
        /* If invalid driver number */
        if (serialDrvNum == ERROR)
        {
            status = ERROR;
        }
        else
        {
            /* Find device */
            pSerialDev = (SERIAL_DEV *) iosDevFind(name, &pTail);
            if (pSerialDev == NULL)
            {
                status = ERROR;
            }
            else
            {
                /* Delete typewriter */
                tyDevRemove(&pSerialDev->tyDev);

                /* Delete device */
                iosDevDelete(&pSerialDev->devHeader);

                /* Free struct */
                free(pSerialDev);

                status = OK;
            }
        }
    }

    return status;
}

/******************************************************************************
 * serialCreate - Create not supported
 *
 * RETURNS: ERROR
 */

LOCAL int serialCreate(
    SERIAL_DEV *pSerialDev,
    const char *name,
    mode_t mode,
    const char *symlink)
{
    return ERROR;
}

/******************************************************************************
 * serialDelete - Delete not supported
 *
 * RETURNS: ERROR
 */

LOCAL int serialDelete(
    SERIAL_DEV *pSerialDev,
    const char *name,
    mode_t mode
    )
{
    return ERROR;
}

/******************************************************************************
 * serialOpen - Open serial device
 *
 * RETURNS: Pointer to serial device or ERROR
 */

LOCAL int serialOpen(
    SERIAL_DEV *pSerialDev,
    const char *name,
    int flags,
    mode_t mode
    )
{
    int ret;

    if ((name != NULL) && (strlen(name) > 0))
    {
        errnoSet(S_ioLib_NO_DEVICE_NAME_IN_PATH);
        ret = ERROR;
    }
    else
    {
        ret = (int) pSerialDev;
    }

    return ret;
}

/*******************************************************************************
 * serialClose - Close serial
 *
 * RETURNS: OK or ERROR
 */

LOCAL int serialClose(
    SERIAL_DEV *pSerialDev
    )
{
    int ret;

    /* Check serial device */
    if (pSerialDev == NULL)
    {
        ret = ERROR;
    }
    else
    {
        ret = OK;
    }

    return ret;
}

/******************************************************************************
 * serialRead - Read from serial device
 *
 * RETURNS: Number of bytes read or ERROR
 */

LOCAL int serialRead(
    SERIAL_DEV *pSerialDev,
    char *buf,
    unsigned int maxBytes
    )
{
    return tyRead(&pSerialDev->tyDev, buf, maxBytes);
}

/******************************************************************************
 * serialWrite - Write to serial device
 *
 * RETURNS: Number of bytes written or ERROR
 */

LOCAL int serialWrite(
    SERIAL_DEV *pSerialDev,
    char *buf,
    int maxBytes
    )
{
#ifdef NOT_YET
    return tyWrite(&pSerialDev->tyDev, buf, maxBytes);
#else
    int  i;
    int  rv;
    int  count;

    i = 0;
    while (i < maxBytes) {
        rv = SIO_POLL_OUTPUT(pSerialDev->pChan, buf[i]);
#if 0
        if (rv == OK) {
            i++;
        }
        else if (rv == EAGAIN) {
            continue;
        }
        else {
            break;
        }
#else
        for (count = 0; count < 100; count++) {
            sysDelay();
        }
        i++;
#endif
    }

    return i;
#endif
}

/******************************************************************************
 * serialIoctl - Perform ioctl on serial device
 *
 * RETURNS: OK or ERROR
 */

LOCAL STATUS serialIoctl(
    SERIAL_DEV *pSerialDev,
    int func,
    ARG arg
    )
{
    return SIO_IOCTL(pSerialDev->pChan, func, arg);
}

