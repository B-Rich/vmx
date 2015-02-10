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

#include <stdlib.h>
#include <string.h>
#include "uglWin.h"
#include "private/uglWinP.h"

#define GFX_MODE

/* Locals */

UGL_LOCAL UGL_VOID  winWakeUp (
    WIN_APP_ID  appId
    );

UGL_LOCAL UGL_VOID  winDirtyClear (
    WIN_ID  winId
    );

UGL_LOCAL UGL_STATUS winRegionExpose (
    WIN_ID               winId,
    const UGL_REGION_ID  regionId,
    const UGL_RECT *     pRect
    );

UGL_LOCAL UGL_STATUS  winReveal (
    WIN_ID         winId,
    UGL_REGION_ID  regionId
    );

UGL_LOCAL UGL_STATUS  winRegionObscure (
    WIN_ID               winId,
    const UGL_REGION_ID  regionId,
    const UGL_RECT *     pRect
    );

UGL_LOCAL UGL_STATUS  winConceal (
    WIN_ID         winId,
    UGL_REGION_ID  regionId
    );

UGL_LOCAL UGL_VOID  winBackgroundDraw (
    UGL_GC_ID     gcId,
    WIN_ID        winId,
    WIN_ID        childId,
    UGL_RECT *    pViewport,
    UGL_REGION *  pClipRegion,
    UGL_ORD       dx,
    UGL_ORD       dy
    );

UGL_LOCAL UGL_VOID  winClassInit (
    WIN_ID  winId
    );

UGL_LOCAL UGL_VOID  winClassDeinit (
    WIN_ID  winId
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

                winAttach(pWindow, frameId, UGL_NULL);
                winFrameCaptionSet(frameId, appId->pName);
                winSizeSet(pWindow, width, height);
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
 * winCbAdd - Add callback to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winCbAdd (
    WIN_ID      winId,
    UGL_UINT32  filterMin,
    UGL_UINT32  filterMax,
    WIN_CB *    pCallback,
    void *      pParam
    ) {
    UGL_STATUS  status;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = uglCbAdd(
            &winId->callbackList,
            filterMin,
            filterMax,
            (UGL_CB *) pCallback,
            pParam
            );
    }

    return status;
}

