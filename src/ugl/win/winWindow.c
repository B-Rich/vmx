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

UGL_LOCAL UGL_VOID  winWakeUp (
    WIN_APP_ID  appId
    );

UGL_LOCAL UGL_VOID  winDirtyClear (
    UGL_WINDOW *  pWindow
    );

UGL_LOCAL UGL_VOID  winClassInit (
    UGL_WINDOW *  pWindow
    );

UGL_LOCAL UGL_VOID  winClassDeinit (
    WIN_ID  winId
    );

UGL_LOCAL UGL_STATUS  winDefaultMsgHandler (
    WIN_ID     winId,
    WIN_MSG *  pMsg
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
    WIN_ID        frameId;

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

            if (appId->pWinMgr->pFrameClass != UGL_NULL) {
                pWindow->attributes |= WIN_ATTRIB_VISIBLE;

                /* Setup attributes for frame window */
                attributes |= (WIN_ATTRIB_FRAME | WIN_ATTRIB_CLIP_CHILDREN);
                attributes &= ~(WIN_ATTRIB_FRAMED | WIN_ATTRIB_DOUBLE_BUFFER |
                                WIN_ATTRIB_OFF_SCREEN | WIN_ATTRIB_NO_POINTER);

                /* Create window for frame and attach content window */
                frameId = winCreate (
                    appId->pWinMgr->pRootWindow->pApp,
                    appId->pWinMgr->pFrameClass,
                    attributes,
                    x,
                    y,
                    0,
                    0,
                    UGL_NULL,
                    0,
                    UGL_NULL
                    );

                /* TODO: winAttach(pWindow, frameId, UGL_NULL); */

                winFrameCaptionSet(frameId, appId->pName);

                /* TODO: winSizeSet(pWindow, width, height); */
            }
            else {
                pWindow->attributes &= ~WIN_ATTRIB_FRAME;
            }
        }
    }

    return pWindow;
}

