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
#include "winMgr/wwm/wwm.h"

/* Defines */

#define WWM_APP_PRIORITY          (WIN_APP_DEF_PRIORITY - 1)
#define WWM_APP_STACK_SIZE        20000

/* Imports */
extern const UGL_RGB   wwmRGBColorTable[16];

extern const UGL_CDIB  wwmCDibArrow;
extern const UGL_CDIB  wwmCDibEdit;
extern const UGL_CDIB  wwmCDibWait;
extern const UGL_CDIB  wwmCDibInvalid;
extern const UGL_CDIB  wwmCDibMove;
extern const UGL_CDIB  wwmCDibSizeHoriz;
extern const UGL_CDIB  wwmCDibSizeVert;
extern const UGL_CDIB  wwmCDibSizeDiagTlBr;
extern const UGL_CDIB  wwmCDibSizeDiagTrBl;

extern WIN_CLASS_ID    wwmFrameClassCreate(void);

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
    void *              pResult;
    WIN_APP_ID          appId;
    WIN_ID              rootWinId;
    UGL_COLOR *         pColorTable;
    UGL_MODE_INFO       modeInfo;
    UGL_REG_DATA *      pRegData;
    UGL_FONT_DRIVER_ID  fntDrvId;
    UGL_FONT_DEF        fntDef;
    UGL_CDDB_ID *       pCursorTable;
    UGL_FONT_ID *       pFntTable;
    UGL_ORD             textOrigin;
    UGL_ORD             i;

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

    /* Initialize cursor */
    uglCursorInit(displayId, 32, 32, modeInfo.width / 2, modeInfo.height / 2);

    /* Initialize cursor table */
    pCursorTable = UGL_CALLOC(WIN_NUM_STANDARD_CURSORS, sizeof(UGL_CDDB_ID));

    pCursorTable[WIN_CURSOR_INDEX_ARROW] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibArrow
        );

    pCursorTable[WIN_CURSOR_INDEX_EDIT] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibEdit
        );

    pCursorTable[WIN_CURSOR_INDEX_WAIT] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibWait
        );

    pCursorTable[WIN_CURSOR_INDEX_INVALID] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibInvalid
        );

    pCursorTable[WIN_CURSOR_INDEX_MOVE] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibMove
        );

    pCursorTable[WIN_CURSOR_INDEX_SIZE_HORIZ] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibSizeHoriz
        );

    pCursorTable[WIN_CURSOR_INDEX_SIZE_LEFT] =
        pCursorTable[WIN_CURSOR_INDEX_SIZE_HORIZ];

    pCursorTable[WIN_CURSOR_INDEX_SIZE_RIGHT] =
        pCursorTable[WIN_CURSOR_INDEX_SIZE_HORIZ];

    pCursorTable[WIN_CURSOR_INDEX_SIZE_VERT] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibSizeVert
        );

    pCursorTable[WIN_CURSOR_INDEX_SIZE_TOP] =
        pCursorTable[WIN_CURSOR_INDEX_SIZE_VERT];

    pCursorTable[WIN_CURSOR_INDEX_SIZE_BOTTOM] =
        pCursorTable[WIN_CURSOR_INDEX_SIZE_VERT];

    pCursorTable[WIN_CURSOR_INDEX_SIZE_TL_BR] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibSizeDiagTlBr
        );

    pCursorTable[WIN_CURSOR_INDEX_SIZE_TOP_LEFT] =
        pCursorTable[WIN_CURSOR_INDEX_SIZE_TL_BR];

    pCursorTable[WIN_CURSOR_INDEX_SIZE_BOTTOM_RIGHT] =
        pCursorTable[WIN_CURSOR_INDEX_SIZE_TL_BR];

    pCursorTable[WIN_CURSOR_INDEX_SIZE_TR_BL] = uglCursorBitmapCreate(
        displayId,
        (UGL_CDIB *) &wwmCDibSizeDiagTrBl
        );

    pCursorTable[WIN_CURSOR_INDEX_SIZE_TOP_RIGHT] =
        pCursorTable[WIN_CURSOR_INDEX_SIZE_TR_BL];

    pCursorTable[WIN_CURSOR_INDEX_SIZE_BOTTOM_LEFT] =
        pCursorTable[WIN_CURSOR_INDEX_SIZE_TR_BL];

    /* Set cursor table */
    winMgrCursorTableSet(winMgrId, pCursorTable, WIN_NUM_STANDARD_CURSORS);

    /* Setup current cursor */
    uglCursorImageSet(displayId, pCursorTable[WIN_CURSOR_INDEX_ARROW]);
    uglCursorOn(displayId);

    pRegData = uglRegistryFind(UGL_FONT_ENGINE_TYPE, UGL_NULL, 0, 0);
    if (pRegData != UGL_NULL) {
        fntDrvId  = (UGL_FONT_DRIVER_ID) pRegData->data;
        pFntTable = UGL_CALLOC(WIN_NUM_STANDARD_FONTS, sizeof(UGL_FONT_ID));

        uglFontFindString(fntDrvId, WWM_SYSTEM_FONT, &fntDef);
        pFntTable[WIN_FONT_INDEX_SYSTEM] = uglFontCreate(fntDrvId, &fntDef);

        uglFontFindString(fntDrvId, WWM_SMALL_FONT, &fntDef);
        pFntTable[WIN_FONT_INDEX_SMALL] = uglFontCreate(fntDrvId, &fntDef);

        uglFontFindString(fntDrvId, WWM_FIXED_FONT, &fntDef);
        pFntTable[WIN_FONT_INDEX_FIXED] = uglFontCreate(fntDrvId, &fntDef);

        textOrigin = UGL_FONT_TEXT_UPPER_LEFT;
        uglFontInfo(
            pFntTable[WIN_FONT_INDEX_SYSTEM],
            UGL_FONT_TEXT_ORIGIN_SET,
            &textOrigin
            );
        uglFontInfo(
            pFntTable[WIN_FONT_INDEX_SMALL],
            UGL_FONT_TEXT_ORIGIN_SET,
            &textOrigin
            );
        uglFontInfo(
            pFntTable[WIN_FONT_INDEX_FIXED],
            UGL_FONT_TEXT_ORIGIN_SET,
            &textOrigin
            );

        winMgrFontTableSet(winMgrId, pFntTable, WIN_NUM_STANDARD_FONTS);
    }
    else {
        winMgrFontTableSet(winMgrId, UGL_NULL, 0);
    }

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

