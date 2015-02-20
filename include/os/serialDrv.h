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

/* serialDrv.h - Serial driver header */

#ifndef _serialDrv_h
#define _serialDrv_h

/* Includes */

#include <os/tyLib.h>
#include <os/sioLib.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct {
    DEV_HEADER      devHeader;
    TY_DEV          tyDev;
    SIO_CHAN *      pChan;
    VOIDFUNCPTR     isr;
} SERIAL_DEV;

/* Functions */

/******************************************************************************
 * serialDrvInit- Install serial device
 *
 * RETURNS: OK or ERROR
 */

STATUS serialDrvInit(
    void
    );

/*******************************************************************************
 * serialDevCreate - Create serial device
 *
 * RETURNS: OK or ERROR
 */

STATUS serialDevCreate(
    char *name,
    SIO_CHAN *  pChan
    );

/******************************************************************************
 * serialDevDelete - Delete serial device
 *
 * RETURNS: OK or ERROR
 */

STATUS serialDevDelete(
    char *name
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _serialDrv_h */

