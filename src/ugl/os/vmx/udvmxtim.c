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

/* udvmxtim.c - Universal graphics library time library */

#include <vmx/tickLib.h>
#include "uglos.h"

int sysClockRateGet(void);

/******************************************************************************
 *
 * uglOSTimeStamp - Get current timestamp
 *
 * RETURNS: UGL_TIMESTAMP
 */

UGL_TIMESTAMP uglOSTimeStamp (
    void
    ) {
    UGL_TIMESTAMP timeStamp;

    timeStamp = ((tickGet() * 1000) / sysClockRateGet());

    return timeStamp;
}

