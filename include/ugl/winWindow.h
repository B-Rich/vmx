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

/* winWindow.h - Universal graphics librarye window header */

#ifndef _winWindow_h
#define _winWindow_h

#include "ugl.h"
#include "winTypes.h"
#include "winMsg.h"
#include "uglregn.h"

/* Defines */

/* Window attributes */
#define WIN_ATTRIB_CLIP_CHILDREN    0x00000001
#define WIN_ATTRIB_DBL_CLICK        0x00000002
#define WIN_ATTRIB_DISABLED         0x00000004
#define WIN_ATTRIB_DOUBLE_BUFFER    0x00000008
#define WIN_ATTRIB_FRAMED           0x00000010
#define WIN_ATTRIB_KBD_INPUT        0x00000020
#define WIN_ATTRIB_NO_ACTIVATE      0x00000040
#define WIN_ATTRIB_NO_CLOSE         0x00000080
#define WIN_ATTRIB_NO_KEYBOARD      0x00000100
#define WIN_ATTRIB_NO_MAXIMIZE      0x00000200
#define WIN_ATTRIB_NO_MINIMIZE      0x00000400
#define WIN_ATTRIB_NO_MOVE          0x00000800
#define WIN_ATTRIB_NO_POINTER       0x00001000
#define WIN_ATTRIB_NO_RESIZE        0x00002000
#define WIN_ATTRIB_OFF_SCREEN       0x00004000
#define WIN_ATTRIB_TOPMOST          0x00008000
#define WIN_ATTRIB_TRANSPARENT      0x00010000

/* Window states */
#define WIN_STATE_ACTIVE            0x00000001
#define WIN_STATE_DIRTY             0x00000002
#define WIN_STATE_HIDDEN            0x00000004
#define WIN_STATE_MANAGED           0x00000008
#define WIN_STATE_MAXIMIZED         0x00000010
#define WIN_STATE_MINIMIZED         0x00000020

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef UGL_STATUS  (WIN_CB) (
    WIN_ID     winId,
    WIN_MSG *  pMsg,
    void *     pData,
    void *     pParam
    );

typedef struct win_cb_item {
    UGL_UINT32  filterMin;
    UGL_UINT32  filterMax;
    WIN_CB *    pCallback;
    void *      pParam;
} WIN_CB_ITEM;

/******************************************************************************
 *
 * winCreate - Create window
 *
 * RETURNS: WIN_ID or UGL_NULL
 */

WIN_ID  winCreate (
    WIN_APP_ID           appId,
    WIN_CLASS_ID         classId,
    UGL_UINT32           attributes,
    UGL_POS              x,
    UGL_POS              y,
    UGL_SIZE             width,
    UGL_SIZE             height,
    void *               pAppData,
    UGL_SIZE             appDataSize,
    const WIN_CB_ITEM *  pCallbackArray
    );

/******************************************************************************
 *
 * winSend - Prepare and send message to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winSend (
    WIN_ID        winId,
    UGL_MSG_TYPE  msgType,
    const void *  pMsgData,
    UGL_SIZE      dataSize
    );

/******************************************************************************
 *
 * winMsgSend - Send message to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMsgSend (
    WIN_ID     winId,
    WIN_MSG *  pMsg
    );

/******************************************************************************
 *
 * winMsgHandle - Handle window message
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMsgHandle (
    WIN_ID        winId,
    WIN_CLASS_ID  classId,
    WIN_MSG *     pMsg
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _winWindow_h */

