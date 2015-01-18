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

             /* TODO: winAppDeinit(pApp); */
             pApp = UGL_NULL;
        }
    }

    return pApp;
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
 * winAppInit - Post message to application queue
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

#ifdef TODO
    /* Initialize windowing system */
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
 * winAppTask - Window application task
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  winAppTask (
    WIN_APP *  pApp
    ) {

    /* TODO */
}