/******************************************************************************
 *
 * winDestroy - Destroy window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDestroy (
    WIN_ID  winId
    ) {
    UGL_STATUS    status;
    WIN_APP *     pApp;
    UGL_WINDOW *  pWindow;
    WIN_ID        childId;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pApp = winId->pApp;

        if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00 &&
            winId->pParent != UGL_NULL) {
            pWindow = winId->pParent;
        }
        else {
            pWindow = winId;
        }

        winLock(pWindow);

#ifdef TODO
        if (pWindow->pParent != UGL_NULL) {
            winDetach(pWindow);
        }
#endif

        while ((childId = winFirst(pWindow)) != UGL_NULL) {
            uglListRemove(&pWindow->childList, (UGL_NODE *) childId);
            childId->pParent = UGL_NULL;
            winDestroy(childId);
        }

        winUnlock(pWindow);

        /* Mark as destroyed */
        uglOSLock(pApp->lockId);

        pWindow->state &= ~WIN_STATE_DIRTY;
        pWindow->state |= WIN_STATE_DEAD;

        /* Move to end of resource list */
        uglListRemove(&pApp->resourceList, &pWindow->resource.node);
        uglListAdd(&pApp->resourceList, &pWindow->resource.node);

        uglOSUnlock(pApp->lockId);

        winWakeUp(pApp);

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winFree - Free resources used by window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winFree (
    WIN_ID  winId
    ) {
    UGL_STATUS  status;
    WIN_MSG     msg;
    WIN_APP *   pApp = winId->pApp;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        winSend(winId, MSG_DESTROY, UGL_NULL, 0);

        /* Remove from resource list */
        uglOSLock(pApp->lockId);
        uglListRemove(&pApp->resourceList, &winId->resource.node);
        uglOSUnlock(pApp->lockId);

        if (uglListCount(&pApp->resourceList) == 0) {
            msg.type  = MSG_APP_NO_WINDOWS;
            msg.winId = UGL_NULL;

            uglOSMsgQPost(
                pApp->pQueue->osQId,
                UGL_NO_WAIT,
                &msg,
                sizeof(WIN_MSG)
                );
        }

        winClassDeinit(winId);

        uglCbListDeinit(&winId->callbackList);

        /* Deinit regions */
        uglRegionDeinit(&winId->paintersRegion);
        uglRegionDeinit(&winId->visibleRegion);
        uglRegionDeinit(&winId->dirtyRegion);

        UGL_FREE(winId);

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winActivate - Activate window
 *
 * RETURNS: UGL_STATUS_OK, UGL_STATUS_PERMISSION_DENIED or UGL_STATUS_ERROR
 */

UGL_STATUS  winActivate (
    WIN_ID  winId
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pWindow;
    UGL_WINDOW *  pOldWindow;
    UGL_WINDOW *  pDeactivateWindow;
    UGL_WINDOW *  pParent = winId->pParent;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
        goto activateRet;
    }

    if ((winId->attributes & WIN_ATTRIB_NO_ACTIVATE) != 0x00) {
        status = UGL_STATUS_PERMISSION_DENIED;
        goto activateRet;
    }

    if ((winId->state & WIN_STATE_ACTIVE) != 0x00) {
        status = UGL_STATUS_OK;
        goto activateRet;
    }

    if (pParent == UGL_NULL) {
        status = UGL_STATUS_OK;
        goto activateRet;
    }
    else {
        status = winActivate(pParent);
        if (status != UGL_STATUS_OK ||
            (winId->state & WIN_STATE_ACTIVE) != 0x00) {
            goto activateRet;
        }
    }

    if ((winId->state | WIN_STATE_MANAGED) != 0x00) {

        if (pParent->pActiveChild != UGL_NULL &&
            pParent->pActiveChild != winId &&
            (pParent->pActiveChild->state & WIN_STATE_ACTIVE) != 0x00) {

            pOldWindow = pParent->pActiveChild;
            while (pOldWindow->pActiveChild != UGL_NULL &&
                   (pOldWindow->pActiveChild->state &
                    WIN_STATE_ACTIVE) != 0x00) {

                pOldWindow = pOldWindow->pActiveChild;
            }

            pDeactivateWindow = pOldWindow;
            while(pDeactivateWindow != pParent) {

                if ((pDeactivateWindow->state & WIN_STATE_ACTIVE) != 0x00) {

                    pDeactivateWindow->state &= ~WIN_STATE_ACTIVE;
                    status = winSend(
                        pDeactivateWindow,
                        MSG_DEACTIVATE,
                        &winId,
                        sizeof(WIN_ID)
                        );
                    if (status != UGL_STATUS_OK) {

                        pWindow = pDeactivateWindow;

                        pDeactivateWindow->state |= WIN_STATE_ACTIVE;
                        while (pDeactivateWindow->pActiveChild != UGL_NULL) {

                            pDeactivateWindow = pDeactivateWindow->pActiveChild;

                            pDeactivateWindow->state |= WIN_STATE_ACTIVE;
                            winSend(
                                pDeactivateWindow,
                                MSG_DEACTIVATE,
                                &pWindow,
                                sizeof(WIN_ID)
                                );
                        }

                        goto activateRet;
                    }
                }

               /* Advance */
               pDeactivateWindow = pDeactivateWindow->pParent;
            }

            /* Deactivate old app and activate new app */
            if (pOldWindow->pApp != winId->pApp) {
                winAppPost(
                    pOldWindow->pApp,
                    MSG_APP_DEACTIVATE,
                    UGL_NULL,
                    0,
                    UGL_NO_WAIT
                    );

                winAppPost(
                    winId->pApp,
                    MSG_APP_ACTIVATE,
                    UGL_NULL,
                    0,
                    UGL_NO_WAIT
                    );
            }
        }

        /* Activate new window */
        winId->state |= WIN_STATE_ACTIVE;
        pParent->pActiveChild = winId;
        status = winSend(winId, MSG_ACTIVATE, &winId, sizeof(WIN_ID));

        if (winId->pActiveChild != UGL_NULL) {

            pWindow = winId->pActiveChild;
            while (pWindow != UGL_NULL) {

                pWindow->state |= WIN_STATE_ACTIVE;
                status = winSend(
                    pWindow,
                    MSG_ACTIVATE,
                    &pWindow,
                    sizeof(WIN_ID)
                    );
                if (status != UGL_STATUS_OK) {
                    goto activateRet;
                }

                /* Advance */
                pWindow = pWindow->pActiveChild;
            }
        }
    }
    else {
        pParent->pActiveChild = winId;
    }

activateRet:

    return status;
}

/******************************************************************************
 *
 * winIsActive - Check if a window is active
 *
 * RETURNS: UGL_TRUE or UGL_FALSE
 */

