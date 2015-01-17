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

/* winApp.h - Universal graphics library application context header */

#ifndef _winApp_h
#define _winApp_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * winAppMsgPost - Post message to application queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS  winAppMsgPost (
    WIN_APP_ID   appId,
    WIN_MSG *    pMsg,
    UGL_TIMEOUT  timeout
    );

/******************************************************************************
 *
 * winAppPost - Send message to application
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS  winAppPost (
    WIN_APP_ID    appId,
    UGL_MSG_TYPE  msgType,
    void *        pMsgData,
    UGL_SIZE      dataSize,
    UGL_TIMEOUT   timeout
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _winApp_h */

