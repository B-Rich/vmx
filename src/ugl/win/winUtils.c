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

