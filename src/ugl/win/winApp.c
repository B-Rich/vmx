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

/* winApp.c - Universal ghaphics library application context */

#include <string.h>
#include "ugl.h"
#include "uglWin.h"
#include "private/uglWinP.h"

/* Locals */

UGL_LOCAL const UGL_CHAR  winAppDefaultName[] = "winApp";

UGL_LOCAL WIN_APP_ID  winAppInit (
    UGL_CHAR *               pName,
    UGL_SIZE                 qSize,
    const WIN_APP_CB_ITEM *  pCallbackArray
    );

UGL_LOCAL UGL_STATUS  winAppMsgGet (
    WIN_APP_ID   appId,
    WIN_MSG *    pMsg,
    UGL_TIMEOUT  timeout
    );

UGL_LOCAL UGL_VOID  winAppTask (
    WIN_APP *  pApp
    );

/******************************************************************************
 *
 * winAppCreate - Create application context
 *
 * RETURNS: Application id or UGL_NULL
 */

WIN_APP_ID  winAppCreate (
    UGL_CHAR *               pName,
    UGL_UINT32               priority,
    UGL_SIZE                 stackSize,
    UGL_SIZE                 qSize,
    const WIN_APP_CB_ITEM *  pCallbackArray
    ) {
    WIN_APP *  pApp;

    if (pName == UGL_NULL) {
        pName = (UGL_CHAR *) winAppDefaultName;
    }

    pApp = (WIN_APP *) winAppInit(pName, qSize, pCallbackArray);
    if (pApp != UGL_NULL) {

        /* Check and select defaults if requested */
        if (priority <= 0) {
            priority = WIN_APP_DEF_PRIORITY;
        }

        if (stackSize <= 0) {
            stackSize = WIN_APP_DEF_STACK_SIZE;
        }

        /* Create application task */
        pApp->taskId = uglOSTaskCreate(
            pName,
            (UGL_FPTR) winAppTask,
            priority,
            0,
            stackSize,
            (UGL_ARG) pApp,
            (UGL_ARG) 0,
            (UGL_ARG) 0,
            (UGL_ARG) 0,
            (UGL_ARG) 0);

        if (pApp->taskId == (UGL_TASK_ID) 0) {
             winAppDestroy(pApp);
             pApp = UGL_NULL;
        }
    }

    return pApp;
}

/******************************************************************************
 *
 * winAppDestroy - Destroy application context
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS  winAppDestroy (
    WIN_APP_ID  appId
    ) {
    UGL_STATUS      status;
    UGL_MSG         msg;
    WIN_RESOURCE *  pResource;
    UGL_WINDOW *    pWindow;

    if (appId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (appId->taskId != (UGL_TASK_ID) 0) {

            /* Terminate by task */
            status = winAppPost(
                appId,
                MSG_APP_EXIT,
                UGL_NULL,
                0,
                UGL_WAIT_FOREVER
                );
        }
        else {
            pResource = (WIN_RESOURCE *) uglListFirst(&appId->resourceList);
            while (pResource != UGL_NULL) {

                pWindow = (UGL_WINDOW *) pResource->id;
                if ((pWindow->state & WIN_STATE_DEAD) != 0x00) {
                    break;
                }

                winDestroy(pWindow);

                /* Advance */
                pResource = (WIN_RESOURCE *) uglListFirst(&appId->resourceList);
            }

            /* Collect destroyed windows  */
            uglOSLock(appId->lockId);
            while ((pWindow = winDeadGet(appId)) != UGL_NULL) {
                winFree(pWindow);
            }
            uglOSUnlock(appId->lockId);

            /* Remove from window manager app list */
            uglOSLock(appId->pWinMgr->lockId);
            uglListRemove(&appId->pWinMgr->appList, &appId->node);
            uglOSUnlock(appId->pWinMgr->lockId);

            /* Send destroy message */
            memset(&msg, 0, sizeof(UGL_MSG));
            msg.type = MSG_APP_DESTROY;
            uglCbListExecute(&appId->callbackList, appId, &msg, appId->pQueue);

            /* Deinit callback list */
            uglCbListDeinit(&appId->callbackList);

            /* Destroy graphics context */
            uglGcDestroy(appId->pGc);

            /* Destroy message queue */
            uglMsgQDestroy(appId->pQueue);

            /* Destroy lock */
            uglOSLockDestroy(appId->lockId);

            UGL_FREE(appId);

#ifdef WINDOW_SYSTEM_AUTOSTART
            winDeinit();
#endif

            status = UGL_STATUS_OK;
        }
    }

    return status;
}

