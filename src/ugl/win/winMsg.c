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

/* winMsg.c - Universal graphics library window message library */

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
    UGL_STATUS   status;
    WIN_CLASS *  pClass;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (classId == UGL_NULL) {
            pClass = winId->pClass;
        }
        else {
            pClass = classId->pParent;
        }

        if (pClass == UGL_NULL) {
            status = winDefaultMsgHandler(winId, pMsg);
        }
        else {
            status = (*pClass->pMsgHandler) (
                winId,
                pClass,
                pMsg,
                (UGL_INT8 *) winId->pClassData + pClass->dataOffset
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
                if ((winId->attributes & WIN_ATTRIB_FRAME) != 0x00) {
                    UGL_WINDOW *  pWindow = (UGL_WINDOW *) winFirst(winId);

                    /* Compensate for frame */
                    pMsg->data.rectChanging.newRect.left += pWindow->rect.left;
                    pMsg->data.rectChanging.newRect.top += pWindow->rect.top;
                    pMsg->data.rectChanging.newRect.right -=
                        UGL_RECT_WIDTH(winId->rect) - pWindow->rect.right - 1;
                    pMsg->data.rectChanging.newRect.bottom -=
                        UGL_RECT_HEIGHT(winId->rect) - pWindow->rect.bottom - 1;

                    /* Send message so window can determine size */
                    winMsgSend(pWindow, pMsg);

                    /* Compensate for frame */
                    pMsg->data.rectChanging.newRect.left -= pWindow->rect.left;
                    pMsg->data.rectChanging.newRect.top -= pWindow->rect.top;
                    pMsg->data.rectChanging.newRect.right +=
                        UGL_RECT_WIDTH(winId->rect) - pWindow->rect.right - 1;
                    pMsg->data.rectChanging.newRect.bottom +=
                        UGL_RECT_HEIGHT(winId->rect) - pWindow->rect.bottom - 1;
                }
                status = UGL_STATUS_OK;
                break;

            case MSG_RECT_CHILD_CHANGING: {
                UGL_WINDOW *  pChild =
                    (UGL_WINDOW *) pMsg->data.rectChildChanging.childId;

                if ((pChild->attributes & WIN_ATTRIB_FRAMED) == 0x00) {

                    /* Send message so window can determine size */
                    pMsg->type = MSG_RECT_CHANGING;
                    winDrawRectGet(winId, &pMsg->data.rectChanging.maxRect);
                    winMsgSend(pChild, pMsg);

                    /* Restore message */
                    pMsg->type = MSG_RECT_CHILD_CHANGING;
                    pMsg->data.rectChildChanging.childId = pChild;
                }

                status = UGL_STATUS_OK;
                } break;

            case MSG_RECT_CHANGED:
                if (UGL_RECT_WIDTH(pMsg->data.rectChanged.newRect) !=
                    UGL_RECT_WIDTH(pMsg->data.rectChanged.oldRect) ||
                    UGL_RECT_HEIGHT(pMsg->data.rectChanged.newRect) !=
                    UGL_RECT_HEIGHT(pMsg->data.rectChanged.oldRect)) {

                    UGL_WINDOW *  pChild = winFirst(winId);

                    /* Send message to all children */
                    while (pChild != UGL_NULL) {
                        winRectSet(pChild, UGL_NULL);
                    }
                }
                else {
                    UGL_WINDOW *  pChild = winFirst(winId);

                    /* Send message to window */
                    if (pChild != UGL_NULL &&
                        (pChild->attributes & WIN_ATTRIB_FRAMED) != 0x00) {

                        WIN_MSG  msg;
                        UGL_POS  dx = pMsg->data.rectChanged.newRect.left -
                                      pMsg->data.rectChanged.oldRect.left;
                        UGL_POS  dy = pMsg->data.rectChanged.newRect.top -
                                      pMsg->data.rectChanged.oldRect.top;

                        msg.type = MSG_RECT_CHANGED;
                        winRectGet(pChild, &msg.data.rectChanged.newRect);
                        UGL_RECT_MOVE(msg.data.rectChanged.oldRect, dx, dy);
                        winMsgSend(pChild, &msg);
                    }
                }
                break;

            case MSG_FRAME_MAXIMIZE:
                winId->state |= WIN_STATE_MAXIMIZED;
                status = UGL_STATUS_OK;
                break;

            case MSG_FRAME_MINIMIZE:
                winId->state |= WIN_STATE_MINIMIZED;
                status = UGL_STATUS_OK;
                break;

            case MSG_FRAME_RESTORE:
                if ((winId->state & WIN_STATE_MINIMIZED) != 0x00) {
                    winId->state &= ~WIN_STATE_MINIMIZED;
                }
                else {
                    winId->state &= ~WIN_STATE_MAXIMIZED;
                }
                status = UGL_STATUS_OK;
                break;

            case MSG_ZPOS_CHILD_CHANGING:
                if ((winId->attributes & WIN_ATTRIB_ROOT) != 0x00) {
                    winMsgSend(pMsg->data.zPos.changeId, pMsg);

                    /* Move window in hieararchy until desired position */
                    if (pMsg->data.zPos.newPos < pMsg->data.zPos.oldPos) {
                        UGL_ORD       oldPos  = pMsg->data.zPos.oldPos;
                        UGL_ORD       newPos  = pMsg->data.zPos.newPos;
                        UGL_WINDOW *  pWindow =
                            (UGL_WINDOW *) winNth(winId, -newPos);

                        pMsg->type = MSG_ZPOS_CHANGING;
                        while (pWindow != UGL_NULL &&
                               newPos < pMsg->data.zPos.oldPos) {

                            pMsg->data.zPos.oldPos = newPos;
                            pMsg->data.zPos.newPos = newPos + 1;
                            winMsgSend(pWindow, pMsg);

                            if (pMsg->data.zPos.newPos >= newPos + 1) {
                                break;
                            }

                            /* Advance */
                            newPos++;
                            pWindow = winPrev(pWindow);
                        }

                        /* Restore message */
                        pMsg->type = MSG_ZPOS_CHANGING;
                        pMsg->data.zPos.newPos = newPos;
                        pMsg->data.zPos.oldPos = oldPos;
                    }
                }
                status = UGL_STATUS_OK;
                break;

            default:
                status = UGL_STATUS_ERROR;
                break;
        }
    }

    return status;
}

