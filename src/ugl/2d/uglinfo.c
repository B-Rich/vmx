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

/* uglinfo.c - Get driver info */

#include "ugl.h"

/******************************************************************************
 *
 * uglInfo - Get driver info
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  uglInfo (
    UGL_DEVICE_ID  devId,
    UGL_INFO_REQ   infoReq,
    void *         pInfo
    ) {
    UGL_STATUS  status;

    if ((devId == UGL_NULL) || (pInfo == UGL_NULL)) {
        status = UGL_STATUS_ERROR;
    }
    else {
        uglOSLock(devId->lockId);
        status = (*devId->info) (devId, infoReq, pInfo);
        uglOSUnlock(devId->lockId);
    }

    return status;
}

