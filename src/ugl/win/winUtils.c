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

/* winUtils.c - Universal graphics library window utility library */

#include <stdlib.h>
#include <string.h>
#include "uglWin.h"
#include "private/uglWinP.h"

/* Imports */

extern WIN_MGR *  pDefaultWinMgr;

/******************************************************************************
 *
 * winCount - Get number of child windows
 *
 * RETURNS: Number of child windows or 0
 */

UGL_SIZE  winCount (
    WIN_ID  winId
    ) {
    UGL_SIZE  count;

    if (winId == UGL_NULL) {
        count = 0;
    }
    else {
        count = uglListCount(&winId->childList);
    }

    return count;
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
 * winLast - Get window last child
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winLast (
    WIN_ID  winId
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListLast(&winId->childList);
    }

    return pWindow;
}

/******************************************************************************
 *
 * winNext - Get next window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winNext (
    WIN_ID  winId
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListNext(&winId->node);
    }

    return pWindow;
}

/******************************************************************************
 *
 * winPrev - Get previous window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winPrev (
    WIN_ID  winId
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListPrev(&winId->node);
    }

    return pWindow;
}

/******************************************************************************
 *
 * winNth - Get n:th window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winNth (
    WIN_ID   winId,
    UGL_ORD  n
    ) {
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        pWindow = UGL_NULL;
    }
    else {
        pWindow = (UGL_WINDOW *) uglListNth(&winId->childList, n);
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
 * winStateGet - Get window state
 *
 * RETURNS: Window state or UGL_STATUS_ERROR
 */

UGL_UINT32  winStateGet (
    WIN_ID  winId
    ) {
    UGL_UINT32  state;

    if (winId == UGL_NULL) {
        state = UGL_STATUS_ERROR;
    }
    else {
        state = winId->state;
    }

    return state;
}


/******************************************************************************
 *
 * winAttribGet - Get public window attributes
 *
 * RETURNS: Window attributes or zero
 */

UGL_UINT32  winAttribGet (
    WIN_ID  winId
    ) {
    UGL_UINT32  attributes;

    if (winId == UGL_NULL) {
        attributes = 0;
    }
    else {
        attributes = winId->attributes & WIN_PUBLIC_ATTRIBS;
    }

    return attributes;
}

/******************************************************************************
 *
 * winVisibleSet - Check if a window is visible
 *
 * RETURNS: UGL_TRUE or UGL_FALSE
 */

UGL_STATUS  winVisibleSet (
    WIN_ID    winId,
    UGL_BOOL  visible
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
            if (winId->pParent == UGL_NULL) {
                status = UGL_STATUS_ERROR;
            }
            else {
                pWindow = winId->pParent;
                status = UGL_STATUS_OK;
            }
        }
        else {
            pWindow = winId;
            status = UGL_STATUS_OK;
        }
    }

    if (status == UGL_STATUS_OK) {
        winLock(winId);

        if (visible == UGL_TRUE &&
            (pWindow->attributes & WIN_ATTRIB_VISIBLE) == 0x00) {

            pWindow->attributes |= WIN_ATTRIB_VISIBLE;
            if (((pWindow->state & WIN_STATE_MANAGED) != 0x00) &&
                (pWindow->pParent == UGL_NULL ||
                 (pWindow->pParent->state & WIN_STATE_HIDDEN) != 0x00)) {

                winShow(pWindow);
            }
        }
        else if (visible == UGL_FALSE &&
                 (pWindow->attributes & WIN_ATTRIB_VISIBLE) != 0x00) {

            winHide(pWindow);
            pWindow->attributes &= ~WIN_ATTRIB_VISIBLE;
        }

        winUnlock(winId);
    }

    return status;
}

/******************************************************************************
 *
 * winVisibleGet - Check if a window is visible
 *
 * RETURNS: UGL_TRUE or UGL_FALSE
 */

UGL_BOOL  winVisibleGet (
    WIN_ID  winId
    ) {
    UGL_STATUS    status;
    UGL_BOOL      result;
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
            if (winId->pParent == UGL_NULL) {
                status = UGL_STATUS_ERROR;
            }
            else {
                pWindow = winId->pParent;
                status = UGL_STATUS_OK;
            }
        }
        else {
            pWindow = winId;
            status = UGL_STATUS_OK;
        }
    }

    if (status == UGL_STATUS_OK) {
        if ((pWindow->attributes & WIN_ATTRIB_VISIBLE) != 0x00) {
            result = UGL_TRUE;
        }
        else {
            result = UGL_FALSE;
        }
    }
    else {
        result = UGL_FALSE;
    }
    
    return result;
}

