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

/* winUtils.c - Universal graphics library window utility library */

#include <stdlib.h>
#include <string.h>
#include "uglWin.h"
#include "private/uglWinP.h"

/* Locals */

UGL_LOCAL UGL_STATUS  winDefaultMsgHandler (
    WIN_ID     winId,
    WIN_MSG *  pMsg
    );

/******************************************************************************
 *
 * winCount - Get number of child windows
 *
 * RETURNS: Number of child windows or 0
 */

UGL_SIZE  winCount (
    WIN_ID  winId
    ) {
    UGL_SIZE  count;

    if (winId == UGL_NULL) {
        count = 0;
    }
    else {
        count = uglListCount(&winId->childList);
    }

    return count;
}

/******************************************************************************
 *
 * winFirst - Get window first child
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winFirst (
    WIN_ID  winId
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListFirst(&winId->childList);
    }

    return pWindow;
}

/******************************************************************************
 *
 * winLast - Get window last child
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winLast (
    WIN_ID  winId
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListLast(&winId->childList);
    }

    return pWindow;
}

/******************************************************************************
 *
 * winNext - Get next window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winNext (
    WIN_ID  winId
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListNext(&winId->node);
    }

    return pWindow;
}

/******************************************************************************
 *
 * winPrev - Get previous window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winPrev (
    WIN_ID  winId
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListPrev(&winId->node);
    }

    return pWindow;
}

/******************************************************************************
 *
 * winNth - Get n:th window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winNth (
    WIN_ID   winId,
    UGL_ORD  n
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListNth(&winId->childList, n);
    }

    return pWindow;
}

/******************************************************************************
 *
 * winLock - Get excluse access to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winLock (
    WIN_ID  winId
    ) {
    UGL_STATUS  status;

    if (winId == UGL_NULL) {
       status = UGL_STATUS_ERROR;
    }
    else {
        status = uglOSLock(winId->pApp->pWinMgr->lockId);
    }

    return status;
}

/******************************************************************************
 *
 * winUnlock - Give up excluse access to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winUnlock (
    WIN_ID  winId
    ) {
    UGL_STATUS  status;

    if (winId == UGL_NULL) {
       status = UGL_STATUS_ERROR;
    }
    else {
        status = uglOSUnlock(winId->pApp->pWinMgr->lockId);
    }

    return status;
}

/******************************************************************************
 *
 * winMgrGet - Get window manager for window
 *
 * RETURNS: Pointer to window manager or UGL_NULL
 */

WIN_MGR_ID  winMgrGet (
    WIN_ID  winId
    ) {
    WIN_MGR_ID  winMgrId;

    if (winId == UGL_NULL || (winId->state & WIN_STATE_MANAGED) == 0x00) {
        winMgrId = UGL_NULL;
    }
    else {
        winMgrId = winId->pApp->pWinMgr;
    }

    return winMgrId;
}

/******************************************************************************
 *
 * winWindowToScreen - Convert window coordinates to screen coordinates
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winWindowToScreen (
    WIN_ID       winId,
    UGL_POINT *  pPoints,
    UGL_SIZE     numPoints
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pWindow;
    UGL_ORD       i;
    UGL_ORD       dx;
    UGL_ORD       dy;

    if (winId == UGL_NULL || pPoints == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pWindow = winId;
        dx = 0;
        dy = 0;

        while (pWindow != UGL_NULL) {
            dx += pWindow->rect.left;
            dy += pWindow->rect.top;

            /* Advance */
            pWindow = pWindow->pParent;
        }

        for (i = 0; i < numPoints; i++) {
            UGL_POINT_MOVE(pPoints[i], dx, dy);
        }

        status = UGL_STATUS_ERROR;
    }

    return status;
}

