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

/* uglInput.c - Universal graphics library input library */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ugl.h"
#include "uglos.h"
#include "uglMsg.h"
#include "uglinput.h"

/* Defines */

#define UGL_INPUT_Q_SIZE    64

/* Types */

typedef struct ugl_input_service {
    UGL_LOCK_ID            lockId;
    UGL_TASK_ID            taskId;
    UGL_RECT               boundingRect;
    UGL_POINT              position;
    UGL_UINT32             modifiers;
    volatile UGL_UINT32    taskState;
    UGL_BOOL               autoLedControl;
    struct ugl_cb_list    *pCbList;
    struct ugl_msg_queue  *pDefaultQ;
    struct ugl_msg_queue  *pInputQ;
} UGL_INPUT_SERVICE;

/* Locals */

UGL_LOCAL UGL_VOID uglInputTask (
    UGL_INPUT_SERVICE_ID  srvId
    );

/******************************************************************************
 *
 * uglInputMsgPost - Post message to input queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS uglInputMsgPost (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_MSG              *pMsg
    ) {
    UGL_STATUS  status;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        /* TODO: Raw keyboard map */

        status = uglMsgQPost(srvId->pInputQ, pMsg, UGL_NO_WAIT);
    }

    return status;
}

/******************************************************************************
 *
 * uglInputMsgGet - Get message from input queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS uglInputMsgGet (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_MSG              *pMsg,
    UGL_TIMEOUT           timeout
    ) {

    return uglMsgQGet(srvId->pDefaultQ, pMsg, timeout);
}

/******************************************************************************
 *
 * uglInputServiceCreate - Create input service
 *
 * RETURNS: UGL_INPUT_SERVICE_ID or UGL_NULL
 */

UGL_INPUT_SERVICE_ID uglInputServiceCreate (
    UGL_RECT           *pInputRect,
    UGL_INPUT_CB_ITEM  *pInputCbArray
    ) {
    UGL_INPUT_SERVICE  *pSrv;

    pSrv = (UGL_INPUT_SERVICE *) UGL_CALLOC(1, sizeof(UGL_INPUT_SERVICE));
    if (pSrv != UGL_NULL) {
        memcpy(&pSrv->boundingRect, pInputRect, sizeof(UGL_RECT));
        pSrv->position.x = (pInputRect->left + pInputRect->right) / 2;
        pSrv->position.y = (pInputRect->top + pInputRect->bottom) / 2;
        pSrv->autoLedControl = UGL_TRUE;
        pSrv->lockId = uglOSLockCreate();

        if (pSrv->lockId == UGL_NULL) {
            UGL_FREE(pSrv);
            pSrv = UGL_NULL;
        }
        else {
            pSrv->pCbList = uglCbListCreate((UGL_CB_ITEM *) pInputCbArray);
            if (pSrv->pCbList == UGL_NULL) {
                uglOSLockDestroy(pSrv->lockId);
                UGL_FREE(pSrv);
                pSrv = UGL_NULL;
            }
            else {
                pSrv->pDefaultQ = uglMsgQCreate(UGL_INPUT_Q_SIZE);
                if (pSrv->pDefaultQ == UGL_NULL) {
                    uglCbListDestroy(pSrv->pCbList);
                    uglOSLockDestroy(pSrv->lockId);
                    UGL_FREE(pSrv);
                    pSrv = UGL_NULL;
                }
                else {
                    pSrv->pInputQ = uglMsgQCreate(UGL_INPUT_Q_SIZE);
                    if (pSrv->pInputQ == UGL_NULL) {
                        uglMsgQDestroy(pSrv->pDefaultQ);
                        uglCbListDestroy(pSrv->pCbList);
                        uglOSLockDestroy(pSrv->lockId);
                        UGL_FREE(pSrv);
                        pSrv = UGL_NULL;
                    }
                    else {
                        pSrv->taskId = uglOSTaskCreate(
                                           "tUglInput",
                                           (UGL_FPTR) uglInputTask,
                                           UGL_INPUT_TASK_PRIORITY,
                                           0,
                                           UGL_INPUT_TASK_STACK_SIZE,
                                           (UGL_ARG) pSrv,
                                           (UGL_ARG) 0,
                                           (UGL_ARG) 0,
                                           (UGL_ARG) 0,
                                           (UGL_ARG) 0);
                        if (pSrv->taskId == (UGL_TASK_ID) 0) {
                            uglMsgQDestroy(pSrv->pInputQ);
                            uglMsgQDestroy(pSrv->pDefaultQ);
                            uglCbListDestroy(pSrv->pCbList);
                            uglOSLockDestroy(pSrv->lockId);
                            UGL_FREE(pSrv);
                            pSrv = UGL_NULL;
                        }
                    }
               }
            }
        }
    }

    return pSrv;
}

/******************************************************************************
 *
 * uglInputTask - Input service task
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID uglInputTask (
    UGL_INPUT_SERVICE_ID  srvId
    ) {

    /* TODO */
}