/******************************************************************************
 *
 * winMgrGet - Get window manager for window
 *
 * RETURNS: Pointer to window manager or UGL_NULL
 */

WIN_MGR_ID  winMgrGet (
    WIN_ID  winId
    ) {
    WIN_MGR_ID  winMgrId;

    if (winId == UGL_NULL || (winId->state & WIN_STATE_MANAGED) == 0x00) {
        winMgrId = UGL_NULL;
    }
    else {
        winMgrId = winId->pApp->pWinMgr;
    }

    return winMgrId;
}

/******************************************************************************
 *
 * winWindowToScreen - Convert window coordinates to screen coordinates
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winWindowToScreen (
    WIN_ID       winId,
    UGL_POINT *  pPoints,
    UGL_SIZE     numPoints
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pWindow;
    UGL_ORD       i;
    UGL_ORD       dx;
    UGL_ORD       dy;

    if (winId == UGL_NULL || pPoints == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pWindow = winId;
        dx = 0;
        dy = 0;

        while (pWindow != UGL_NULL) {
            dx += pWindow->rect.left;
            dy += pWindow->rect.top;

            /* Advance */
            pWindow = pWindow->pParent;
        }

        for (i = 0; i < numPoints; i++) {
            UGL_POINT_MOVE(pPoints[i], dx, dy);
        }

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winWindowRectToScreen - Convert window rectangles to screen coordinates
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winWindowRectToScreen (
    WIN_ID       winId,
    UGL_RECT *   pRects,
    UGL_SIZE     numRects
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pWindow;
    UGL_ORD       i;
    UGL_ORD       dx;
    UGL_ORD       dy;

    if (winId == UGL_NULL || pRects == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pWindow = winId;
        dx = 0;
        dy = 0;

        while (pWindow != UGL_NULL) {
            dx += pWindow->rect.left;
            dy += pWindow->rect.top;

            /* Advance */
            pWindow = pWindow->pParent;
        }

        for (i = 0; i < numRects; i++) {
            UGL_RECT_MOVE(pRects[i], dx, dy);
        }

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winScreenToWindow - Convert screen coordinates to window coordinates
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winScreenToWindow (
    WIN_ID       winId,
    UGL_POINT *  pPoints,
    UGL_SIZE     numPoints
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pWindow;
    UGL_ORD       i;
    UGL_ORD       dx;
    UGL_ORD       dy;

    if (winId == UGL_NULL || pPoints == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pWindow = winId;
        dx = 0;
        dy = 0;

        while (pWindow != UGL_NULL) {
            dx -= pWindow->rect.left;
            dy -= pWindow->rect.top;

            /* Advance */
            pWindow = pWindow->pParent;
        }

        for (i = 0; i < numPoints; i++) {
            UGL_POINT_MOVE(pPoints[i], dx, dy);
        }

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winDrawRectGet - Get window drawing rectangle
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDrawRectGet (
    WIN_ID      winId,
    UGL_RECT *  pRect
    ) {
    UGL_STATUS  status;

    if (winId == UGL_NULL || pRect == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
       pRect->left   = 0;
       pRect->top    = 0;
       pRect->right  = winId->rect.right - winId->rect.left;
       pRect->bottom = winId->rect.bottom - winId->rect.top;
    }

    return status;
}

/******************************************************************************
 *
 * winGetFromPoint - Get window at point
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winGetFromPoint (
    WIN_MGR_ID   winMgrId,
    UGL_POINT *  pPoint
    ) {
    WIN_ID          winId;
    UGL_REG_DATA *  pData;
    UGL_WINDOW *    pWindow;
    UGL_WINDOW *    pChild;

    if (winMgrId == UGL_NULL) {
        pData = uglRegistryFind(UGL_WIN_MGR_TYPE, UGL_NULL, 0, UGL_NULL);
        if (pData != UGL_NULL) {
            winMgrId = (WIN_MGR_ID) pData->data;
        }
    }

    if (winMgrId != UGL_NULL) {
        pChild = winMgrId->pRootWindow;
        if (pChild == UGL_NULL) {
            winId = UGL_NULL;
        }
        else {
            winLock(winMgrId->pRootWindow);

            pWindow = UGL_NULL;
            while (pChild != UGL_NULL) {
                if ((pChild->state & WIN_STATE_HIDDEN) == 0x00 &&
                    UGL_POINT_IN_RECT(*pPoint, pChild->rect)) {
                    pPoint->x -= pChild->rect.left;
                    pPoint->y -= pChild->rect.top;

                    /* Store hit */
                    pWindow = pChild;

                    /* Advance to last */
                    pChild = (UGL_WINDOW *) uglListLast(&pWindow->childList);
                    continue;
                }

                /* Advance */
                pChild = (UGL_WINDOW *) uglListPrev(&pChild->node);
            }

            winUnlock(winMgrId->pRootWindow);
            winId = pWindow;
        }
    }
    else {
        winId = UGL_NULL;
    }

    return winId;
}