/******************************************************************************
 *
 * winWindowRectToScreen - Convert window rectangles to screen coordinates
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winWindowRectToScreen (
    WIN_ID       winId,
    UGL_RECT *   pRects,
    UGL_SIZE     numRects
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pWindow;
    UGL_ORD       i;
    UGL_ORD       dx;
    UGL_ORD       dy;

    if (winId == UGL_NULL || pRects == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pWindow = winId;
        dx = 0;
        dy = 0;

        while (pWindow != UGL_NULL) {
            dx += pWindow->rect.left;
            dy += pWindow->rect.top;

            /* Advance */
            pWindow = pWindow->pParent;
        }

        for (i = 0; i < numRects; i++) {
            UGL_RECT_MOVE(pRects[i], dx, dy);
        }

        status = UGL_STATUS_ERROR;
    }

    return status;
}
/******************************************************************************
 *
 * winDrawRectGet - Get window drawing rectangle
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDrawRectGet (
    WIN_ID      winId,
    UGL_RECT *  pRect
    ) {
    UGL_STATUS  status;

    if (winId == UGL_NULL || pRect == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
       pRect->left   = 0;
       pRect->top    = 0;
       pRect->right  = winId->rect.right - winId->rect.left;
       pRect->bottom = winId->rect.bottom - winId->rect.top;
    }

    return status;
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
 * winDefaultMsgHandler - Window default message handler
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winDefaultMsgHandler (
    WIN_ID     winId,
    WIN_MSG *  pMsg
    ) {
    UGL_STATUS  status;
    WIN_MSG     msg;
    WIN_MGR *   pWinMgr = winId->pApp->pWinMgr;

    /* Execute window manager callback list */
    status = uglCbListExecute(
        &pWinMgr->callbackList,
        winId,
        (UGL_MSG *) pMsg,
        winId->pAppData
        );
    if (status == UGL_STATUS_OK) {
        switch (pMsg->type) {
            case MSG_KEYBOARD:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_POINTER:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_PTR_BTN1_DOWN:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_PTR_ENTER:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_ATTACH:
                if ((winId->attributes & WIN_ATTRIB_ROOT) != 0x00) {
                    WIN_ID    childId = pMsg->data.attachId;
                    WIN_ID    nextId = winNext(childId);
                    UGL_BOOL  foundActive = UGL_FALSE;

                    while (nextId != UGL_NULL) {
                       if (winIsActive(nextId) == UGL_TRUE) {
                           foundActive = UGL_TRUE;
                           status = UGL_STATUS_OK;
                           break;
                       }

                       /* Advance */
                       nextId = winNext(nextId);
                    }

                    if (foundActive == UGL_FALSE) {
                        status = winActivate(childId);
                    }
                }
                break;

            case MSG_ACTIVATE:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_CLOSE:
                if ((winId->attributes & WIN_ATTRIB_FRAME) != 0x00) {
                    status = winMsgSend(winFirst(winId), pMsg);
                }
                else {
                    status = winDestroy(winId);
                }
                break;

            case MSG_EXPOSE:
                msg.data.draw.gcId = winDrawStart(winId, UGL_NULL, UGL_TRUE);
                if ((winId->attributes & WIN_ATTRIB_DOUBLE_BUFFER) == 0x00) {
                    msg.type = MSG_DRAW;
                    winDrawRectGet(winId, &msg.data.draw.rect);
                    msg.data.draw.displayId = winId->pApp->pWinMgr->pDisplay;
                    winMsgSend(winId, &msg);
                }
                status = winDrawEnd(winId, msg.data.draw.gcId, UGL_TRUE);
                break;

            case MSG_RECT_CHANGING:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_RECT_CHILD_CHANGING:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_RECT_CHANGED:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_FRAME_MAXIMIZE:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_FRAME_MINIMIZE:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_FRAME_RESTORE:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            case MSG_ZPOS_CHILD_CHANGING:
                /* TODO */
                status = UGL_STATUS_ERROR;
                break;

            default:
                status = UGL_STATUS_ERROR;
                break;
        }
    }

    return status;
}