/******************************************************************************
 *
 * winSizeSet - Set size of a window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winSizeSet (
    WIN_ID    winId,
    UGL_SIZE  width,
    UGL_SIZE  height
    ) {
    UGL_STATUS  status;
    UGL_RECT    rect;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        memcpy(&rect, &winId->rect, sizeof(UGL_RECT));
        if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
            UGL_RECT_MOVE(
                rect,
                winId->pParent->rect.left,
                winId->pParent->rect.top
                );
        }
        UGL_RECT_SIZE_TO(rect, width, height);

        status = winRectSet(winId, &rect);
    }

    return status;
}

/******************************************************************************
 *
 * winRectSet - Set dimensions of a window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winRectSet (
    WIN_ID            winId,
    const UGL_RECT *  pRect
    ) {
    UGL_STATUS         status;
    WIN_MSG            msg;
    UGL_RECT           rect;
    UGL_RECT           oldRect;
    UGL_RECT           newRect;
    UGL_RECT           intersectRect;
    UGL_RECT           viewportRect;
    UGL_REGION         region;
    UGL_REGION         moveRegion;
    UGL_REGION_RECT *  pRegionRect;
    UGL_WINDOW *       pParent;
    UGL_WINDOW *       pSibling;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
        goto rectSetRet;
    }

    if (pRect == UGL_NULL) {
        pRect = &winId->rect;
    }
    else if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
        status = winSend(
            winId->pParent,
            MSG_FRAME_CONTENT_RECT_SET,
            pRect,
            sizeof(UGL_RECT)
            );
        goto rectSetRet;
    }

    /* Send change message to parent if needed */
    if (winId->pParent != UGL_NULL) {
        msg.type = MSG_RECT_CHILD_CHANGING;
        msg.data.rectChildChanging.childId = winId;
        memcpy(
            &msg.data.rectChildChanging.newRect,
            pRect,
            sizeof(UGL_RECT)
            );
        memcpy(
            &msg.data.rectChildChanging.oldRect,
            &winId->rect,
            sizeof(UGL_RECT)
            );
        winMsgSend(winId->pParent, &msg);
        memcpy(
            &newRect,
            &msg.data.rectChildChanging.newRect,
            sizeof(UGL_RECT)
            );
    }
    else {
        memcpy(&newRect, pRect, sizeof(UGL_RECT));
    }

    memcpy(&oldRect, &winId->rect, sizeof(UGL_RECT));

    /* Check that new rectangle not the same as the current rectangle */
    if (newRect.left == oldRect.left && newRect.top == oldRect.top &&
        newRect.right == oldRect.right && newRect.bottom == oldRect.bottom) {

        /* Done */
        status = UGL_STATUS_OK;
        goto rectSetRet;
    }

    if ((winId->state & WIN_STATE_HIDDEN) == 0x00) {
        uglRegionInit(&region);
        winLock(winId);

        if (UGL_RECT_WIDTH(newRect) != UGL_RECT_WIDTH(oldRect) ||
            UGL_RECT_HEIGHT(newRect) != UGL_RECT_HEIGHT(oldRect)) {

            /* Obscure portion of the window */
            memcpy(&rect, &newRect, sizeof(UGL_RECT));
            UGL_RECT_MOVE_TO(rect, oldRect.left, oldRect.top);
            uglRegionRectInclude(&region, &oldRect);
            uglRegionRectExclude(&region, &rect);
            for (pRegionRect = region.pFirstTL2BR;
                 pRegionRect != UGL_NULL;
                 pRegionRect = pRegionRect->pNextTL2BR) {

                winRegionObscure(winId, UGL_NULL, &pRegionRect->rect);
            }

            uglRegionEmpty(&region);

            memcpy(&winId->rect, &newRect, sizeof(UGL_RECT));

            /* Expose portion of the window */
            memcpy(&rect, &oldRect, sizeof(UGL_RECT));
            UGL_RECT_MOVE_TO(rect, newRect.left, newRect.top);
            uglRegionRectInclude(&region, &newRect);
            uglRegionRectExclude(&region, &rect);
            for (pRegionRect = region.pFirstTL2BR;
                 pRegionRect != UGL_NULL;
                 pRegionRect = pRegionRect->pNextTL2BR) {

                winRegionExpose(winId, UGL_NULL, &pRegionRect->rect);
            }

            uglRegionEmpty(&region);
        }

#ifdef GFX_MODE
        if (newRect.left != oldRect.left || newRect.top != oldRect.top) {
            winInvalidate(winId);
        }
#endif

        memcpy(&winId->rect, &oldRect, sizeof(UGL_RECT));

        /* Expose parent and siblings */
        uglRegionRectInclude(&region, &oldRect);
        uglRegionIntersect(
            &region,
            &winId->pParent->paintersRegion,
            &region
            );
        uglRegionRectExclude(&region, &newRect);

        pSibling = winNext(winId);
        while (pSibling != UGL_NULL && region.pFirstTL2BR != UGL_NULL) {

            if ((pSibling->state & WIN_STATE_HIDDEN) == 0x00) {
                UGL_RECT_INTERSECT(winId->rect, pSibling->rect, intersectRect);

                if (intersectRect.right >= intersectRect.left &&
                    intersectRect.bottom >= intersectRect.top) {

                    uglRegionRectExclude(&region, &intersectRect);
                    if ((pSibling->attributes &
                         WIN_ATTRIB_TRANSPARENT) != 0x00) {

                        UGL_RECT_MOVE(
                            intersectRect,
                            -pSibling->rect.left,
                            -pSibling->rect.top
                            );
                        winRectInvalidate(pSibling, &intersectRect);
                    }
                }
            }

            /* Advance */
            pSibling = winNext(pSibling);
        }

        if (region.pFirstTL2BR != UGL_NULL) {
            winConceal(winId, &region);
            uglRegionEmpty(&region);
        }

        /* Move window */
        memcpy(&winId->rect, &newRect, sizeof(UGL_RECT));

        /* Reveal parent and siblings */
        uglRegionRectInclude(&region, &newRect);
        uglRegionRectExclude(&region, &oldRect);
        if (region.pFirstTL2BR != UGL_NULL) {
            winReveal(winId, &region);
            uglRegionEmpty(&region);
        }

        /* Obscure window and siblings */
        uglRegionRectInclude(&region, &winId->rect);
        uglRegionExclude(&region, &winId->pParent->paintersRegion);
        pSibling = winNext(winId);
        while (pSibling != UGL_NULL) {

            if ((pSibling->state & WIN_STATE_HIDDEN) == 0x00) {
                UGL_RECT_INTERSECT(winId->rect, pSibling->rect, intersectRect);

                if (intersectRect.right >= intersectRect.left &&
                    intersectRect.bottom >= intersectRect.top) {

                    uglRegionRectInclude(&region, &intersectRect);
                    if ((pSibling->attributes &
                         WIN_ATTRIB_TRANSPARENT) != 0x00) {

                        UGL_RECT_MOVE(
                            intersectRect,
                            -pSibling->rect.left,
                            -pSibling->rect.top
                            );
                        winRectInvalidate(pSibling, &intersectRect);
                    }
                }
            }

            /* Advance */
            pSibling = winNext(pSibling);
        }

        if (region.pFirstTL2BR != UGL_NULL) {
            winRegionObscure(winId, &region, &winId->rect);
            uglRegionEmpty(&region);
        }

        /* Send message rect changed */
        msg.type = MSG_RECT_CHANGED;
        memcpy(&msg.data.rectChanged.newRect, &newRect, sizeof(UGL_RECT));
        memcpy(&msg.data.rectChanged.oldRect, &oldRect, sizeof(UGL_RECT));

        /* Compensate for frame */
        if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
            UGL_RECT_MOVE(
                msg.data.rectChanged.newRect,
                winId->pParent->rect.left,
                winId->pParent->rect.top
                );
            UGL_RECT_MOVE(
                msg.data.rectChanged.oldRect,
                winId->pParent->rect.left,
                winId->pParent->rect.top
                );
        }

        msg.data.rectChanged.validRect.left = 0;
        msg.data.rectChanged.validRect.top  = 0;
        msg.data.rectChanged.validRect.right = min(
            oldRect.right - oldRect.left,
            newRect.right - newRect.left
            );
        msg.data.rectChanged.validRect.bottom = min(
            oldRect.bottom - oldRect.top,
            newRect.bottom - newRect.top
            );

        uglRegionRectInclude(&region, &msg.data.rectChanged.validRect);
        winMsgSend(winId, &msg);
        uglRegionRectExclude(&region, &msg.data.rectChanged.validRect);

        if (region.pFirstTL2BR != UGL_NULL) {
            pRegionRect = region.pFirstTL2BR;
            while (pRegionRect != UGL_NULL) {
                winRectInvalidate(winId, &pRegionRect->rect);

                /* Advance */
                pRegionRect = pRegionRect->pNextTL2BR;

            }
        }

#ifdef GFX_MODE
        /* Move window */
        if (newRect.left != oldRect.left || newRect.top != oldRect.top) {
            uglRegionInit(&moveRegion);
            uglRegionCopy(&winId->paintersRegion, &moveRegion);
            uglRegionClipToRect(&moveRegion, &msg.data.rectChanged.validRect);
            uglRegionExclude(&moveRegion, &winId->dirtyRegion);
            uglRegionMove(&moveRegion, newRect.left, newRect.top);

            if (moveRegion.pFirstTL2BR != UGL_NULL) {
                pParent = winId->pParent;
                memcpy(&viewportRect, &pParent->rect, sizeof(UGL_RECT));
                while (pParent->pParent != UGL_NULL) {

                    /* Advance */
                    pParent = pParent->pParent;

                    UGL_RECT_MOVE(
                        viewportRect,
                        pParent->rect.left,
                        pParent->rect.right
                        );
                }

                uglDefaultBitmapSet(winId->pApp->pGc, UGL_DISPLAY_ID);
                uglViewPortSet(
                    winId->pApp->pGc,
                    viewportRect.left,
                    viewportRect.top,
                    viewportRect.right,
                    viewportRect.bottom
                    );
                uglClipRegionSet(winId->pApp->pGc, &moveRegion);
                uglRasterModeSet(winId->pApp->pGc, UGL_RASTER_OP_COPY);
                uglBitmapBlt(
                    winId->pApp->pGc,
                    UGL_DEFAULT_ID,
                    oldRect.left,
                    oldRect.top,
                    oldRect.right,
                    oldRect.bottom,
                    UGL_DEFAULT_ID,
                    newRect.left,
                    newRect.top
                    );
            }

            uglRegionDeinit(&moveRegion);
        }
