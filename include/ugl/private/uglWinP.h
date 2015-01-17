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
#include "winMsg.h"
#include "winClass.h"
#include "private/winListP.h"
#include "private/uglMsgP.h"
#include "private/uglRegionP.h"

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct win_class {
    WIN_NODE                node;
    UGL_CHAR *              pName;
    struct win_class *      pParent;
    WIN_MSG_HANDLER *       pMsgHandler;
    UGL_SIZE                windowSize;
    UGL_SIZE                dataSize;
    UGL_ORD                 dataOffset;
    void *                  pDefaultData;
    UGL_SIZE                useCount;
} WIN_CLASS;

typedef struct win_resource {
    WIN_NODE                node;
    UGL_ID                  id;
} WIN_RESOURCE;

typedef struct win_app {
    WIN_NODE                node;
    UGL_CHAR *              pName;
    struct win_mgr *        pWinMgr;
    UGL_TASK_ID             taskId;
    UGL_MSG_QUEUE *         pQueue;
    UGL_GC *                pGc;
    WIN_LIST                resourceList;
    UGL_CB_LIST             callbackList;
    UGL_UINT32              state;
    UGL_LOCK_ID             lockId;
} WIN_APP;

typedef struct ugl_window {
    WIN_NODE                node;
    WIN_RESOURCE            resource;
    WIN_CLASS *             pClass;
    struct ugl_window *     pParent;
    struct ugl_window *     pActiveChild;
    WIN_LIST                childList;
    UGL_UINT32              attributes;
    UGL_UINT32              state;
    UGL_RECT                rect;
    UGL_REGION              paintersRegion;
    UGL_REGION              visibleRegion;
    UGL_REGION              dirtyRegion;
    WIN_APP *               pApp;
    void *                  pClassData;
    void *                  pAppData;
    UGL_CB_LIST             callbackList;
    UGL_BITMAP_ID           dblBufBitmapId;
} UGL_WINDOW;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglWinP_h */

