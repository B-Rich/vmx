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
#include <sys/time.h>
#include <sys/select.h>
#include "ugl.h"
#include "uglos.h"
#include "uglMsg.h"
#include "uglinput.h"

/* Defines */

#define UGL_INPUT_Q_SIZE           64

#define UGL_SERVICE_RUN             0
#define UGL_SERVICE_IDLE            1
#define UGL_SERVICE_DESTROYED       2
#define UGL_SERVICE_IDLE_REQ        3
#define UGL_SERVICE_RESUME_REQ      4
#define UGL_SERVICE_DESTROY_REQ     5

#define UGL_SERVICE_DELAY_CYCLE   100
#define UGL_SELECT_TIMEOUT       1000

/* Types */

typedef struct ugl_input_service {
    UGL_LOCK_ID            lockId;
    UGL_TASK_ID            taskId;
    UGL_RECT               boundingRect;
    UGL_POINT              position;
    UGL_UINT32             modifiers;
    volatile UGL_UINT32    taskState;
    UGL_BOOL               autoLedControl;
    UGL_KBD_MAP           *pKbdMap;
    struct ugl_cb_list    *pCbList;
    struct ugl_msg_queue  *pDefaultQ;
    struct ugl_msg_queue  *pInputQ;
    struct ugl_input_dev  *ppDevice[UGL_MAX_INPUT_DEVICES];
} UGL_INPUT_SERVICE;

/* Locals */

UGL_LOCAL UGL_VOID uglInputTask (
    UGL_INPUT_SERVICE_ID  srvId
    );

LOCAL UGL_STATUS uglInputServiceIdle(
    UGL_INPUT_SERVICE_ID  srvId
    );

LOCAL UGL_STATUS uglInputServiceResume(
    UGL_INPUT_SERVICE_ID  srvId
    );

UGL_LOCAL UGL_STATUS uglPtrMsgMap (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_MSG              *pMsg
    );

/******************************************************************************
 *
 * uglInputDevAdd - Add input device to input system
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputDevAdd (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_INPUT_DEV_ID      devId
    ) {
    UGL_STATUS  status;
    UGL_ORD     devNum = 0;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        uglOSLock(srvId->lockId);

        /* Find free slot in device list */
        while (srvId->ppDevice[devNum] != UGL_NULL &&
               devNum < UGL_MAX_INPUT_DEVICES) {
            devNum++;
        }

        if (devNum >= UGL_MAX_INPUT_DEVICES) {
            status = UGL_STATUS_ERROR;
        }
        else {
            srvId->ppDevice[devNum] = devId;
            devId->inputServiceId   = srvId;

            uglOSUnlock(srvId->lockId);
            status = UGL_STATUS_OK;
        }
    }

    return status;
}

/******************************************************************************
 *
 * uglInputDevRemove - Remove input device from input system
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputDevRemove (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_INPUT_DEV_ID      devId
    ) {
    UGL_STATUS  status;
    UGL_ORD     devNum = 0;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        uglInputServiceIdle(srvId);
        uglOSUnlock(srvId->lockId);

        /* Find device */
        while (srvId->ppDevice[devNum] != devId &&
               devNum < UGL_MAX_INPUT_DEVICES) {
           devNum++;
        }

        /* Remove device */
        if (devNum < UGL_MAX_INPUT_DEVICES) {
            srvId->ppDevice[devNum] = UGL_NULL;
            status = UGL_STATUS_OK;
        }
        else {
            status = UGL_STATUS_ERROR;
        }

        uglOSUnlock(srvId->lockId);
        uglInputServiceResume(srvId);
    }

    return status;
}

/******************************************************************************
 *
 * uglInputDevOpen - Open input device
 *
 * RETURNS: UGL_INPUT_DEV_ID or UGL_NULL
 */

UGL_INPUT_DEV_ID uglInputDevOpen (
    UGL_CHAR       *name,
    UGL_INPUT_DRV  *pDriver
    ) {

    return (*pDriver->open) (name, pDriver);
}