#endif

        /* Expose window */
        uglRegionCopy(&winId->pParent->paintersRegion, &region);
        uglRegionClipToRect(&region, &winId->rect);
        uglRegionMove(&region, -winId->rect.left, -winId->rect.top);
        uglRegionExclude(&region, &winId->paintersRegion);

        if (region.pFirstTL2BR != UGL_NULL) {
            uglRegionMove(&region, winId->rect.left, winId->rect.top);

            pSibling = winNext(winId);
            while (pSibling != UGL_NULL) {

                if ((pSibling->state & WIN_STATE_HIDDEN) == 0x00) {
                    UGL_RECT_INTERSECT(
                        winId->rect,
                        pSibling->rect,
                        intersectRect
                        );

                    if (intersectRect.right >= intersectRect.left &&
                        intersectRect.bottom >= intersectRect.top) {

                        uglRegionRectExclude(&region, &intersectRect);
                    }
                }

                /* Advance */
                pSibling = winNext(pSibling);
            }

            if (region.pFirstTL2BR != UGL_NULL) {
                winRegionExpose(winId, &region, &winId->rect);
            }

            uglRegionEmpty(&region);
        }

        winUnlock(winId);
        uglRegionDeinit(&region);
    }
    else {
        memcpy(&winId->rect, &newRect, sizeof(UGL_RECT));

        /* Send rect changed message */
        msg.type = MSG_RECT_CHANGED;
        memcpy(&msg.data.rectChanged.newRect, &newRect, sizeof(UGL_RECT));
        memcpy(&msg.data.rectChanged.oldRect, &oldRect, sizeof(UGL_RECT));

        /* Compensate for frame */
        if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
            UGL_RECT_MOVE(
                msg.data.rectChanged.newRect,
                winId->pParent->rect.left,
                winId->pParent->rect.top
                );
            UGL_RECT_MOVE(
                msg.data.rectChanged.oldRect,
                winId->pParent->rect.left,
                winId->pParent->rect.top
                );
        }

        memset(&msg.data.rectChanged.validRect, 0, sizeof(UGL_RECT));
        winMsgSend(winId, &msg);
    }

    status = UGL_STATUS_OK;

rectSetRet:

    return status;
}