/******************************************************************************
 *
 * winAppCbAdd - Add message callback to application
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS  winAppCbAdd (
    WIN_APP_ID    appId,
    UGL_UINT32    filterMin,
    UGL_UINT32    filterMax,
    WIN_APP_CB *  pCallback,
    void *        pParam
    ) {
    STATUS  status;

    if (appId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = uglCbAdd(
            &appId->callbackList,
            filterMin,
            filterMax,
            (UGL_CB *) pCallback,
            pParam
            );
    }

    return status;
}

/******************************************************************************
 *
 * winAppCbRemove - Remove message callback from application
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winAppCbRemove (
    WIN_APP_ID    appId,
    WIN_APP_CB *  pCallback
    ) {
    UGL_STATUS  status;

    if (appId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = uglCbRemove(&appId->callbackList, (UGL_CB *) pCallback);
    }

    return status;
}

/******************************************************************************
 *
 * winAppGet - Get application context for window
 *
 * RETURNS: Window application context id or UGL_NULL
 */

WIN_APP_ID  winAppGet (
    WIN_ID  winId
    ) {
    WIN_APP_ID  appId;

    if (winId == UGL_NULL) {
        appId = UGL_NULL;
    }
    else {
        appId = winId->pApp;
    }

    return appId;
}

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
    ) {
    UGL_STATUS  status;

    if (appId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = uglMsgQPost(appId->pQueue, (UGL_MSG *) pMsg, timeout);
    }

    return status;
}

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
    ) {
    WIN_MSG  msg;

    msg.type  = msgType;
    msg.winId = UGL_NULL;
    memcpy(msg.data.uglData.reserved, pMsgData, dataSize);

    return winAppMsgPost(appId, &msg, timeout);
}

/******************************************************************************
 *
 * winAppInit - Initialize application context
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_LOCAL WIN_APP_ID  winAppInit (
    UGL_CHAR *               pName,
    UGL_SIZE                 qSize,
    const WIN_APP_CB_ITEM *  pCallbackArray
    ) {
    WIN_APP *       pApp;
    UGL_MSG         msg;
    UGL_REG_DATA *  pRegData;
    WIN_MGR *       pWinMgr = UGL_NULL;

    if (pName == UGL_NULL) {
        pName = (UGL_CHAR *) winAppDefaultName;
    }

#ifdef WINDOW_SYSTEM_AUTOSTART
    winInit();
#endif

    /* Find window manager */
    pRegData = uglRegistryFind(UGL_WIN_MGR_TYPE, 0, 0, 0);
    if (pRegData == UGL_NULL) {
        pApp = UGL_NULL;
    }
    else {
        pWinMgr = (WIN_MGR *) pRegData->data;

        /* Check and select defaults if requested */
        if (qSize <= 0) {
            qSize = WIN_APP_DEF_QUEUE_SIZE;
        }

        pApp = (WIN_APP *) UGL_CALLOC(1, sizeof(WIN_APP) + strlen(pName) + 1);
        if (pApp != UGL_NULL) {

            /* Initialize application context */
            pApp->pName = (UGL_CHAR *) &pApp[1];
            strcpy(pApp->pName, pName);

            pApp->pWinMgr = pWinMgr;

            uglListInit(&pApp->resourceList);

            pApp->lockId = uglOSLockCreate();
            if (pApp->lockId == UGL_NULL) {
                UGL_FREE(pApp);
                pApp = UGL_NULL;
            }
            else {
                pApp->pQueue = uglMsgQCreate(qSize);
                if (pApp->pQueue == UGL_NULL) {
                    uglOSLockDestroy(pApp->lockId);
                    UGL_FREE(pApp);
                    pApp = UGL_NULL;
                }
                else {
                    pApp->pGc = uglGcCreate(pWinMgr->pDisplay);
                    if (pApp->pGc == UGL_NULL) {
                        uglMsgQDestroy(pApp->pQueue);
                        uglOSLockDestroy(pApp->lockId);
                        UGL_FREE(pApp);
                        pApp = UGL_NULL;
                    }
                    else {
                        uglOSLock(pApp->pWinMgr->lockId);
                        uglListAdd(&pApp->pWinMgr->appList, &pApp->node);
                        uglOSUnlock(pApp->pWinMgr->lockId);

                        if (pCallbackArray != UGL_NULL) {
                            uglCbAddArray(
                                &pApp->callbackList,
                                (UGL_CB_ITEM *) pCallbackArray
                                );
                        }

                        /* Send app create message */
                        memset(&msg, 0, sizeof(WIN_MSG));
                        msg.type = MSG_APP_CREATE;
                        uglMsgQPost(pApp->pQueue, &msg, UGL_WAIT_FOREVER);

                        pApp->taskId = uglOSGetTaskIdSelf();
                    }
                }
            }
        }
    }

    return pApp;
}

