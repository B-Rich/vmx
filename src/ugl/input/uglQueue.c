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

/* uglQueue.c - Universal graphics library message queue */

#include <string.h>
#include "ugl.h"
#include "uglos.h"
#include "uglMsg.h"

/* Types */
typedef struct ugl_msg_queue {
    UGL_OS_MSG_Q_ID     osQId;
    UGL_MSG             peekMsg;
    UGL_UINT32          useCount;
} UGL_MSG_QUEUE;

/******************************************************************************
 *
 * uglMsgQCreate - Create message queue
 *
 * RETURNS: UGL_MSG_Q_ID or UGL_NULL
 */

UGL_MSG_Q_ID uglMsgQCreate (
    UGL_SIZE  queueSize
    ) {
    UGL_MSG_Q_ID qId;

    qId = (UGL_MSG_Q_ID) UGL_CALLOC(1, sizeof(UGL_MSG_QUEUE));
    if (qId != UGL_NULL) {
        qId->osQId = uglOSMsgQCreate(queueSize, sizeof(UGL_MSG));
        if (qId->osQId == UGL_NULL) {
            UGL_FREE(qId);
            qId = UGL_NULL;
        }
    }

    return qId;
}

/******************************************************************************
 *
 * uglMsgQDestroy - Destroy message queue
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglMsgQDestroy (
    UGL_MSG_Q_ID  qId
    ) {
    UGL_STATUS  status;

   if (qId == UGL_NULL || qId->useCount > 0) {
       status = UGL_STATUS_ERROR;
    }
    else {
        status = uglOSMsgQDelete(qId->osQId);
    }

    UGL_FREE(qId);

    return status;
}

/******************************************************************************
 *
 * uglMsgQPost - Post message to message queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS uglMsgQPost (
    UGL_MSG_Q_ID  qId,
    UGL_MSG      *pMsg,
    UGL_TIMEOUT   timeout
    ) {

    return uglOSMsgQPost(qId->osQId, timeout, pMsg, sizeof(UGL_MSG));
}

/******************************************************************************
 *
 * uglMsgQGet - Get message from message queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS uglMsgQGet (
    UGL_MSG_Q_ID  qId,
    UGL_MSG      *pMsg,
    UGL_TIMEOUT   timeout
    ) {
    UGL_STATUS  status;
    UGL_MSG    *pPeekMsg;

    if (qId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pPeekMsg = &qId->peekMsg;
        if (pMsg == UGL_NULL) {
            pPeekMsg->type = 0;
            status = UGL_STATUS_ERROR;
        }
        else {
            if (pPeekMsg->type != 0) {
                memcpy(pMsg, pPeekMsg, sizeof(UGL_MSG));
                pPeekMsg->type = 0;
                status = UGL_STATUS_OK;
            }
            else {
                status = uglOSMsgQGet(qId->osQId, timeout,
                                      pMsg, sizeof(UGL_MSG));
            }

            /* Compress pointer messages */
            if (status == UGL_STATUS_OK &&
                pMsg->type == MSG_POINTER &&
                pMsg->data.pointer.buttonChange == 0) {

                while (uglOSMsgQGet(qId->osQId, UGL_NO_WAIT, pPeekMsg,
                                    sizeof(UGL_MSG)) == UGL_STATUS_OK &&
                       pPeekMsg->type == MSG_POINTER &&
                       pPeekMsg->data.pointer.buttonChange == 0) {

                    pMsg->data.pointer.delta.x +=
                        pPeekMsg->data.pointer.delta.x;
                    pMsg->data.pointer.delta.y +=
                        pPeekMsg->data.pointer.delta.y;
                }
            }
        }
    }

    return status;
}