/******************************************************************************
 *
 * winRectGet - Get window rectangle
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winRectGet (
    WIN_ID      winId,
    UGL_RECT *  pRect
    ) {
    UGL_STATUS  status;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        memcpy(pRect, &winId->rect, sizeof(UGL_RECT));

        /* Adjust for frame */
        if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
            UGL_RECT_MOVE(
                *pRect,
                winId->pParent->rect.left,
                winId->pParent->rect.top
                );
        }

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winZPosSet - Set window depth position
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winZPosSet (
    WIN_ID   winId,
    UGL_ORD  zPos
    ) {
    UGL_STATUS    status;
    UGL_ORD       oldPos;
    UGL_ORD       newPos;
    UGL_ORD       bottomPos;
    UGL_RECT      intersectRect;
    UGL_REGION    hideRegion;
    UGL_REGION    exposeRegion;
    WIN_MSG       msg;
    UGL_WINDOW *  pParent;
    UGL_WINDOW *  pSibling;
    UGL_WINDOW *  pOldWindow;
    UGL_WINDOW *  pNextWindow;

    if (winId == UGL_NULL || winId->pParent == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = UGL_STATUS_OK;

        if ((winId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
            winId = winId->pParent;
            if (winId->pParent == UGL_NULL) {
                status = UGL_STATUS_ERROR;
            }
        }

        if (zPos == 0 || zPos > winCount(winId->pParent)) {
            status = UGL_STATUS_ERROR;
        }
    }

    if (status == UGL_STATUS_OK) {
        if (zPos < 0) {
            zPos = winCount(winId->pParent) + zPos + 1;
        }

        pParent = winId->pParent;
        oldPos = winZPosGet(winId);
        bottomPos = winCount(winId->pParent);

        /* Send zpos child changing message */
        msg.type               = MSG_ZPOS_CHILD_CHANGING;
        msg.data.zPos.newPos   = zPos;
        msg.data.zPos.oldPos   = oldPos;
        msg.data.zPos.changeId = winId;
        winMsgSend(winId->pParent, &msg);

        newPos = msg.data.zPos.newPos;

        /* Check that new position is different */
        if (newPos != oldPos) {
            winLock(winId);

            /* Change postition in window list */
            pOldWindow = winPrev(winId);
            uglListRemove(&pParent->childList, &winId->node);
            if (newPos == 1) {
                pNextWindow = UGL_NULL;
            }
            else if (newPos == bottomPos) {
                pNextWindow = winFirst(pParent);
            }
            else {
                pNextWindow = winNth(pParent, -(newPos - 1));
            }
            uglListInsert(
                &pParent->childList,
                &winId->node,
                &pNextWindow->node
                );

            /* Update window regions */
            if ((winId->state & WIN_STATE_HIDDEN) == 0x00) {
                uglRegionInit(&hideRegion);
                uglRegionInit(&exposeRegion);

                if (newPos > oldPos) {
                    uglRegionCopy(&winId->paintersRegion, &exposeRegion);
                    uglRegionMove(
                        &exposeRegion,
                        winId->rect.left,
                        winId->rect.top
                        );

                    pSibling = pOldWindow;
                    while (pSibling != winId &&
                           exposeRegion.pFirstTL2BR != UGL_NULL) {

                        if ((pSibling->state & WIN_STATE_HIDDEN) == 0x00) {
                            UGL_RECT_INTERSECT(
                                pSibling->rect,
                                winId->rect,
                                intersectRect
                                );

                            if (intersectRect.right >= intersectRect.left &&
                                intersectRect.bottom >= intersectRect.top) {

                                winRegionExpose(
                                    pSibling,
                                    &exposeRegion,
                                    &intersectRect
                                    );
                                uglRegionRectExclude(
                                    &exposeRegion,
                                    &intersectRect
                                    );
                                uglRegionRectInclude(
                                    &hideRegion,
                                    &intersectRect
                                    );
                            }
                        }

                        /* Advance */
                        pSibling = winNext(pSibling);
                    }

                    winRegionObscure(winId, &hideRegion, &winId->rect);
                }
                else {
                    uglRegionRectInclude(&hideRegion, &winId->rect);
                    uglRegionIntersect(
                        &hideRegion,
                        &pParent->paintersRegion,
                        &hideRegion
                        );
                    uglRegionMove(
                        &hideRegion,
                        -winId->rect.left,
                        -winId->rect.top
                        );
                    uglRegionExclude(&hideRegion, &winId->paintersRegion);
                    uglRegionMove(
                        &hideRegion,
                        winId->rect.left,
                        winId->rect.top
                        );
                    uglRegionCopy(&hideRegion, &exposeRegion);

                    pSibling = winPrev(winId);
                    while (pSibling != pOldWindow &&
                           hideRegion.pFirstTL2BR != UGL_NULL) {

                        if ((pSibling->state & WIN_STATE_HIDDEN) == 0x00) {
                            UGL_RECT_INTERSECT(
                                pSibling->rect,
                                winId->rect,
                                intersectRect
                                );

                            if (intersectRect.right >= intersectRect.left &&
                                intersectRect.bottom >= intersectRect.top) {

                                winRegionObscure(
                                    pSibling,
                                    UGL_NULL,
                                    &intersectRect
                                    );
                                uglRegionRectExclude(
                                    &hideRegion,
                                    &intersectRect
                                    );
                            }
                        }

                        /* Advance */
                        pSibling = winPrev(pSibling);
                    }

                    pSibling = winNext(winId);
                    while (pSibling != UGL_NULL) {
                        if ((pSibling->state & WIN_STATE_HIDDEN) == 0x00) {
                            UGL_RECT_INTERSECT(
                                pSibling->rect,
                                winId->rect,
                                intersectRect
                                );

                            if (intersectRect.right >= intersectRect.left &&
                                intersectRect.bottom >= intersectRect.top) {

                                uglRegionRectExclude(
                                    &exposeRegion,
                                    &intersectRect
                                    );
                            }
                        }

                        /* Advance */
                        pSibling = winNext(pSibling);
                    }

                    winRegionExpose(winId, &exposeRegion, &winId->rect);
                }

                uglRegionDeinit(&hideRegion);
                uglRegionDeinit(&exposeRegion);
            }

            /* Send zpos changed message */
            msg.type               = MSG_ZPOS_CHANGED;
            msg.data.zPos.oldPos   = oldPos;
            msg.data.zPos.newPos   = newPos;
            msg.data.zPos.changeId = winId;
            winMsgSend(winId, &msg);

            winUnlock(winId);
        }
    }

    return status;
}

/******************************************************************************
 *
 * winZPosGet - Get window depth position
 *
 * RETURNS: Window z position or 0
 */

UGL_ORD  winZPosGet (
    WIN_ID  winId
    ) {
    UGL_ORD       zPos;
    UGL_WINDOW *  pMatchWindow;

    if (winId == UGL_NULL || winId->pParent == UGL_NULL) {
        zPos = 0;
    }
    else {
        winLock(winId);

        zPos = 1;
        pMatchWindow = (UGL_WINDOW *) winLast(winId->pParent);
        while (pMatchWindow != UGL_NULL) {
            zPos++;
            pMatchWindow = winPrev(pMatchWindow);
        }

        winUnlock(winId);

        if (pMatchWindow == UGL_NULL) {
            zPos = 0;
        }
    }

    return zPos;
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
 * winAttach - Attach window to hiearachy
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winAttach (
    WIN_ID  childId,
    WIN_ID  parentId,
    WIN_ID  nextId
    ) {
    UGL_STATUS      status;
    UGL_ORD         zPos;
    WIN_MSG         msg;
    UGL_REG_DATA *  pRegData;
    WIN_MGR *       pWinMgr;

    if (childId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = UGL_STATUS_OK;
        if (childId->pParent != UGL_NULL) {
            if ((childId->attributes & WIN_ATTRIB_FRAMED) != 0x00) {
                childId = childId->pParent;
            }
            else {
                status = UGL_STATUS_ERROR;
            }

            if (childId->pParent != UGL_NULL) {
                status = UGL_STATUS_ERROR;
            }
        }

        if (status == UGL_STATUS_OK) {
            if (parentId == UGL_NULL) {
                pRegData = uglRegistryFind(UGL_WIN_MGR_TYPE, 0, 0, 0);
                if (pRegData == UGL_NULL) {
                    status = UGL_STATUS_ERROR;
                }
                else {
                    pWinMgr = (WIN_MGR *) pRegData->data;
                    if (pWinMgr->pRootWindow == UGL_NULL) {
                        status = UGL_STATUS_ERROR;
                    }
                    else {
                        parentId = pWinMgr->pRootWindow;
                    }
                }
            }

            if (status == UGL_STATUS_OK) {
                if (nextId == UGL_NULL) {
                    zPos = 1;
                }
                else {
                    zPos = winZPosGet(nextId) + 1;
                }

                winLock(parentId);

                uglListInsert(
                    &parentId->childList,
                    &childId->node,
                    uglListFirst(&parentId->childList)
                    );
                childId->pParent = parentId;

                /* Adjust z position */
                if (winCount(parentId) != zPos) {
                    winZPosSet(childId, zPos);
                }
                else {
                    msg.type             = MSG_ZPOS_CHANGED;
                    msg.data.zPos.newPos = zPos;
                    msg.data.zPos.oldPos = 0;
                    winMsgSend(childId, &msg);
                }

                /* Send attach message */
                winSend(parentId, MSG_ATTACH, &childId, sizeof(WIN_ID));

                winRectSet(childId, UGL_NULL);

                if ((parentId->state & WIN_STATE_MANAGED) != 0x00) {
                    winManage(childId);
                    if ((parentId->state & WIN_STATE_HIDDEN) == 0x00 &&
                        (childId->attributes & WIN_ATTRIB_VISIBLE) != 0x00) {

                        winShow(childId);
                    }
                }

                if (parentId->pActiveChild == UGL_NULL) {
                    if ((parentId->state & WIN_STATE_ACTIVE) != 0x00) {
                        winActivate(childId);
                    }
                    else {
                        parentId->pActiveChild = childId;
                    }
                }

               winUnlock(parentId);
            }
        }
    }

    return status;
}

/******************************************************************************
 *
 * winManage - Manage window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winManage (
    WIN_ID  winId
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pChild;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        winId->state |= WIN_STATE_MANAGED;
        winSend(winId, MSG_MANAGE, UGL_NULL, 0);

        /* Manage children */
        pChild = winFirst(winId);
        while (pChild != UGL_NULL) {
            winManage(pChild);

            /* Advance */
            pChild = winNext(pChild);
        }

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winShow - Show window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winShow (
    WIN_ID  winId
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pChild;
    UGL_WINDOW *  pSibling;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (winId->pParent == UGL_NULL) {
            uglRegionRectInclude(&winId->paintersRegion, &winId->rect);
        }
        else if ((winId->pParent->state & WIN_STATE_HIDDEN) == 0x00) {
            uglRegionCopy(
                &winId->pParent->paintersRegion,
                &winId->paintersRegion
                );

            pSibling = winNext(winId);
            while (pSibling != UGL_NULL) {
                if ((pSibling->state & WIN_STATE_HIDDEN) == 0x00) {
                    uglRegionRectExclude(
                        &winId->paintersRegion,
                        &pSibling->rect
                        );
                }

                /* Advance */
                pSibling = winNext(pSibling);
            }
        }
        else {
            uglRegionCopy(
                &winId->pParent->visibleRegion,
                &winId->paintersRegion
                );
        }

        uglRegionClipToRect(&winId->paintersRegion, &winId->rect);
        uglRegionMove(
            &winId->paintersRegion,
            -winId->rect.left,
            -winId->rect.top
            );

        /* Determine visible region and show children */
        pChild = winLast(winId);
        if (pChild != UGL_NULL) {
            uglRegionCopy(&winId->paintersRegion, &winId->visibleRegion);
            while (pChild != UGL_NULL) {
                if ((pChild->attributes & WIN_ATTRIB_VISIBLE) != 0x00) {
                    winShow(pChild);
                    if ((winId->attributes &
                         WIN_ATTRIB_CLIP_CHILDREN) != 0x00) {

                        uglRegionRectExclude(
                            &winId->visibleRegion,
                            &pChild->rect
                            );
                    }
                }

                /* Advance */
                pChild = winPrev(pChild);
            }

            /* Visible region required for clip children */
            if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) == 0x00) {
               uglRegionEmpty(&winId->visibleRegion);
            }
        }
        else if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00) {
            uglRegionCopy(&winId->paintersRegion, &winId->visibleRegion);
        }

        /* Mark window non-hidden */
        winId->state &= ~WIN_STATE_HIDDEN;

        winSend(winId, MSG_SHOW, UGL_NULL, 0);

        /* Invalidate window */
        if (winId->pParent == UGL_NULL ||
            (winId->pParent->state & WIN_STATE_HIDDEN) == 0x00) {

            winReveal(winId, UGL_NULL);
            winInvalidate(winId);
        }

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winDrawStart - Start window drawing
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_GC_ID  winDrawStart (
    WIN_ID     winId,
    UGL_GC_ID  gcId,
    UGL_BOOL   clipToDirty
    ) {
    WIN_MGR *     pWinMgr;
    UGL_GC *      pGc;
    UGL_POINT     deltaPoint;
    UGL_RECT      viewportRect;
    UGL_RECT      rect;
    UGL_REGION *  pClipRegion;
    UGL_REGION    clipRegion;

    if (winId == UGL_NULL) {
        pGc = UGL_NULL;
    }
    else {
        winLock(winId);

        /* Prepare graphics context */
        if (gcId == UGL_NULL) {
            pWinMgr = winId->pApp->pWinMgr;
            pGc = winId->pApp->pGc;

            pGc->rasterOp        = UGL_RASTER_OP_COPY;
            pGc->backgroundColor = pWinMgr->pColorTable[WIN_INDEX_BLACK];
            pGc->foregroundColor = pWinMgr->pColorTable[WIN_INDEX_WHITE];
            if (pWinMgr->pFontTable != UGL_NULL) {
                pGc->pFont = pWinMgr->pFontTable[WIN_FONT_INDEX_SYSTEM];
            }
            else {
                pGc->pFont = UGL_NULL;
            }
            pGc->lineWidth       = 1;
            pGc->lineStyle       = UGL_LINE_STYLE_SOLID;
            pGc->pPatternBitmap  = UGL_NULL;
        }
        else {
            pGc = gcId;
        }

        /* Set default bitmap and view */
        if ((winId->attributes & WIN_ATTRIB_OFF_SCREEN) != 0x00) {
            if ((winId->attributes & WIN_ATTRIB_DOUBLE_BUFFER) != 0x00) {
                uglDefaultBitmapSet(pGc, winId->dblBufBitmapId);
                viewportRect.left   = 0;
                viewportRect.top    = 0;
                viewportRect.right  = UGL_RECT_WIDTH(winId->rect) - 1;
                viewportRect.bottom = UGL_RECT_HEIGHT(winId->rect) - 1;
            }
            else {
               uglDefaultBitmapSet(
                   pGc,
                   winId->pApp->pWinMgr->offScreenBitmapId
                   );
               memcpy(&viewportRect, &winId->rect, sizeof(UGL_RECT));
               winWindowRectToScreen(winId->pParent, &viewportRect, 1);
               uglViewPortSet(
                   pGc,
                   viewportRect.left,
                   viewportRect.top,
                   viewportRect.right,
                   viewportRect.bottom
                   );
            }
        }
        else {
            uglDefaultBitmapSet(pGc, UGL_DISPLAY_ID);
            memcpy(&viewportRect, &winId->rect, sizeof(UGL_RECT));
            winWindowRectToScreen(winId->pParent, &viewportRect, 1);
            uglViewPortSet(
                pGc,
                viewportRect.left,
                viewportRect.top,
                viewportRect.right,
                viewportRect.bottom
                );
        }

        /* Setup clipping region */
        if (clipToDirty == UGL_TRUE) {
            pClipRegion = &winId->dirtyRegion;
        }
        else if ((winId->attributes & WIN_ATTRIB_DOUBLE_BUFFER) != 0x00) {
            pClipRegion = UGL_NULL;
        }
        else if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00) {
            pClipRegion = &winId->visibleRegion;
        }
        else {
            pClipRegion = &winId->paintersRegion;
        }

        uglClipRegionSet(pGc, pClipRegion);

        uglBatchStart(pGc);

        /* Draw background behind transparent windows */
        if ((winId->attributes & WIN_ATTRIB_TRANSPARENT) != 0x00 &&
            clipToDirty == UGL_TRUE) {

            rect.left   = viewportRect.left;
            rect.top    = viewportRect.top;
            rect.right  = rect.left + UGL_RECT_WIDTH(winId->pParent->rect) - 1;
            rect.bottom = rect.top + UGL_RECT_HEIGHT(winId->pParent->rect) - 1;

            deltaPoint.x = rect.left - winId->rect.left;
            deltaPoint.y = rect.top - winId->rect.top;
            UGL_RECT_MOVE(rect, -deltaPoint.x, -deltaPoint.y);

            uglRegionInit(&clipRegion);
            uglRegionCopy(pClipRegion, &clipRegion);
            uglRegionMove(&clipRegion, winId->rect.left, winId->rect.top);
            winBackgroundDraw(
                pGc,
                winId->pParent,
                winPrev(winId),
                &rect,
                &clipRegion,
                deltaPoint.x,
                deltaPoint.y
                );
            uglRegionDeinit(&clipRegion);

            uglViewPortSet(
                pGc,
                viewportRect.left,
                viewportRect.top,
                viewportRect.right,
                viewportRect.bottom
                );

            uglClipRegionSet(pGc, pClipRegion);
        }
    }

    return pGc;
}