/******************************************************************************
 *
 * uglInputDevClose - Close input device
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputDevClose (
    UGL_INPUT_DEV_ID  devId
    ) {
    UGL_STATUS  status;

    if (devId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (devId->inputServiceId != UGL_NULL) {
            uglInputDevRemove(devId->inputServiceId, devId);
        }

        status = (*devId->pDriver->close) (devId);
    }

    return status;
}

/******************************************************************************
 *
 * uglInputDevControl - Input device control
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputDevControl (
    UGL_INPUT_DEV_ID  devId,
    UGL_DEVICE_REQ    request,
    void             *pArg
    ) {
    UGL_STATUS         status;
    UGL_INPUT_SERVICE *pSrv;
    UGL_RECT          *pRect;
    UGL_POINT         *pPoint;
    UGL_POINT          point;
    UGL_MSG            msg;
    UGL_UINT32         leds;

    if (devId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pSrv = devId->inputServiceId;

        switch(request) {
            case ICR_SET_PTR_POS:
                pPoint = (UGL_POINT *) pArg;

                if (pPoint->x < pSrv->boundingRect.left) {
                    pPoint->x = pSrv->boundingRect.left;
                }
                else if (pPoint->x > pSrv->boundingRect.right) {
                    pPoint->x = pSrv->boundingRect.right;
                }

                if (pPoint->y < pSrv->boundingRect.top) {
                    pPoint->y = pSrv->boundingRect.top;
                }
                else if (pPoint->y > pSrv->boundingRect.bottom) {
                    pPoint->y = pSrv->boundingRect.bottom;
                }

                if (pPoint->x != pSrv->position.x ||
                    pPoint->y != pSrv->position.y) {

                    msg.type                    = MSG_RAW_PTR;
                    msg.objectId                = UGL_NULL;
                    msg.data.rawPtr.deviceId    = devId;
                    msg.data.rawPtr.buttonState =
                        (pSrv->modifiers & UGL_PTR_BUTTON_MASK);
                    msg.data.rawPtr.isAbsolute  = UGL_TRUE;
                    memcpy(&msg.data.rawPtr.pos.absolute,
                           pPoint,
                           sizeof(UGL_POINT));
                    status = uglInputMsgPost(pSrv, &msg);
                    if (status == UGL_STATUS_OK) {
                        status = (*devId->pDriver->control) (
                                     devId,
                                     request,
                                     pArg
                                     );
                    }
                }
                else {
                    status = UGL_STATUS_OK;
                }
                break;

            case ICR_SET_PTR_CONSTRAINT:
                pRect = (UGL_RECT *) pArg;

                memcpy(&pSrv->boundingRect, pRect, sizeof(UGL_RECT));

                if (UGL_POINT_IN_RECT(pSrv->position, pSrv->boundingRect) ==
                    UGL_FALSE) {

                    memcpy(&point, &pSrv->position, sizeof(UGL_POINT));
                    status = uglInputDevControl(devId, ICR_SET_PTR_POS, &point);
                }
                else {
                    status = UGL_STATUS_OK;
                }
                break;

            case ICR_SET_AUTO_LED_CONTROL:
                pSrv->autoLedControl = *(UGL_BOOL *) pArg;
                status = UGL_STATUS_OK;
                break;

            case ICR_SET_LED_STATE:
                leds = *(UGL_UINT32 *) pArg;
                status = uglInputDevControl(devId, ICR_SET_LED, &leds);
                if (status == UGL_STATUS_OK) {
                    leds = ~leds;
                    status = uglInputDevControl(devId, ICR_CLEAR_LED, &leds);
                }
                break;

            default:
                status = (*devId->pDriver->control) (devId, request, pArg);
                break;
        }
    }

    return status;
}

/******************************************************************************
 *
 * uglInputCbAdd - Add callback to input service
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputCbAdd (
    UGL_INPUT_SERVICE_ID srvId,
    UGL_INT32            filterMin,
    UGL_INT32            filterMax,
    UGL_INPUT_CB        *pCallback,
    void                *pParam
    ) {
    UGL_STATUS  status;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = uglCbAdd(srvId->pCbList, filterMin, filterMax,
                          (UGL_CB *) pCallback, pParam);
    }

    return status;
}

/******************************************************************************
 *
 * uglInputCbAddArray - Add array of callbacks to input service
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputCbAddArray (
    UGL_INPUT_SERVICE_ID     srvId,
    const UGL_INPUT_CB_ITEM *pCbArray
    ) {
    UGL_STATUS  status;

    if (pCbArray == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = uglCbAddArray(srvId->pCbList, (UGL_CB_ITEM *) pCbArray);
    }

    return status;
}

/******************************************************************************
 *
 * uglInputCbRemove - Remove callback from input service
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputCbRemove (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_INPUT_CB         *pCallback
    ) {
    UGL_STATUS  status;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = uglCbRemove(srvId->pCbList, (UGL_CB *) pCallback);
    }

    return status;
}

/******************************************************************************
 *
 * uglInputCbRemoveArray - Remove array of callbacks from input service
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputCbRemoveArray (
    UGL_INPUT_SERVICE_ID     srvId,
    const UGL_INPUT_CB_ITEM *pCbArray
    ) {
    UGL_STATUS  status;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        uglCbRemoveArray(srvId->pCbList, (UGL_CB_ITEM *) pCbArray);
   }

    return status;
}

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
        if (pMsg->type == MSG_RAW_KBD) {
            /* TODO */
        }
        else if (pMsg->type == MSG_RAW_PTR) {
            uglPtrMsgMap(srvId, pMsg);
        }

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
 * uglInputServiceDestroy - Destroy input service
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputServiceDestroy (
    UGL_INPUT_SERVICE_ID  srvId
    ) {
    UGL_STATUS  status;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {

       /* Wait for shutdown */
       srvId->taskState = UGL_SERVICE_DESTROY_REQ;
       while (srvId->taskState != UGL_SERVICE_DESTROYED) {
           uglOSTaskDelay(UGL_SERVICE_DELAY_CYCLE);
       }

       if (srvId->pInputQ != UGL_NULL) {
           uglMsgQDestroy(srvId->pInputQ);
       }

       if (srvId->pDefaultQ != UGL_NULL) {
           uglMsgQDestroy(srvId->pDefaultQ);
       }

       if (srvId->pCbList != UGL_NULL) {
           uglCbListDestroy(srvId->pCbList);
       }

       uglOSLockDestroy(srvId->lockId);
       UGL_FREE(srvId);
       status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglInputServiceIdle - Put input service in idle mode
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

LOCAL UGL_STATUS uglInputServiceIdle(
    UGL_INPUT_SERVICE_ID  srvId
    ) {
    STATUS  status;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        srvId->taskState = UGL_SERVICE_IDLE_REQ;
        while (srvId->taskState != UGL_SERVICE_IDLE) {
            uglOSTaskDelay(UGL_SERVICE_DELAY_CYCLE);
        }

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglInputServiceResume - Resume input service from idle mode
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

LOCAL UGL_STATUS uglInputServiceResume(
    UGL_INPUT_SERVICE_ID  srvId
    ) {
    STATUS  status;

    if (srvId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        srvId->taskState = UGL_SERVICE_RESUME_REQ;
        while (srvId->taskState != UGL_SERVICE_RUN) {
            uglOSTaskDelay(UGL_SERVICE_DELAY_CYCLE);
        }

        status = UGL_STATUS_OK;
    }

    return status;
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
    UGL_STATUS     status;
    struct timeval timeout;
    struct timeval tmpTimeout;
    fd_set         readFds;
    fd_set         tmpReadFds;
    int            result;
    int            width;
    int            ern;
    UGL_ORD        i;
    UGL_INPUT_DEV *pDev;
    UGL_MSG        msg;
    UGL_MSG_Q_ID   qId;

    /* Timeout for select */
    timeout.tv_sec  = UGL_SELECT_TIMEOUT / 1000;
    timeout.tv_usec = (UGL_SELECT_TIMEOUT % 1000) * 1000;

    while (srvId->taskState != UGL_SERVICE_DESTROY_REQ) {

        FD_ZERO(&readFds);
        width = 0;

        uglOSLock(srvId->lockId);

        /* Select file descriptors for all input devices */
        for (i = 0; i < UGL_MAX_INPUT_DEVICES; i++) {

            if (srvId->ppDevice[i] != UGL_NULL && srvId->ppDevice[i]->fd > 0) {
                FD_SET(srvId->ppDevice[i]->fd, &readFds);
                if (srvId->ppDevice[i]->fd > width) {
                    width = srvId->ppDevice[i]->fd;
                }
            }
        }

        width++;

        uglOSUnlock(srvId->lockId);

        /* Perform select on input filedescriptors */
        ern = errno;
        result = select(width, &readFds, UGL_NULL, UGL_NULL, &timeout);
        if (result > 0) {

            /* Perform operation for all selected devices */
            for (i = 0; i < UGL_MAX_INPUT_DEVICES; i++) {
                pDev = srvId->ppDevice[i];

                if (pDev != UGL_NULL && FD_ISSET(pDev->fd, &readFds)) {

                    /* Perform read operation for selected device */
                    (*pDev->pDriver->control) (pDev, ICR_READ, UGL_NULL);

                    /* Perform additional selects without waiting */
                    tmpTimeout.tv_sec  = 0;
                    tmpTimeout.tv_usec = 0;

                    FD_ZERO(&tmpReadFds);
                    FD_SET(pDev->fd, &tmpReadFds);

                    ern = errno;

                    while ((result = select(pDev->fd + 1, &tmpReadFds,
                                            UGL_NULL, UGL_NULL, &tmpTimeout)) &&
                           result >= 0) {

                        (*pDev->pDriver->control) (pDev, ICR_READ, UGL_NULL);

                        FD_ZERO(&tmpReadFds);
                        FD_SET(pDev->fd, &tmpReadFds);
                    }

                    if (result == 0) {
                        errno = ern;
                    }
                }
            }
        }
        else if (result == 0) {
            errno = ern;
        }

        /* Process messages put on queue by device read control command */
        while (uglMsgQGet(srvId->pInputQ, &msg, UGL_NO_WAIT) ==
               UGL_STATUS_OK) {
            qId = srvId->pDefaultQ;

            status = uglCbListExecute(srvId->pCbList, srvId, &msg, qId);
            if (status == UGL_STATUS_OK && qId != UGL_NULL) {
                uglMsgQPost(qId, &msg, UGL_NO_WAIT);
            }
        }

        /* Check input task for state changes */
        while (srvId->taskState != UGL_SERVICE_RUN &&
               srvId->taskState != UGL_SERVICE_DESTROY_REQ) {

            switch(srvId->taskState) {
                case UGL_SERVICE_IDLE_REQ:
                    srvId->taskState = UGL_SERVICE_IDLE;
                    break;

                case UGL_SERVICE_RESUME_REQ:
                    srvId->taskState = UGL_SERVICE_RUN;
                    break;

                default:
                    uglOSTaskDelay(UGL_SERVICE_DELAY_CYCLE);
                    break;
            }
        }
    }

    /* If we end up here the input task is destroyed */
    srvId->taskId    = (UGL_TASK_ID) 0;
    srvId->taskState = UGL_SERVICE_DESTROYED;
}

/******************************************************************************
 *
 * uglPtrMsgMap - Map raw pointer messages
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS uglPtrMsgMap (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_MSG              *pMsg
    ) {
    UGL_STATUS  status;

    if (pMsg->type != MSG_RAW_PTR) {
        status = UGL_STATUS_ERROR;
    }
    else {

        /* Convert raw pointer message to pointer message */
        pMsg->type = MSG_POINTER;
        pMsg->data.pointer.timeStamp = uglOSTimeStamp();

        /* Convert coordinates */
        if (pMsg->data.rawPtr.isAbsolute == UGL_TRUE) {
            memcpy(&pMsg->data.pointer.position,
                   &pMsg->data.rawPtr.pos.absolute,
                   sizeof(UGL_POINT));
        }
        else {
            pMsg->data.pointer.position.x =
               srvId->position.x + pMsg->data.rawPtr.pos.relative.x;
            pMsg->data.pointer.position.y =
               srvId->position.y + pMsg->data.rawPtr.pos.relative.y;
        }

        /* Clip to bouding rectangle */
        if (pMsg->data.pointer.position.x < srvId->boundingRect.left) {
            pMsg->data.pointer.position.x = srvId->boundingRect.left;
        }

        if (pMsg->data.pointer.position.x > srvId->boundingRect.right) {
            pMsg->data.pointer.position.x = srvId->boundingRect.right;
        }

        if (pMsg->data.pointer.position.y < srvId->boundingRect.top) {
            pMsg->data.pointer.position.y = srvId->boundingRect.top;
        }

        if (pMsg->data.pointer.position.y > srvId->boundingRect.bottom) {
            pMsg->data.pointer.position.y = srvId->boundingRect.bottom;
        }

        /* Calculate position delta */
        pMsg->data.pointer.delta.x =
            pMsg->data.pointer.position.x - srvId->position.x;
        pMsg->data.pointer.delta.y =
            pMsg->data.pointer.position.y - srvId->position.y;

        /* Determine button state change */
       pMsg->data.pointer.buttonChange = pMsg->data.rawPtr.buttonState ^
          (srvId->modifiers & UGL_PTR_MOD_MASK);
       pMsg->data.pointer.buttonState =
           srvId->modifiers ^= pMsg->data.pointer.buttonChange;

        status = UGL_STATUS_OK;
    }

    return status;
}
