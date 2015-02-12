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

/* winManage.c - Universal graphics library window manager */

#include "ugl.h"
#include "uglWin.h"
#include "private/uglWinP.h"

/* Globals */

const UGL_CHAR  winRootClassName[]  = "WIN_ROOT_CLASS";
WIN_MGR *       pDefaultWinMgr      = UGL_NULL;

/* Locals */
UGL_LOCAL UGL_STATUS  winManageCbMsgRoute (
    UGL_INPUT_SERVICE_ID  inputServiceId,
    UGL_MSG *             pMsg,
    UGL_MSG_Q_ID *        pQueueId,
    WIN_MGR_ID            winMgrId
    );

UGL_LOCAL UGL_STATUS  winManageCallback (
    WIN_APP_ID  appId,
    WIN_MSG *   pMsg,
    void *      pData,
    WIN_MGR *   pWinMgr
    );

/******************************************************************************
 *
 * winMgrCreate - Create window manager
 *
 * RETURNS: Window manager id or UGL_NULL
 */

WIN_MGR_ID  winMgrCreate (
    UGL_DEVICE_ID         displayId,
    UGL_INPUT_SERVICE_ID  inputServiceId,
    WIN_MGR_ENGINE *      pEngine
    ) {
    WIN_MGR *  pWinMgr;

    pWinMgr = UGL_CALLOC(1, sizeof(WIN_MGR));
    if (pWinMgr != UGL_NULL) {

        pWinMgr->lockId = uglOSLockCreate();
        if (pWinMgr->lockId == UGL_NULL) {
            UGL_FREE(pWinMgr);
            pWinMgr = UGL_NULL;
        }

        /* Initialize window manager stuct */
        pWinMgr->pDisplay      = (UGL_UGI_DRIVER *) displayId;
        pWinMgr->pInputService = inputServiceId;
        pWinMgr->pEngine       = pEngine;

        /* Add input callbacks */
        uglInputCbAdd(
            pWinMgr->pInputService,
            MSG_INPUT_FIRST,
            MSG_INPUT_LAST,
            (UGL_INPUT_CB *) winManageCbMsgRoute,
            pWinMgr
            );

        /* Register window manager */
        uglRegistryAdd(UGL_WIN_MGR_TYPE, (UGL_ARG) pWinMgr, 0, UGL_NULL);
        if (pDefaultWinMgr == UGL_NULL) {
            pDefaultWinMgr = pWinMgr;
        }

        /* Call engine create method */
        pWinMgr->pEngineData = (*pEngine->pCreateFunc) (
            pWinMgr,
            displayId,
            inputServiceId
            );

        /* Add application callbacks */
        winAppCbAdd(
            (WIN_APP_ID) uglListFirst(&pWinMgr->appList),
            MSG_APP_FIRST,
            MSG_APP_LAST,
            (WIN_APP_CB *) winManageCallback,
            pWinMgr);
    }

    return pWinMgr;
}

/******************************************************************************
 *
 * winMgrRootWinSet - Set window manager root window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMgrRootWinSet (
    WIN_MGR_ID  winMgrId,
    WIN_ID      rootWinId
    ) {
    UGL_STATUS  status;
    UGL_RECT    rect;

    if (winMgrId == UGL_NULL && winMgrId->pDisplay != UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        /* Setup display area rectangle */
        rect.left   = 0;
        rect.top    = 0;
        rect.right  = winMgrId->pDisplay->pMode->width - 1;
        rect.bottom = winMgrId->pDisplay->pMode->height - 1;

        /* Setup root window */
        winMgrId->pRootWindow = (UGL_WINDOW *) rootWinId;
        winMgrId->pRootWindow->state |= WIN_ATTRIB_ROOT;
        winRectSet(winMgrId->pRootWindow, &rect);
        winManage(winMgrId->pRootWindow);
        winShow(winMgrId->pRootWindow);

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winMgrFrameClassSet - Set window manager frame class
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMgrFrameClassSet (
    WIN_MGR_ID    winMgrId,
    WIN_CLASS_ID  classId
    ) {
    UGL_STATUS  status;
    WIN_MSG     msg;

    if (winMgrId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (classId == UGL_NULL &&
            winMgrId->pFrameClass != UGL_NULL &&
            --winMgrId->pFrameClass->useCount == 0) {

           msg.type  = MSG_CLASS_DEINIT;
           msg.winId = winMgrId->pRootWindow;
           status = (*winMgrId->pFrameClass->pMsgHandler) (
               winMgrId->pRootWindow,
               winMgrId->pFrameClass,
               &msg,
               UGL_NULL
               );
        }
        else if (classId != UGL_NULL &&
                 classId->useCount++ == 0) {
            msg.type  = MSG_CLASS_INIT;
            msg.winId = winMgrId->pRootWindow;
            status = (*classId->pMsgHandler) (
                winMgrId->pRootWindow,
                classId,
                &msg,
                UGL_NULL
                );
        }

        /* Set frame class */
        winMgrId->pFrameClass = classId;
    }

    return status;
}

/******************************************************************************
 *
 * winMgrColorTableSet - Set window manager color table
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMgrColorTableSet (
    WIN_MGR_ID   winMgrId,
    UGL_COLOR *  pColorTable,
    UGL_SIZE     colorTableSize
    ) {
    UGL_STATUS  status;

    if (winMgrId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        winMgrId->pColorTable    = pColorTable;
        winMgrId->colorTableSize = colorTableSize;

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winMgrFontTableSet - Set window manager font table
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMgrFontTableSet (
    WIN_MGR_ID     winMgrId,
    UGL_FONT_ID *  pFontTable,
    UGL_SIZE       fontTableSize
    ) {
    UGL_STATUS  status;

    if (winMgrId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        winMgrId->pFontTable    = pFontTable;
        winMgrId->fontTableSize = fontTableSize;

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winMgrFontTableGet - Get window manager font table
 *
 * RETURNS: Pointer to font table or UGL_NULL
 */

UGL_FONT_ID *  winMgrFontTableGet (
    WIN_MGR_ID winMgrId,
    UGL_SIZE * pFontTableSize
    ) {
    UGL_FONT_ID *  pFontTable;

    if (winMgrId == UGL_NULL) {
        pFontTable = UGL_NULL;
    }
    else {
        if (pFontTableSize != UGL_NULL) {
            *pFontTableSize = winMgrId->fontTableSize;
        }

        pFontTable = winMgrId->pFontTable;
    }

    return pFontTable;
}

/******************************************************************************
 *
 * winManageCbMsgRoute - Route messages
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winManageCbMsgRoute (
    UGL_INPUT_SERVICE_ID  inputServiceId,
    UGL_MSG *             pMsg,
    UGL_MSG_Q_ID *        pQueueId,
    WIN_MGR_ID            winMgrId
    ) {

    /* TODO */

    return UGL_STATUS_ERROR;
}

/******************************************************************************
 *
 * winManageCallback - Window manager callback method
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winManageCallback (
    WIN_APP_ID  appId,
    WIN_MSG *   pMsg,
    void *      pData,
    WIN_MGR *   pWinMgr
    ) {

    /* TODO */

    return UGL_STATUS_ERROR;
}

