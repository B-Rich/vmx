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

/* uglWinP.h - Private window header for universal graphics library */

#ifndef _uglWinP_h
#define _uglWinP_h

#include "ugl.h"
#include "uglos.h"
#include "uglinput.h"
#include "winMsg.h"
#include "winClass.h"
#include "private/uglListP.h"
#include "private/uglMsgP.h"
#include "private/uglRegionP.h"

/* Defines */

#define WIN_MSG_TYPE_WAKE_UP       (100 - 1)
#define WIN_MSG_TYPE_FLUSH         (100 - 2)

#define WIN_PUBLIC_ATTRIBS         0x00ffffff
#define WIN_PRIVATE_ATTRIBS        0xff000000

#define WIN_ATTRIB_ROOT            0x80000000
#define WIN_ATTRIB_FRAME           0x40000000

#define WIN_PUBLIC_STATES          0x0000ffff
#define WIN_PRIVATE_STATES         0xffff0000

#define WIN_STATE_DEAD             0x80000000

#define WIN_APP_STATE_PENDING      0x00000001
#define WIN_APP_STATE_EXIT_WAIT    0x00000002

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct win_class {
    UGL_NODE                    node;
    UGL_CHAR *                  pName;
    struct win_class *          pParent;
    WIN_MSG_HANDLER *           pMsgHandler;
    UGL_SIZE                    windowSize;
    UGL_SIZE                    dataSize;
    UGL_ORD                     dataOffset;
    void *                      pDefaultData;
    UGL_SIZE                    useCount;
} WIN_CLASS;

typedef struct win_resource {
    UGL_NODE                    node;
    UGL_ID                      id;
} WIN_RESOURCE;

typedef struct win_app {
    UGL_NODE                    node;
    UGL_CHAR *                  pName;
    struct win_mgr *            pWinMgr;
    UGL_TASK_ID                 taskId;
    UGL_MSG_QUEUE *             pQueue;
    UGL_GC *                    pGc;
    UGL_LIST                    resourceList;
    UGL_CB_LIST                 callbackList;
    UGL_UINT32                  state;
    UGL_LOCK_ID                 lockId;
} WIN_APP;

typedef struct ugl_window {
    UGL_NODE                    node;
    WIN_RESOURCE                resource;
    WIN_CLASS *                 pClass;
    struct ugl_window *         pParent;
    struct ugl_window *         pActiveChild;
    UGL_LIST                    childList;
    UGL_UINT32                  attributes;
    UGL_UINT32                  state;
    UGL_RECT                    rect;
    UGL_REGION                  paintersRegion;
    UGL_REGION                  visibleRegion;
    UGL_REGION                  dirtyRegion;
    WIN_APP *                   pApp;
    void *                      pClassData;
    void *                      pAppData;
    UGL_CB_LIST                 callbackList;
    UGL_BITMAP_ID               dblBufBitmapId;
} UGL_WINDOW;

struct win_mgr_engine;

typedef struct win_mgr {
    struct win_mgr_engine *     pEngine;
    void *                      pEngineData;
    UGL_UGI_DRIVER *            pDisplay;
    struct ugl_input_service *  pInputService;
    WIN_CLASS *                 pFrameClass;
    UGL_WINDOW *                pRootWindow;
    UGL_WINDOW *                pPtrGrabWindow;
    UGL_WINDOW *                pPtrWin;
    UGL_WINDOW *                pKbdGrabWindow;
    UGL_LOCK_ID                 lockId;
    UGL_BITMAP_ID               offScreenBitmapId;
    UGL_CB_LIST                 callbackList;
    UGL_COLOR *                 pColorTable;
    UGL_SIZE                    colorTableSize;
    UGL_CDDB_ID *               pCursorTable;
    UGL_SIZE                    cursorTableSize;
    UGL_FONT_ID *               pFontTable;
    UGL_SIZE                    fontTableSize;
    UGL_LIST                    appList;
} WIN_MGR;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglWinP_h */