/******************************************************************************
 *
 * winDrawEnd - End window drawing
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDrawEnd (
    WIN_ID     winId,
    UGL_GC_ID  gcId,
    UGL_BOOL   clearDirty
    ) {
    UGL_STATUS     status;
    UGL_RECT       winRect;
    UGL_BITMAP_ID  bmpId;

    if (winId == UGL_NULL || gcId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        uglBatchEnd(gcId);

        if ((winId->attributes & WIN_ATTRIB_OFF_SCREEN) != 0x00 ||
            ((winId->attributes & WIN_ATTRIB_TRANSPARENT) != 0x00 &&
            clearDirty == UGL_TRUE)) {

            memcpy(&winRect, &winId->rect, sizeof(UGL_RECT));
            uglDefaultBitmapGet(gcId, &bmpId);
            winWindowRectToScreen(winId->pParent, &winRect, 1);
            uglViewPortSet(
                gcId,
                winRect.left,
                winRect.top,
                winRect.right,
                winRect.bottom
                );
            if (clearDirty == UGL_TRUE) {
                uglClipRegionSet(gcId, &winId->dirtyRegion);
            }
            else if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00) {
                uglClipRegionSet(gcId, &winId->visibleRegion);
            }
            else {
                uglClipRegionSet(gcId, &winId->paintersRegion);
            }

            /* Copy window to display */
            if ((winId->attributes & WIN_ATTRIB_DOUBLE_BUFFER) != 0x00) {
                uglBitmapBlt(
                    gcId,
                    bmpId,
                    0,
                    0,
                    UGL_RECT_WIDTH(winId->rect) - 1,
                    UGL_RECT_HEIGHT(winId->rect) - 1,
                    UGL_DEFAULT_ID,
                    0,
                    0
                    );
            }
            else {
                uglBitmapBlt(
                    gcId,
                    bmpId,
                    winRect.left,
                    winRect.top,
                    winRect.right,
                    winRect.bottom,
                    UGL_DEFAULT_ID,
                    0,
                    0
                    );
            }
        }

        if (clearDirty == UGL_TRUE) {
            uglRegionEmpty(&winId->dirtyRegion);
            winDirtyClear(winId);
        }

        winUnlock(winId);

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winInvalidate - Invalidate window and children
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winInvalidate(
    WIN_ID  winId
    ) {
    UGL_STATUS    status;
    UGL_WINDOW *  pChild;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) == 0x00) {
            uglRegionCopy(&winId->paintersRegion, &winId->dirtyRegion);
            winDirtySet(winId);
        }

        pChild = winFirst(winId);
        while (pChild != UGL_NULL) {
            if ((pChild->state & WIN_STATE_HIDDEN) == 0x00) {
                winInvalidate(pChild);
            }

            /* Advance */
            pChild = winNext(pChild);
        }

        if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00 &&
            winId->visibleRegion.pFirstTL2BR != UGL_NULL) {

            uglRegionCopy(&winId->visibleRegion, &winId->dirtyRegion);
            winDirtySet(winId);
        }

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winRectInvalidate - Invalidate rectangle of window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winRectInvalidate (
    WIN_ID            winId,
    const UGL_RECT *  pRect
    ) {
    UGL_STATUS    status;
    UGL_RECT      rect;
    UGL_RECT      intersectRect;
    UGL_WINDOW *  pChild;

    if (winId == UGL_NULL || (winId->state & WIN_STATE_HIDDEN) != 0x00) {
        status = UGL_STATUS_ERROR;
    }
    else if (pRect == UGL_NULL) {
        memcpy(&rect, &winId->rect, sizeof(UGL_RECT));
        UGL_RECT_MOVE_TO(rect, 0, 0);
        status = winRectInvalidate(winId, &rect);
    }
    else {
        winLock(winId);

        uglRegionRectInclude(&winId->dirtyRegion, pRect);
        if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00) {
            status = uglRegionIntersect(
                &winId->dirtyRegion,
                &winId->visibleRegion,
                &winId->dirtyRegion
                );
            winDirtySet(winId);
        }
        else {
            status = uglRegionIntersect(
                &winId->dirtyRegion,
                &winId->paintersRegion,
                &winId->dirtyRegion
                );
            winDirtySet(winId);

            /* Process children */
            pChild = winFirst(winId);
            while (pChild != UGL_NULL) {
                if ((pChild->state & WIN_STATE_HIDDEN) == 0x00) {
                    UGL_RECT_INTERSECT(pChild->rect, *pRect, intersectRect);

                    if (intersectRect.right >= intersectRect.left &&
                        intersectRect.bottom >= intersectRect.top) {

                        UGL_RECT_MOVE(
                            intersectRect,
                            -pChild->rect.left,
                            -pChild->rect.top
                            );
                        winRectInvalidate(pChild, &intersectRect);
                    }
                }

                /* Advance */
                pChild = winNext(pChild);
            }
        }

        winUnlock(winId);

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winDirtySet - Mark window as dirty
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDirtySet (
    WIN_ID  winId
    ) {
    UGL_STATUS      status;
    WIN_RESOURCE *  pResource;
    UGL_WINDOW *    pWindow;
    WIN_APP *       pApp = winId->pApp;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if ((winId->state & WIN_STATE_DIRTY) == 0x00) {

            uglOSLock(pApp->lockId);
            winId->state |= WIN_STATE_DIRTY;
            uglListRemove(&pApp->resourceList, &winId->resource.node);

            /* Insert window after other dirty windows */
            pResource = (WIN_RESOURCE *) uglListFirst(&pApp->resourceList);
            while (pResource != NULL) {

                pWindow = (UGL_WINDOW *) pResource->id;
                if ((pWindow->state & WIN_STATE_DIRTY) == 0x00) {
                    break;
                }

                /* Advance */
                pResource = (WIN_RESOURCE *) uglListNext(&pResource->node);
            }

            uglListInsert(
                &pApp->resourceList,
                &winId->resource.node, 
                &pResource->node
                );

            uglOSUnlock(pApp->lockId);

            winWakeUp(pApp);
        }

        status = UGL_STATUS_OK;
    }

    return status;
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
    WIN_ID  winId
    ) {
    WIN_APP *  pApp = winId->pApp;

    if ((winId->state & WIN_STATE_DIRTY) != 0x00) {
        uglOSLock(pApp->lockId);

        /* Move to end of list */
        uglListRemove(&pApp->resourceList, &winId->resource.node);
        uglListAdd(&pApp->resourceList, &winId->resource.node);

        winId->state &= ~WIN_STATE_DIRTY;

        uglOSUnlock(pApp->lockId);
    }
}