UGL_BOOL  winIsActive (
    WIN_ID  winId
    ) {
    UGL_BOOL  result;

    if (winId == UGL_NULL || (winId->state & WIN_STATE_ACTIVE) == 0x00) {
        result = UGL_FALSE;
    }
    else {
        result = UGL_TRUE;
    }

    return result;
}

/******************************************************************************
 *
 * winDirtyGet - Get next window on dirty list
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winDirtyGet (
    WIN_APP_ID  appId
    ) {
    UGL_WINDOW *  pWindow;

    if (uglListFirst(&appId->resourceList) == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        uglOSLock(appId->lockId);

        pWindow = (UGL_WINDOW *)
            ((WIN_RESOURCE *) uglListFirst(&appId->resourceList))->id;
        while ((pWindow->state & WIN_STATE_DIRTY) != 0x00 &&
               pWindow->dirtyRegion.pFirstTL2BR == UGL_NULL) {

            winDirtyClear(pWindow);

            pWindow = (UGL_WINDOW *)
                ((WIN_RESOURCE *) uglListFirst(&appId->resourceList))->id;
        }

        uglOSUnlock(appId->lockId);

        if ((pWindow->state & WIN_STATE_DIRTY) == 0x00) {
            pWindow = UGL_NULL;
        }
    }

    return pWindow;
}

/******************************************************************************
 *
 * winDeadGet - Get next window on destroy list
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winDeadGet (
    WIN_APP_ID  appId
    ) {
    UGL_WINDOW *    pWindow;
    WIN_RESOURCE *  pResource;

    if (uglListLast(&appId->resourceList) == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        uglOSLock(appId->lockId);

        for (pResource = (WIN_RESOURCE *) uglListLast(&appId->resourceList);
             pResource != UGL_NULL;
             pResource = (WIN_RESOURCE *) uglListPrev(&pResource->node)) {

            pWindow = (UGL_WINDOW *) pResource->id;
            if ((pWindow->state & WIN_STATE_DEAD) != 0x00) {
                break;
            }
        }

        uglOSUnlock(appId->lockId);
    }

    return pWindow;
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
 * winWakeUp - Wake up application window
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  winWakeUp (
    WIN_APP_ID  appId
    ) {
    UGL_MSG  msg;

    if ((appId->state & WIN_APP_STATE_PENDING) != 0x00) {
        msg.type = WIN_MSG_TYPE_WAKE_UP;
        uglOSMsgQPost(
            appId->pQueue->osQId,
            UGL_NO_WAIT,
            &msg,
            sizeof(UGL_MSG)
            );
    }
}

/******************************************************************************
 *
 * winDirtyClear - Clear window dirty attribute
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  winDirtyClear (
    UGL_WINDOW *  pWindow
    ) {
    WIN_APP *  pApp = pWindow->pApp;

    if ((pWindow->state & WIN_STATE_DIRTY) != 0x00) {
        uglOSLock(pApp->lockId);

        /* Move to end of list */
        uglListRemove(&pApp->resourceList, &pWindow->resource.node);
        uglListAdd(&pApp->resourceList, &pWindow->resource.node);

        pWindow->state &= ~WIN_STATE_DIRTY;

        uglOSUnlock(pApp->lockId);
    }
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

        if (pClass->useCount++ == 0) {
            msg.type  = MSG_CLASS_INIT;
            msg.winId = pWindow;

            (*pClass->pMsgHandler) (
                pWindow,
                pClass,
                &msg,
                (UGL_INT8 *) pWindow->pClassData + pClass->dataOffset
                );
        }
    }
}
 
/******************************************************************************
 *
 * winClassDeinit - Deinitialize class hierarcy for window
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  winClassDeinit (
    WIN_ID  winId
    ) {
    WIN_MSG      msg;
    WIN_CLASS *  pClass = winId->pClass;

    if (pClass != UGL_NULL) {

        if (winId->pParent != UGL_NULL) {
            winClassDeinit(winId->pParent);
        }

        if (--pClass->useCount == 0) {
            msg.type  = MSG_CLASS_DEINIT;
            msg.winId = winId;

            (*pClass->pMsgHandler) (
                winId,
                pClass,
                &msg,
                (UGL_INT32 *) winId->pClassData + pClass->dataOffset
                );
        }
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

