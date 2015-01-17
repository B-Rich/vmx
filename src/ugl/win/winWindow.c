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

/* winWindow.c - Universal graphics library window library */

#include <string.h>
#include "uglWin.h"
#include "private/uglWinP.h"

/* Locals */

UGL_LOCAL UGL_STATUS  winDefaultMsgHandler (
    WIN_ID     winId,
    WIN_MSG *  pMsg
    );

UGL_LOCAL UGL_VOID  winClassInit (
    UGL_WINDOW *  pWindow
    );

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
    ) {
    WIN_CLASS *   pClass;
    UGL_WINDOW *  pWindow;
    UGL_SIZE      sz;

    if (classId == UGL_NULL) {
        pClass = winClassLookup(winRootClassName);
    }
    else {
        pClass = classId;
    }

    if (pClass != UGL_NULL) {
        sz = pClass->windowSize;
    }
    else {
        sz = sizeof(UGL_WINDOW) + appDataSize;
    }
        
    pWindow = (UGL_WINDOW *) UGL_CALLOC(1, sz);
    if (pWindow != UGL_NULL) {
        pWindow->pClass = pClass;
        pWindow->pApp   = appId;

        /* Setup window rectangle */
        pWindow->rect.left   = x;
        pWindow->rect.right  = pWindow->rect.left + width - 1;
        pWindow->rect.top    = y;
        pWindow->rect.bottom = pWindow->rect.top + height - 1;

        /* Setup window attributes and state */
        pWindow->attributes = attributes | WIN_ATTRIB_CLIP_CHILDREN;
        pWindow->state      = WIN_STATE_HIDDEN;

        /* Setup window data pointers */
        if (appDataSize > 0) {
            pWindow->pAppData= &pWindow[1];

            if (pAppData != UGL_NULL) {
                memcpy(pWindow->pAppData, pAppData, appDataSize);
            }

            pWindow->pClassData = (UGL_INT8 *) pWindow->pAppData + appDataSize;
        }
        else {
            pWindow->pClassData = &pWindow[1];
        }

        /* Copy hiearcy of class data */
        while (pClass != UGL_NULL) {
            memcpy((UGL_INT8 *) pWindow->pClassData + pClass->dataOffset,
                   pClass->pDefaultData,
                   pClass->dataSize
                   );
            pClass = pClass->pParent;
        }

        /* Setup window regions */
        uglRegionInit(&pWindow->paintersRegion);
        uglRegionInit(&pWindow->visibleRegion);
        uglRegionInit(&pWindow->dirtyRegion);

        /* Setup callbacks */
        uglCbAddArray(&pWindow->callbackList, (UGL_CB_ITEM *) pCallbackArray);

        /* Add window to application resource list */
        uglOSLock(appId->lockId);
        uglListAdd(&appId->resourceList, &pWindow->resource.node);
        uglOSUnlock(appId->lockId);

        /* Initialize window class */
        winClassInit(pWindow);

        /* Send window create message */
        winSend(pWindow, MSG_CREATE, UGL_NULL, 0);

        /* Create window frame */
        if ((attributes & WIN_ATTRIB_FRAMED) != 0x00) {

            /* TODO */
        }
    }

    return pWindow;
}

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
    ) {
    WIN_MSG  msg;

    /* Compile message */
    msg.type  = msgType;
    msg.winId = winId;
    if (pMsgData != UGL_NULL) {
        memcpy(msg.data.uglData.reserved, pMsgData, dataSize);
    }

    return winMsgSend(winId, &msg);
}

/******************************************************************************
 *
 * winMsgSend - Send message to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMsgSend (
    WIN_ID     winId,
    WIN_MSG *  pMsg
    ) {
    UGL_STATUS  status;

    if (winId == UGL_NULL || pMsg == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pMsg->winId = winId;
        status = uglCbListExecute(
            &winId->callbackList,
            winId,
            (UGL_MSG *) pMsg,
            winId->pAppData
            );
        if (status == UGL_STATUS_OK) {
            winMsgHandle(winId, UGL_NULL, pMsg);
        }
    }

    return status;
}

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
    ) {
    UGL_STATUS status;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (classId == UGL_NULL) {
            status = winDefaultMsgHandler(winId, pMsg);
        }
        else {
            status = (*classId->pMsgHandler) (
                winId,
                classId,
                pMsg,
                (UGL_INT8 *) winId->pClassData + classId->dataOffset
                );
        }
    }

    return status;
}

/******************************************************************************
 *
 * winClassInit - Initialize class hierarcy for window
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  winClassInit (
    UGL_WINDOW *  pWindow
    ) {
    WIN_MSG      msg;
    WIN_CLASS *  pClass = pWindow->pClass;

    if (pClass != UGL_NULL) {

        if (pWindow->pParent != UGL_NULL) {
            winClassInit(pWindow->pParent);
        }

        if (pClass->useCount == 0) {
            msg.type  = MSG_CLASS_INIT;
            msg.winId = pWindow;

            (*pClass->pMsgHandler) (
                pWindow,
                pClass,
                &msg,
                (UGL_INT8 *) pWindow->pClassData + pClass->dataOffset
                );
        }

        pClass->useCount++;
    }
}
 
/******************************************************************************
 *
 * winDefaultMsgHandler - Window default message handler
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winDefaultMsgHandler (
    WIN_ID     winId,
    WIN_MSG *  pMsg
    ) {

    /* TODO */

    return UGL_STATUS_ERROR;
}