/******************************************************************************
 *
 * winRegionExpose - Expose region of a window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS winRegionExpose (
    WIN_ID               winId,
    const UGL_REGION_ID  regionId,
    const UGL_RECT *     pRect
    ) {
    UGL_STATUS    status;
    UGL_REGION    exposeRegion;
    UGL_RECT      exposeRect;
    UGL_RECT      intersectRect;
    UGL_WINDOW *  pChild;

    if (winId == UGL_NULL || pRect == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        uglRegionInit(&exposeRegion);

        if (regionId != UGL_NULL) {
            uglRegionCopy(regionId, &exposeRegion);
        }
        else {
            uglRegionRectInclude(&exposeRegion, pRect);
        }

        uglRegionClipToRect(&exposeRegion, &winId->rect);
        uglRegionMove(&exposeRegion, -winId->rect.left, -winId->rect.top);
        uglRegionExclude(&exposeRegion, &winId->paintersRegion);

        /* Ensure that expose region is non-empty */
        if (exposeRegion.pFirstTL2BR != UGL_NULL) {
            memcpy(&exposeRect, pRect, sizeof(UGL_RECT));

            /* Expose rectangle relative to window */
            UGL_RECT_INTERSECT(exposeRect, winId->rect, exposeRect);
            UGL_RECT_MOVE(exposeRect, -winId->rect.left, -winId->rect.top);

            /* Expose window */
            uglRegionUnion(
                &winId->paintersRegion,
                &exposeRegion,
                &winId->paintersRegion
                );

            if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) == 0x00) {
                uglRegionUnion(
                    &winId->dirtyRegion,
                    &exposeRegion,
                    &winId->dirtyRegion
                    );

                winDirtySet(winId);
            }

            /* Expose child windows */
            pChild = winLast(winId);
            while (pChild != UGL_NULL && exposeRegion.pFirstTL2BR != UGL_NULL) {

                if ((pChild->state & WIN_STATE_HIDDEN) == 0x00) {
                    UGL_RECT_INTERSECT(pChild->rect, exposeRect, intersectRect);
                }

                if (intersectRect.right >= intersectRect.left &&
                    intersectRect.bottom >= intersectRect.top) {

                    winRegionExpose(pChild, &exposeRegion, &intersectRect);
                    uglRegionRectExclude(&exposeRegion, &intersectRect);
                }

                /* Advance */
                pChild = winPrev(pChild);
            }

            if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00 &&
                exposeRegion.pFirstTL2BR != UGL_NULL) {

                uglRegionUnion(
                    &winId->visibleRegion,
                    &exposeRegion,
                    &winId->visibleRegion
                    );
                uglRegionUnion(
                    &winId->dirtyRegion,
                    &exposeRegion,
                    &winId->dirtyRegion
                    );

                winDirtySet(winId);
            }
        }

        uglRegionDeinit(&exposeRegion);
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winReveal - Reveal region of window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winReveal (
    WIN_ID         winId,
    UGL_REGION_ID  regionId
    ) {
    UGL_STATUS    status;
    UGL_RECT      intersectRect;
    UGL_REGION    hideRegion;
    UGL_WINDOW *  pParent;
    UGL_WINDOW *  pChild;
    UGL_WINDOW *  pSibling;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pParent = winId->pParent;
        pChild  = winId;

        uglRegionInit(&hideRegion);
        if (regionId != UGL_NULL) {
            uglRegionCopy(regionId, &hideRegion);
        }
        else {
            uglRegionCopy(&pChild->paintersRegion, &hideRegion);
            uglRegionMove(&hideRegion, pChild->rect.left, pChild->rect.right);
        }

        /* Conceal parent */
        if (pParent != UGL_NULL &&
            (pParent->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00) {

            uglRegionExclude(&pParent->visibleRegion, &hideRegion);
            uglRegionExclude(&pParent->dirtyRegion, &hideRegion);
        }

        /* Conceal siblings */
        pSibling = winPrev(pChild);
        while (pSibling  != UGL_NULL && hideRegion.pFirstTL2BR != UGL_NULL) {
            if ((pSibling->state & WIN_STATE_HIDDEN) == 0x00 &&
                pSibling->paintersRegion.pFirstTL2BR != UGL_NULL) {

                UGL_RECT_INTERSECT(
                    pChild->rect,
                    pSibling->rect,
                    intersectRect
                    );

                if (intersectRect.right >= intersectRect.left &&
                    intersectRect.bottom >= intersectRect.top) {

                    if (regionId != UGL_NULL) {
                        winRegionObscure(pSibling, &hideRegion, &intersectRect);
                    }
                    else {
                        winRegionObscure(pSibling, UGL_NULL, &intersectRect);
                    }

                    uglRegionRectExclude(&hideRegion, &intersectRect);
                }
            }

            /* Advance */
            pSibling = winPrev(pSibling);
        }

        uglRegionDeinit(&hideRegion);

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winRegionObscure - Obscure region of window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winRegionObscure (
    WIN_ID               winId,
    const UGL_REGION_ID  regionId,
    const UGL_RECT *     pRect
    ) {
    UGL_STATUS    status;
    UGL_RECT      obscureRect;
    UGL_RECT      intersectRect;
    UGL_REGION    obscureRegion;
    UGL_WINDOW *  pChild;

    if (winId == UGL_NULL || pRect == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        uglRegionInit(&obscureRegion);
        if (regionId != UGL_NULL) {
            uglRegionCopy(regionId, &obscureRegion);
        }
        else {
            uglRegionRectInclude(&obscureRegion, pRect);
        }
        uglRegionMove(&obscureRegion, -winId->rect.left, -winId->rect.top);
        uglRegionIntersect(
            &obscureRegion,
            &winId->paintersRegion,
            &obscureRegion
            );

        if (obscureRegion.pFirstTL2BR != UGL_NULL) {
            memcpy(&obscureRect, pRect, sizeof(UGL_RECT));
            UGL_RECT_INTERSECT(obscureRect, winId->rect, obscureRect);
            UGL_RECT_MOVE(obscureRect, -winId->rect.left, -winId->rect.top);

            uglRegionExclude(&winId->paintersRegion, &obscureRegion);
            uglRegionExclude(&winId->dirtyRegion, &obscureRegion);

            /* Obscure children */
            pChild = winLast(winId);
            while (pChild != UGL_NULL &&
                   obscureRegion.pFirstTL2BR != UGL_NULL) {

                if ((pChild->state & WIN_STATE_HIDDEN) == 0x00 &&
                    pChild->paintersRegion.pFirstTL2BR != UGL_NULL) {

                    UGL_RECT_INTERSECT(
                        pChild->rect,
                        obscureRect,
                        intersectRect
                        );

                    if (intersectRect.right >= intersectRect.left &&
                        intersectRect.bottom >= intersectRect.top) {

                        if (regionId != UGL_NULL) {
                            winRegionObscure(
                                pChild,
                                &obscureRegion,
                                &intersectRect
                                );
                        }
                        else {
                            winRegionObscure(
                                pChild,
                                UGL_NULL,
                                &intersectRect
                                );
                        }

                        uglRegionRectExclude(&obscureRegion, &intersectRect);
                    }
                }

                /* Advance */
                pChild = winPrev(pChild);
            }
        }

        if ((winId->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00 &&
            obscureRegion.pFirstTL2BR != UGL_NULL) {

            uglRegionExclude(&winId->visibleRegion, &obscureRegion);
        }

        uglRegionDeinit(&obscureRegion);

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winConceal - Conceal region of window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winConceal (
    WIN_ID         winId,
    UGL_REGION_ID  regionId
    ) {
    UGL_STATUS    status;
    UGL_RECT      intersectRect;
    UGL_REGION    exposeRegion;
    UGL_WINDOW *  pParent;
    UGL_WINDOW *  pChild;
    UGL_WINDOW *  pSibling;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pParent = winId->pParent;
        pChild  = winId;

        uglRegionInit(&exposeRegion);
        if (regionId != UGL_NULL) {
            uglRegionCopy(regionId, &exposeRegion);
        }
        else {
            uglRegionCopy(&pChild->paintersRegion, &exposeRegion);
            uglRegionMove(&exposeRegion, pChild->rect.left, pChild->rect.top);
        }

        if ((pParent->attributes & WIN_ATTRIB_CLIP_CHILDREN) == 0x00) {
            uglRegionUnion(
                &pParent->dirtyRegion,
                &exposeRegion,
                &pParent->dirtyRegion
                );
            winDirtySet(pParent);
        }

        /* Expose siblings */
        pSibling = winPrev(pChild);
        while (pSibling != UGL_NULL && exposeRegion.pFirstTL2BR != UGL_NULL) {
            UGL_RECT_INTERSECT(pChild->rect, pSibling->rect, intersectRect);
            if (intersectRect.right >= intersectRect.left &&
                intersectRect.bottom >= intersectRect.top) {

                winRegionExpose(pSibling, &exposeRegion, &intersectRect);
                uglRegionRectExclude(&exposeRegion, &intersectRect);
            }

            /* Advance */
            pSibling = winPrev(pSibling);
        }

        /* Expose parent window */
        if ((pParent->attributes & WIN_ATTRIB_CLIP_CHILDREN) != 0x00 &&
            exposeRegion.pFirstTL2BR != UGL_NULL) {

            uglRegionUnion(
                &pParent->visibleRegion,
                &exposeRegion,
                &pParent->visibleRegion
                );
            uglRegionUnion(
                &pParent->dirtyRegion,
                &exposeRegion,
                &pParent->dirtyRegion
                );
        }

        uglRegionDeinit(&exposeRegion);

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winBackgroundDraw - Draw background for transparent windows
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_VOID  winBackgroundDraw (
    UGL_GC_ID     gcId,
    WIN_ID        winId,
    WIN_ID        childId,
    UGL_RECT *    pViewport,
    UGL_REGION *  pClipRegion,
    UGL_ORD       dx,
    UGL_ORD       dy
    ) {
    UGL_RECT      intersectRect;
    WIN_MSG       msg;
    UGL_WINDOW *  pWindow = winId;
    UGL_WINDOW *  pChild = childId;

    while (pChild != UGL_NULL && pClipRegion->pFirstTL2BR != UGL_NULL) {
        if ((pChild->state & WIN_STATE_HIDDEN) != 0x00) {
            memcpy(&intersectRect, &pChild->rect, sizeof(UGL_RECT));
            UGL_RECT_MOVE(intersectRect, dx, dy);
            UGL_RECT_INTERSECT(*pViewport, intersectRect, intersectRect);

            if (intersectRect.right >= intersectRect.left &&
                intersectRect.bottom >= intersectRect.top) {

                UGL_RECT_MOVE(*pViewport, pChild->rect.left, pChild->rect.top);
                uglRegionMove(
                    pClipRegion,
                    -pChild->rect.left,
                    -pChild->rect.top
                    );
                winBackgroundDraw(
                    gcId,
                    pChild,
                    winLast(pChild),
                    pViewport,
                    pClipRegion,
                    dx + pChild->rect.left,
                    dy + pChild->rect.top
                    );
                UGL_RECT_MOVE(intersectRect, -pViewport->left, -pViewport->top);
                uglRegionRectExclude(pClipRegion, &intersectRect);
                UGL_RECT_MOVE(
                    *pViewport,
                    -pChild->rect.left,
                    -pChild->rect.top
                    );
                uglRegionMove(pClipRegion, pChild->rect.left, pChild->rect.top);
            }
        }

        /* Advance */
        pChild = winPrev(pChild);
    }

    if (pClipRegion->pFirstTL2BR != UGL_NULL) {
        uglViewPortSet(
            gcId,
            pViewport->left,
            pViewport->top,
            pViewport->right,
            pViewport->bottom
            );
        uglClipRegionSet(gcId, pClipRegion);

        msg.type = MSG_DRAW;
        msg.data.draw.gcId = gcId;
        memcpy(&msg.data.draw.rect, &pWindow->rect, sizeof(UGL_RECT));
        msg.data.draw.displayId = pWindow->pApp->pWinMgr->pDisplay;
        winMsgSend(pWindow, &msg);
    }
}

/******************************************************************************
 *
 * winClassInit - Initialize class hierarcy for window
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  winClassInit (
    WIN_ID  winId
    ) {
    WIN_MSG      msg;
    WIN_CLASS *  pClass = winId->pClass;

    if (pClass != UGL_NULL) {

        if (winId->pParent != UGL_NULL) {
            winClassInit(winId->pParent);
        }

        if (pClass->useCount++ == 0) {
            msg.type  = MSG_CLASS_INIT;
            msg.winId = winId;

            (*pClass->pMsgHandler) (
                winId,
                pClass,
                &msg,
                (UGL_INT8 *) winId->pClassData + pClass->dataOffset
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