/******************************************************************************
 *
 * winDisplayGet - Get display device for window
 *
 * RETURNS: Display device id or UGL_NULL
 */

UGL_DEVICE_ID  winDisplayGet (
    WIN_ID  winId
    ) {
    UGL_DEVICE_ID  devId;

    if (winId == UGL_NULL || (winId->state & WIN_STATE_MANAGED) == 0x00) {
        devId = UGL_NULL;
    }
    else {
        devId = winId->pApp->pWinMgr->pDisplay;
    }

    return devId;
}

/******************************************************************************
 *
 * winColorGet - Get color from window standard colors or zero
 *
 * RETURNS: Window color
 */

UGL_COLOR  winColorGet (
    WIN_ID   winId,
    UGL_ORD  index
    ) {
    UGL_COLOR  color;
    WIN_MGR *  pWinMgr;

    if (winId == UGL_NULL) {
        pWinMgr = pDefaultWinMgr;
    }
    else {
        pWinMgr = winId->pApp->pWinMgr;
    }

    if (index < 0 || index >= pWinMgr->colorTableSize) {
        color = 0;
    }
    else {
        color = pWinMgr->pColorTable[index];
    }

    return color;
}

/******************************************************************************
 *
 * winCursorImageGet - Get cursor from window cursors
 *
 * RETURNS: Cursor image id or UGL_NULL
 */

UGL_CDDB_ID  winCursorImageGet (
    WIN_ID   winId,
    UGL_ORD  index
    ) {
    UGL_CDDB_ID  cursorId;
    WIN_MGR *    pWinMgr;

    if (winId == UGL_NULL) {
        pWinMgr = pDefaultWinMgr;
    }
    else {
        pWinMgr = winId->pApp->pWinMgr;
    }

    if (pWinMgr == UGL_NULL || index < 0 ||
        index >= pWinMgr->cursorTableSize) {

        cursorId = UGL_NULL;
    }
    else {
        cursorId = pWinMgr->pCursorTable[index];
    }

    return cursorId;
}

/******************************************************************************
 *
 * winPointerGrab - Grab pointer input to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winPointerGrab (
    WIN_ID  winId
    ) {
    UGL_STATUS  status;
    WIN_MGR *   pWinMgr;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pWinMgr = winId->pApp->pWinMgr;
        pWinMgr->pPtrGrabWindow = winId;
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winPointerGrabGet - Get window which has focus
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winPointerGrabGet (
    WIN_MGR_ID  winMgrId
    ) {
    WIN_ID          winId;
    UGL_REG_DATA *  pData;

    if (winMgrId == UGL_NULL) {
        pData = uglRegistryFind(UGL_WIN_MGR_TYPE, UGL_NULL, 0, UGL_NULL);
        if (pData != UGL_NULL) {
            winMgrId = (WIN_MGR_ID) pData->data;
        }
    }

    if (winMgrId != UGL_NULL) {
        winId = winMgrId->pPtrGrabWindow;
    }
    else {
        winId = UGL_NULL;
    }

    return winId;
}

/******************************************************************************
 *
 * winPointerUngrab - Release pointer input from window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winPointerUngrab (
    WIN_ID  winId
    ) {
    UGL_STATUS  status;
    WIN_MGR *   pWinMgr;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pWinMgr = winId->pApp->pWinMgr;
        if (pWinMgr->pPtrGrabWindow != winId) {
            status = UGL_STATUS_ERROR;
        }
        else {
            pWinMgr->pPtrGrabWindow = UGL_NULL;
            status = UGL_STATUS_OK;
        }
    }

    return status;
}

