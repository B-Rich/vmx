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

/* wwmInit.c - Generic window manager */

#include "ugl.h"
#include "uglWin.h"

/* Defines */

#define WWM_APP_PRIORITY          (WIN_APP_DEF_PRIORITY - 1)
#define WWM_APP_STACK_SIZE        20000

/* Imports */
extern const UGL_RGB  wwmRGBColorTable[16];
extern WIN_CLASS_ID   wwmFrameClassCreate(void);

/* Locals */

UGL_LOCAL void *  wwmCreate (
    WIN_MGR_ID            winMgrId,
    UGL_DEVICE_ID         displayId,
    UGL_INPUT_SERVICE_ID  inputServiceId 
    );

UGL_LOCAL UGL_STATUS wwmDestroy (
    WIN_MGR_ID  winMgrId,
    void *      pData
    );

UGL_LOCAL UGL_STATUS  wwmRootWinCb (
    WIN_ID     winId,
    WIN_MSG *  pMsg,
    void *     pData,
    void *     pParam
    );

UGL_LOCAL WIN_MGR_ENGINE  wwmEngine = {
    wwmCreate,
    wwmDestroy,
    UGL_NULL
};

/* Globals */

WIN_MGR_ENGINE *  wwmEngineId = &wwmEngine;

/******************************************************************************
 *
 * wwmCreate - Create instance of window manager
 *
 * RETURNS: Pointer to display or UGL_NULL
 */

UGL_LOCAL void *  wwmCreate (
    WIN_MGR_ID            winMgrId,
    UGL_DEVICE_ID         displayId,
    UGL_INPUT_SERVICE_ID  inputServiceId 
    ) {
    void *         pResult;
    WIN_APP_ID     appId;
    WIN_ID         rootWinId;
    UGL_RECT       rootWinRect;
    UGL_COLOR *    pColorTable;
    UGL_MODE_INFO  modeInfo;
    UGL_ORD        i;

    /* Allocate window manager colors */
    pColorTable = UGL_CALLOC(WIN_NUM_STANDARD_COLORS, sizeof(UGL_COLOR));

    uglInfo(displayId, UGL_MODE_INFO_REQ, &modeInfo);
    if (modeInfo.clutSize == 0) {
        uglColorAlloc(
            displayId,
            (UGL_ARGB *) wwmRGBColorTable,
            UGL_NULL,
            pColorTable,
            WIN_NUM_STANDARD_COLORS
            );
    }
    else if (modeInfo.clutSize == WIN_NUM_STANDARD_COLORS) {
        for (i = 0; i < WIN_NUM_STANDARD_COLORS; i++) {
            uglColorAlloc(
                displayId,
                (UGL_ARGB *) &wwmRGBColorTable[i],
                &i,
                &pColorTable[i],
                1
                );
        }
    }

    /* Set color table */
    winMgrColorTableSet(winMgrId, pColorTable, WIN_NUM_STANDARD_COLORS);

    appId = winAppCreate(
        "wwm",
        WWM_APP_PRIORITY,
        WWM_APP_STACK_SIZE,
        0,
        UGL_NULL
        );
    if (appId == UGL_NULL) {
        pResult = UGL_NULL;
    }
    else {
        /* Create and set window class for frame */
        winMgrFrameClassSet(winMgrId, wwmFrameClassCreate());

        /* Create root window */
        rootWinId = winCreate(
            appId,
            UGL_NULL_ID,
            WIN_ATTRIB_VISIBLE,
            0,
            0,
            0,
            0,
            UGL_NULL,
            0,
            UGL_NULL
            );
        if (rootWinId == UGL_NULL) {
            pResult = UGL_NULL;
        }
        else {
            /* Add root window callback and set root window */
            winCbAdd(rootWinId, 0, 0, wwmRootWinCb, UGL_NULL);
            winMgrRootWinSet(winMgrId, rootWinId);

            pResult = displayId;
        }
    }

    return pResult;
}

/******************************************************************************
 *
 * wwmDestroy - Destroy instance of window manager
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS wwmDestroy (
    WIN_MGR_ID  winMgrId,
    void *      pData
    ) {

    /* TODO */

    return UGL_STATUS_ERROR;
}

/******************************************************************************
 *
 * wwmRootWinCb - Window manager root window callback
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  wwmRootWinCb (
    WIN_ID     winId,
    WIN_MSG *  pMsg,
    void *     pData,
    void *     pParam
    ) {

    /* TODO */

    return UGL_STATUS_OK;
}

