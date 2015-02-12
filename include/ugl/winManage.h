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

/* winManage.h - Universal graphics library window manager interface */

#ifndef _winManage_h
#define _winManage_h

#include "uglinput.h"

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef void *  (WIN_MGR_CREATE_FUNC) (
    WIN_MGR_ID            winMgrId,
    UGL_DEVICE_ID         displayId,
    UGL_INPUT_SERVICE_ID  inputServiceId
    );

typedef UGL_STATUS  (WIN_MGR_DESTROY_FUNC) (
    WIN_MGR_ID  winMgrId,
    void *      pWinMgrData
    );

typedef UGL_STATUS  (WIN_MGR_MENU_ADD_FUNC) (
    UGL_CHAR *  name,
    void        (*pFunc) (void *),
    void *      pParam
    );

typedef struct win_mgr_engine {
    WIN_MGR_CREATE_FUNC *    pCreateFunc;
    WIN_MGR_DESTROY_FUNC *   pDestroyFunc;
    WIN_MGR_MENU_ADD_FUNC *  pMenuAddFunc;
} WIN_MGR_ENGINE;

/* Functions */

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
    );

/******************************************************************************
 *
 * winMgrRootWinSet - Set window manager root window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMgrRootWinSet (
    WIN_MGR_ID  winMgrId,
    WIN_ID      rootWinId
    );

/******************************************************************************
 *
 * winMgrFrameClassSet - Set window manager frame class
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMgrFrameClassSet (
    WIN_MGR_ID    winMgrId,
    WIN_CLASS_ID  classId
    );

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
    );

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
    );

/******************************************************************************
 *
 * winMgrFontTableGet - Get window manager font table
 *
 * RETURNS: Pointer to font table or UGL_NULL
 */

UGL_FONT_ID *  winMgrFontTableGet (
    WIN_MGR_ID winMgrId,
    UGL_SIZE * pFontTableSize
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _winManage_h */