/******************************************************************************
 *
 * winAppMsgGet - Get application context message
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winAppMsgGet (
    WIN_APP_ID   appId,
    WIN_MSG *    pMsg,
    UGL_TIMEOUT  timeout
    ) {
    UGL_STATUS  status;
    WIN_ID      winId;

    /* Allow only one task to get messages */
    if (appId == UGL_NULL || appId->taskId != uglOSGetTaskIdSelf()) {
        status = UGL_STATUS_ERROR;
    }
    else {
        do {
            status = uglMsgQGet(appId->pQueue, (UGL_MSG *) pMsg, UGL_NO_WAIT);
            if (status == UGL_STATUS_Q_EMPTY) {
                appId->state |= WIN_APP_STATE_PENDING;

                if ((winId = winDirtyGet(appId)) != UGL_NULL) {
                    appId->state &= ~WIN_APP_STATE_PENDING;

                    pMsg->type  = MSG_EXPOSE;
                    pMsg->winId = winId;

                    status = UGL_STATUS_OK;
                    break;
                }

                /* Collect destroyed windows */
                while ((winId = winDeadGet(appId)) != UGL_NULL) {
                    winFree(winId);
                }

                /* Wait for message */
                status = uglMsgQGet(appId->pQueue, (UGL_MSG *) pMsg, timeout);
                appId->state &= ~WIN_APP_STATE_PENDING;
            }

        } while (pMsg->type == WIN_MSG_TYPE_WAKE_UP &&
                 status == UGL_STATUS_OK);
    }

    return status;
}

/******************************************************************************
 *
 * winAppTask - Window application task
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  winAppTask (
    WIN_APP *  pApp
    ) {
    UGL_STATUS   status;
    WIN_MSG      msg;
    UGL_TIMEOUT  timeout;
    UGL_TIMEOUT  timeStamp;
    UGL_TIMEOUT  exitTime = 0;

    /* Make sure it has correct task id */
    pApp->taskId = uglOSGetTaskIdSelf();

    while (UGL_TRUE) {

        /* Calcualte timeout */
        if ((pApp->state & WIN_APP_STATE_EXIT_WAIT) != 0x00) {
            timeStamp = uglOSTimeStamp();

            if (timeStamp < exitTime) {
                timeout = exitTime - timeStamp;
            }
            else {
                timeout = 0;
            }
        }
        else {
            timeout = UGL_WAIT_FOREVER;
        }

        /* Get next message */
        status = winAppMsgGet(pApp, &msg, timeout);
        if (status != UGL_STATUS_OK) {
            if ((pApp->state & WIN_APP_STATE_EXIT_WAIT) != 0x00 &&
                status == UGL_STATUS_Q_EMPTY) {

                msg.type                = MSG_APP_EXIT;
                msg.winId               = UGL_NULL;
                msg.data.appExitTimeout = 0;
            }
            else {
                continue;
            }
        }

        /* Execute application callbacks */
        status = uglCbListExecute(
            &pApp->callbackList,
            pApp,
            (UGL_MSG *) &msg,
            UGL_NULL
            );

        /* Process message */
        switch (msg.type) {
            case MSG_APP_NO_WINDOWS:
                if (status == UGL_STATUS_OK) {
                    winAppPost(pApp, MSG_APP_EXIT, UGL_NULL, 0, UGL_NO_WAIT);
                }
                break;

            case MSG_APP_EXIT:
                if (status == UGL_STATUS_OK) {
                    pApp->taskId = (UGL_TASK_ID) 0;
                    winAppDestroy(pApp);
                    return;
                }
                else if (msg.data.appExitTimeout != 0) {
                    pApp->state |= WIN_APP_STATE_EXIT_WAIT;
                    exitTime = uglOSTimeStamp() + msg.data.appExitTimeout;
                }
                else {
                    pApp->state &= ~WIN_APP_STATE_EXIT_WAIT;
                }
                break;

            default:
                if (status == UGL_STATUS_OK) {
                    winMsgSend(msg.winId, &msg);
                }
                break;
        }
    }
}

