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

/* wwmFrame.c - Window manager frame class */

#include <string.h>
#include "ugl.h"
#include "winMgr/wwm/wwm.h"

/* Types */

typedef struct wwm_frame_data {
    void *         pTitleText;
    UGL_RECT       contentRect;
    UGL_RECT       closeRect;
    UGL_RECT       minRect;
    UGL_RECT       maxRect;
    UGL_RECT       captionRect;
    UGL_RECT       restoreRect;
    UGL_BITMAP_ID  imageId;
    WIN_ID         taskButtonId;
    UGL_UINT32     buttonState;
} WWM_FRAME_DATA;

typedef struct wwm_frame_class_data {
    UGL_FONT_ID    fontId;
    UGL_COLOR      frameColorActive;
    UGL_COLOR      frameColorInactive;
    UGL_COLOR      textColorActive;
    UGL_COLOR      textColorInactive;
    UGL_COLOR      shineColorActive;
    UGL_COLOR      shineColorInactive;
    UGL_COLOR      shadowColorActive;
    UGL_COLOR      shadowColorInactive;
} WWM_FRAME_CLASS_DATA;

/* Globals */

UGL_RECT  wwmMoveBoundary = {
         0,
    -10000,
     10000,
     10000
};

/* Locals */

UGL_LOCAL WWM_FRAME_CLASS_DATA  wwmFrameClassData;
UGL_LOCAL WIN_CLASS_ID          wwmFrameClassId = UGL_NULL;

UGL_LOCAL UGL_VOID  wwmFrame3DRectDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    const UGL_RECT *  pRect,
    UGL_POS           depth,
    WWM_FRAME_DATA *  pFrameData
    );

UGL_LOCAL UGL_VOID  wwmFrameBorderDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    UGL_RECT *        pRect,
    WWM_FRAME_DATA *  pFrameData
    );

UGL_LOCAL UGL_VOID  wwmFrameCloseIconDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    WWM_FRAME_DATA *  pFrameData
    );

UGL_LOCAL UGL_VOID  wwmFrameMinIconDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    WWM_FRAME_DATA *  pFrameData
    );

UGL_LOCAL UGL_VOID  wwmFrameMaxIconDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    WWM_FRAME_DATA *  pFrameData
    );

UGL_LOCAL UGL_VOID  wwmFrameDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    const UGL_RECT *  pRect,
    WWM_FRAME_DATA *  pFrameData
    );

UGL_LOCAL UGL_VOID  wwmFrameInit (
    WIN_ID            winId,
    WWM_FRAME_DATA *  pFrameData
    );

UGL_LOCAL UGL_STATUS  wwmMoveSizeCallback (
    WIN_ID     winId,
    WIN_MSG *  pMsg,
    void *     pData,
    void *     pParam
    );

UGL_LOCAL UGL_STATUS  wwmFrameMsgHandler (
    WIN_ID            winId,
    WIN_CLASS_ID      classId,
    WIN_MSG *         pMsg,
    WWM_FRAME_DATA *  pFrameData
    );

/******************************************************************************
 *
 * wwmFrameClassCreate - Create window manager frame class
 *
 * RETURNS: Pointer to display or UGL_NULL
 */

WIN_CLASS_ID  wwmFrameClassCreate (
    void
    ) {
    wwmFrameClassId = winClassCreate (
        "wwmFrameClass",
        UGL_NULL,
        (WIN_MSG_HANDLER *) wwmFrameMsgHandler,
        sizeof(WWM_FRAME_DATA),
        UGL_NULL
        );

    return wwmFrameClassId;
}

/******************************************************************************
 *
 * wwmFrame3DRectDraw - Draw 3D frame rectangle
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  wwmFrame3DRectDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    const UGL_RECT *  pRect,
    UGL_POS           depth,
    WWM_FRAME_DATA *  pFrameData
    ) {
    UGL_COLOR  topLeftColor;
    UGL_COLOR  bottomRightColor;

    if (winIsActive(winId) == UGL_TRUE) {
        if (depth < 0) {
            topLeftColor     = wwmFrameClassData.shadowColorActive;
            bottomRightColor = wwmFrameClassData.shineColorActive;
        }
        else if (depth > 0) {
            topLeftColor     = wwmFrameClassData.shineColorActive;
            bottomRightColor = wwmFrameClassData.shadowColorActive;
        }
        else {
            topLeftColor     = wwmFrameClassData.frameColorActive;
            bottomRightColor = wwmFrameClassData.frameColorActive;
        }
    }
    else {
        if (depth < 0) {
            topLeftColor     = wwmFrameClassData.shadowColorInactive;
            bottomRightColor = wwmFrameClassData.shineColorInactive;
        }
        else if (depth > 0) {
            topLeftColor     = wwmFrameClassData.shineColorInactive;
            bottomRightColor = wwmFrameClassData.shadowColorInactive;
        }
        else {
            topLeftColor     = wwmFrameClassData.frameColorInactive;
            bottomRightColor = wwmFrameClassData.frameColorInactive;
        }
    }

    uglLineWidthSet(gcId, 1);

    uglForegroundColorSet(gcId, topLeftColor);
    uglLine(gcId, pRect->left + 1, pRect->top, pRect->right, pRect->top);
    uglLine(gcId, pRect->left, pRect->top, pRect->left, pRect->bottom);

    uglForegroundColorSet(gcId, bottomRightColor);
    uglLine(gcId, pRect->left + 1, pRect->bottom, pRect->right, pRect->bottom);
    uglLine(gcId, pRect->right, pRect->top, pRect->right, pRect->bottom);
}

/******************************************************************************
 *
 * wwmFrameBorderDraw - Draw frame border
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  wwmFrameBorderDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    UGL_RECT *        pRect,
    WWM_FRAME_DATA *  pFrameData
    ) {
    UGL_ORD  i;

    wwmFrame3DRectDraw(winId, gcId, pRect, 1, pFrameData);
    WWM_RECT_DECREMENT(*pRect);

    uglBackgroundColorSet(gcId, UGL_COLOR_TRANSPARENT);
    if (winIsActive(winId) == UGL_TRUE) {
        uglForegroundColorSet(gcId, wwmFrameClassData.frameColorActive);
    }
    else {
        uglForegroundColorSet(gcId, wwmFrameClassData.frameColorInactive);
    }

    for (i = 0; i < WWM_FRAME_BORDER_SIZE - 2; i++) {
        uglRectangle(
            gcId,
            pRect->left,
            pRect->top,
            pRect->right,
            pRect->bottom
            );
        WWM_RECT_DECREMENT(*pRect);
    }

    wwmFrame3DRectDraw(winId, gcId, pRect, -1, pFrameData);
}

/******************************************************************************
 *
 * wwmFrameCloseIconDraw - Draw frame close icon
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  wwmFrameCloseIconDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    WWM_FRAME_DATA *  pFrameData
    ) {
    UGL_RECT  rect;

    rect.left   = (pFrameData->closeRect.left +
                   pFrameData->closeRect.right) / 2 - 3;
    rect.top    = (pFrameData->closeRect.top +
                   pFrameData->closeRect.bottom) / 2 - 3;
    rect.right  = rect.left + 7;
    rect.bottom = rect.top + 6;

    wwmFrame3DRectDraw(winId, gcId, &pFrameData->closeRect, 1, pFrameData);

    uglForegroundColorSet(gcId, WIN_BLACK);
    uglLine(gcId, rect.left, rect.top, rect.right - 1, rect.bottom);
    uglLine(gcId, rect.right - 1, rect.top, rect.left, rect.bottom);
    uglLine(gcId, rect.left + 1, rect.top, rect.right, rect.bottom);
    uglLine(gcId, rect.right, rect.top, rect.left + 1, rect.bottom);
}

/******************************************************************************
 *
 * wwmFrameMinIconDraw - Draw frame minimize icon
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  wwmFrameMinIconDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    WWM_FRAME_DATA *  pFrameData
    ) {
    UGL_RECT  rect;
    UGL_POS   points[8];

    rect.left   = (pFrameData->minRect.left +
                   pFrameData->minRect.right) / 2 - 3;
    rect.top    = (pFrameData->minRect.top +
                   pFrameData->minRect.bottom) / 2 - 1;
    rect.right  = rect.left + 6;
    rect.bottom = rect.top + 3;

    wwmFrame3DRectDraw(winId, gcId, &pFrameData->minRect, 1, pFrameData);

    points[0] = rect.left;
    points[1] = rect.top;
    points[2] = rect.right;
    points[3] = rect.top;
    points[4] = rect.left + 3;
    points[5] = rect.bottom;
    points[6] = rect.left;
    points[7] = rect.top;

    uglBackgroundColorSet(gcId, WIN_BLACK);
    uglForegroundColorSet(gcId, UGL_COLOR_TRANSPARENT);

    uglPolygon(gcId, 4, points);
}

/******************************************************************************
 *
 * wwmFrameMaxIconDraw - Draw frame maximize icon
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  wwmFrameMaxIconDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    WWM_FRAME_DATA *  pFrameData
    ) {
    UGL_RECT  rect;
    UGL_POS   points[8];

    rect.left   = (pFrameData->maxRect.left +
                   pFrameData->maxRect.right) / 2 - 3;
    rect.top    = (pFrameData->maxRect.top +
                   pFrameData->maxRect.bottom) / 2 - 2;
    rect.right  = rect.left + 6;
    rect.bottom = rect.top + 3;

    wwmFrame3DRectDraw(winId, gcId, &pFrameData->maxRect, 1, pFrameData);

    points[0] = rect.left;
    points[1] = rect.bottom;
    points[2] = rect.right;
    points[3] = rect.bottom;
    points[4] = rect.left + 3;
    points[5] = rect.top;
    points[6] = rect.left;
    points[7] = rect.bottom;

    uglBackgroundColorSet(gcId, WIN_BLACK);
    uglForegroundColorSet(gcId, UGL_COLOR_TRANSPARENT);

    uglPolygon(gcId, 4, points);
}

/******************************************************************************
 *
 * wwmFrameDraw - Draw window frame
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  wwmFrameDraw (
    WIN_ID            winId,
    UGL_GC_ID         gcId,
    const UGL_RECT *  pRect,
    WWM_FRAME_DATA *  pFrameData
    ) {
    UGL_RECT    rect;
    UGL_UINT32  attributes;

    memcpy(&rect, pRect, sizeof(UGL_RECT));
    attributes = winAttribGet(winId);

    wwmFrameBorderDraw(winId, gcId, &rect, pFrameData);
    rect.bottom = pFrameData->captionRect.bottom;

    if (winIsActive(winId) == UGL_TRUE) {
        uglBackgroundColorSet(gcId, wwmFrameClassData.frameColorActive);
    }
    else {
        uglBackgroundColorSet(gcId, wwmFrameClassData.frameColorInactive);
    }
    uglForegroundColorSet(gcId, UGL_COLOR_TRANSPARENT);
    uglRectangle(gcId, rect.left, rect.top, rect.right, rect.bottom);
    uglClipRectSet(gcId, rect.left, rect.top, rect.right, rect.bottom);

    if ((attributes & WIN_ATTRIB_NO_CLOSE) == 0x00) {
        wwmFrameCloseIconDraw(winId, gcId, pFrameData);
    }

    if ((attributes & WIN_ATTRIB_NO_MINIMIZE) == 0x00) {
        wwmFrameMinIconDraw(winId, gcId, pFrameData);
    }

    if ((attributes & WIN_ATTRIB_NO_MAXIMIZE) == 0x00) {
        wwmFrameMaxIconDraw(winId, gcId, pFrameData);
    }

    rect.left = pFrameData->captionRect.left;
    wwmFrame3DRectDraw(winId, gcId, &rect, 1, pFrameData);
    WWM_RECT_DECREMENT(rect);
    uglClipRectSet(gcId, rect.left, rect.top, rect.right, rect.bottom);

    if (winIsActive(winId) == UGL_TRUE) {
        uglForegroundColorSet(gcId, wwmFrameClassData.textColorActive);
    }
    else {
        uglForegroundColorSet(gcId, wwmFrameClassData.textColorInactive);
    }

    uglFontSet(gcId, wwmFrameClassData.fontId);
    uglBackgroundColorSet(gcId, UGL_COLOR_TRANSPARENT);
    uglTextDraw(
        gcId,
        rect.left + 2,
        rect.top + 1,
        -1,
        pFrameData->pTitleText
        );
}

/******************************************************************************
 *
 * wwmFrameInit - Initialize frame
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  wwmFrameInit (
    WIN_ID            winId,
    WWM_FRAME_DATA *  pFrameData
    ) {
    UGL_RECT    rect;
    UGL_SIZE    captionHeight;
    UGL_UINT32  attributes;

    uglTextSizeGet(
        wwmFrameClassData.fontId,
        UGL_NULL,
        &captionHeight,
        -1,
        pFrameData->pTitleText
        );
    captionHeight += 4;

    rect.left   = WWM_FRAME_BORDER_SIZE;
    rect.top    = WWM_FRAME_BORDER_SIZE;
    rect.right  = rect.left + captionHeight;
    rect.bottom = rect.top + captionHeight - 1;

    attributes = winAttribGet(winId);
    if ((attributes & WIN_ATTRIB_NO_CLOSE) == 0x00) {
        memcpy(&pFrameData->closeRect, &rect, sizeof(UGL_RECT));
        pFrameData->closeRect.right++;
        UGL_RECT_MOVE(rect, captionHeight + 2, 0);
    }
    else {
        pFrameData->closeRect.left   = -1;
        pFrameData->closeRect.top    = -1;
        pFrameData->closeRect.right  = -1;
        pFrameData->closeRect.bottom = -1;
    }

    if ((attributes & WIN_ATTRIB_NO_MINIMIZE) == 0x00) {
        memcpy(&pFrameData->minRect, &rect, sizeof(UGL_RECT));
        UGL_RECT_MOVE(rect, captionHeight + 1, 0);
    }
    else {
        pFrameData->minRect.left   = -1;
        pFrameData->minRect.top    = -1;
        pFrameData->minRect.right  = -1;
        pFrameData->minRect.bottom = -1;
    }

    if ((attributes & WIN_ATTRIB_NO_MAXIMIZE) == 0x00 &&
        (attributes & WIN_ATTRIB_NO_RESIZE) == 0x00) {

        memcpy(&pFrameData->maxRect, &rect, sizeof(UGL_RECT));
        UGL_RECT_MOVE(rect, captionHeight + 1, 0);
    }
    else {
        pFrameData->maxRect.left   = -1;
        pFrameData->maxRect.top    = -1;
        pFrameData->maxRect.right  = -1;
        pFrameData->maxRect.bottom = -1;
    }

    winDrawRectGet(winId, &pFrameData->captionRect);
    pFrameData->captionRect.left    = rect.left;
    pFrameData->captionRect.top    += WWM_FRAME_BORDER_SIZE;
    pFrameData->captionRect.right  -= WWM_FRAME_BORDER_SIZE;
    pFrameData->captionRect.bottom  = pFrameData->captionRect.top +
        captionHeight - 1;

    winDrawRectGet(winId, &pFrameData->contentRect);
    pFrameData->contentRect.left   += WWM_FRAME_BORDER_SIZE;
    pFrameData->contentRect.top     = pFrameData->captionRect.bottom + 1;
    pFrameData->contentRect.right  -= WWM_FRAME_BORDER_SIZE;
    pFrameData->captionRect.bottom -= WWM_FRAME_BORDER_SIZE;
}

/******************************************************************************
 *
 * wwmMoveSizeCallback
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  wwmMoveSizeCallback (
    WIN_ID     winId,
    WIN_MSG *  pMsg,
    void *     pData,
    void *     pParam
    ) {
    static UGL_ORD     dy        = 0;
    static UGL_UINT32  hitResult = 0x00;

    UGL_STATUS  status;

    switch (pMsg->type) {
        case MSG_PTR_BTN1_DOWN:
        case MSG_PTR_BTN2_DOWN:
            winPointerGrab(winId);
            dy = 0;

            /* TODO: Why do I need this? */
            uglOSTaskDelay(16);

            /* falltru */

        case MSG_PTR_MOVE: {
            WIN_MSG        hitMsg;
            UGL_DEVICE_ID  displayId;

            /* Send get hit message to window */
            hitMsg.type = MSG_HIT_TEST;
            memcpy(
                &hitMsg.data.hitTest.position,
                &pMsg->data.ptr.position,
                sizeof(UGL_POINT)
                );
            hitMsg.data.hitTest.hitResult = 0x00;
            winMsgSend(winId, &hitMsg);

            hitResult = hitMsg.data.hitTest.hitResult;
            displayId = winDisplayGet(winId);

            /* Process hit message */
            if (hitResult == 0x00) {
                uglCursorImageSet(displayId, WIN_CURSOR_ARROW);
            }
            else if (hitResult == WIN_HIT_TEST_MOVE) {
                uglCursorImageSet(displayId, WIN_CURSOR_ARROW);
            }
            else if (hitResult == WIN_HIT_TEST_LEFT ||
                     hitResult == WIN_HIT_TEST_RIGHT) {
                uglCursorImageSet(displayId, WIN_CURSOR_SIZE_HORIZ);
            }
            else if (hitResult == WIN_HIT_TEST_TOP ||
                     hitResult == WIN_HIT_TEST_BOTTOM) {
                uglCursorImageSet(displayId, WIN_CURSOR_SIZE_VERT);
            }
            else if (hitResult ==
                     (WIN_HIT_TEST_TOP | WIN_HIT_TEST_LEFT) ||
                     hitResult ==
                     (WIN_HIT_TEST_BOTTOM | WIN_HIT_TEST_RIGHT)) {
                uglCursorImageSet(displayId, WIN_CURSOR_SIZE_TL_BR);
            }
            else if (hitResult ==
                     (WIN_HIT_TEST_TOP | WIN_HIT_TEST_RIGHT) ||
                     hitResult ==
                     (WIN_HIT_TEST_BOTTOM | WIN_HIT_TEST_LEFT)) {
                uglCursorImageSet(displayId, WIN_CURSOR_SIZE_TR_BL);
            }
            status = UGL_STATUS_OK;
            } break;

        case MSG_PTR_DRAG:
            if (winPointerGrabGet(winMgrGet(winId)) != winId) {
                status = UGL_STATUS_OK;
                break;
            }

            if (hitResult != 0x00 &&
                (pMsg->data.ptr.buttonState &
                 (UGL_PTR_BUTTON1 | UGL_PTR_BUTTON2)) != 0x00) {
                UGL_RECT    rect;
                WIN_ID      dirtyWinId;
                WIN_APP_ID  appId;

                winRectGet(winId, &rect);

                if ((hitResult & WIN_HIT_TEST_LEFT) != 0x00) {
                    if ((hitResult & WIN_HIT_TEST_RIGHT) != 0x00) {
                        rect.left += pMsg->data.ptr.delta.x;
                    }
                    else if ((
                        (rect.right - rect.left + pMsg->data.ptr.delta.x + 1) >
                        WWM_FRAME_MIN_WIDTH) ||
                        (pMsg->data.ptr.position.x < WWM_FRAME_BORDER_SIZE)) {
                        rect.left += pMsg->data.ptr.position.x;
                    }
                }

                if ((hitResult & WIN_HIT_TEST_TOP) != 0x00) {
                    if ((hitResult & WIN_HIT_TEST_BOTTOM) != 0x00) {
                        rect.top += pMsg->data.ptr.delta.y;
                    }
                    else if ((
                        (rect.bottom - rect.top + pMsg->data.ptr.delta.y + 1) >
                        WWM_FRAME_MIN_HEIGHT) ||
                        (pMsg->data.ptr.position.y < WWM_FRAME_BORDER_SIZE)) {
                        rect.top += pMsg->data.ptr.position.y;
                    }

                    rect.top -= dy;
                }

                if ((hitResult & WIN_HIT_TEST_RIGHT) != 0x00) {
                    if ((hitResult & WIN_HIT_TEST_LEFT) != 0x00) {
                        rect.right += pMsg->data.ptr.delta.x;
                    }
                    else if (pMsg->data.ptr.position.x > WWM_FRAME_MIN_WIDTH) {
                        rect.right = pMsg->data.ptr.position.x + rect.left;
                    }
                }

                if ((hitResult & WIN_HIT_TEST_BOTTOM) != 0x00) {
                    if ((hitResult & WIN_HIT_TEST_TOP) != 0x00) {
                        rect.bottom += pMsg->data.ptr.delta.y;
                    }
                    else if (pMsg->data.ptr.position.y > WWM_FRAME_MIN_HEIGHT) {
                        rect.bottom = pMsg->data.ptr.position.y + rect.top;
                    }

                    rect.bottom -= dy;
                }

                dy = 0;
                if (rect.top < wwmMoveBoundary.top) {
                    dy = wwmMoveBoundary.top - rect.top;

                    rect.top    += dy;
                    rect.bottom += dy;
                }

                winRectSet(winId, &rect);

                /* Refresh window view */
                appId = winAppGet(winId);
                while ((dirtyWinId = winDirtyGet(appId)) != UGL_NULL) {
                    winUpdate(dirtyWinId);
                }

                /* Sleep for a while */
                uglOSTaskDelay(16);

                status = UGL_STATUS_FINISHED;
            }
            else {
                status = UGL_STATUS_OK;
            }
            break;

        case MSG_PTR_BTN1_UP:
        case MSG_PTR_BTN2_UP:
            winPointerUngrab(winId);
            status = UGL_STATUS_OK;
            break;
    }

    return status;
}

/******************************************************************************
 *
 * wwmFrameMsgHandler - Frame class message handler
 *
 * RETURNS: Pointer to display or UGL_NULL
 */

UGL_LOCAL UGL_STATUS  wwmFrameMsgHandler (
    WIN_ID            winId,
    WIN_CLASS_ID      classId,
    WIN_MSG *         pMsg,
    WWM_FRAME_DATA *  pFrameData
    ) {

    switch (pMsg->type) {

        case MSG_HIT_TEST: {
            UGL_RECT  rect;

            winDrawRectGet(winId, &rect);

            if (winFrameResizableGet(winId) == UGL_TRUE) {
                if (pMsg->data.hitTest.position.x <= WWM_FRAME_BORDER_SIZE) {
                    pMsg->data.hitTest.hitResult |= WIN_HIT_TEST_LEFT;
                }
                else if (pMsg->data.hitTest.position.x >=
                         rect.right - WWM_FRAME_BORDER_SIZE) {
                    pMsg->data.hitTest.hitResult |= WIN_HIT_TEST_RIGHT;
                }

                if (pMsg->data.hitTest.position.y <= WWM_FRAME_BORDER_SIZE) {
                    pMsg->data.hitTest.hitResult |= WIN_HIT_TEST_TOP;
                }
                else if (pMsg->data.hitTest.position.y >=
                         rect.bottom - WWM_FRAME_BORDER_SIZE) {
                    pMsg->data.hitTest.hitResult |= WIN_HIT_TEST_BOTTOM;
                }
            }
            else {
                pMsg->data.hitTest.hitResult = 0x00;
            }

            if (pMsg->data.hitTest.hitResult == 0x00 &&
                UGL_POINT_IN_RECT(
                    pMsg->data.hitTest.position,
                    pFrameData->captionRect
                    ) == UGL_TRUE) {
                pMsg->data.hitTest.hitResult |= WIN_HIT_TEST_MOVE;
            }

            } break;

        case MSG_CREATE:
            wwmFrameInit(winId, pFrameData);
            winCbAdd(
                winId,
                MSG_PTR_FIRST,
                MSG_PTR_LAST,
                wwmMoveSizeCallback,
                0
                );
            break;

        case MSG_DESTROY:
            winCbRemove(winId, wwmMoveSizeCallback);
            if (pFrameData->pTitleText != UGL_NULL) {
                UGL_FREE(pFrameData->pTitleText);
            }
            break;

        case MSG_MANAGE:
            if (winVisibleGet(winId) == UGL_TRUE ||
                ((winStateGet(winId) & WIN_STATE_MINIMIZED) != 0x00)) {

                if (winVisibleGet(winId) == UGL_TRUE &&
                    ((winStateGet(winId) & WIN_STATE_MINIMIZED) != 0x00)) {

                    winVisibleSet(winId, UGL_FALSE);
                }
            }
            return UGL_STATUS_FINISHED;

        case MSG_CLASS_INIT: {
            UGL_REG_DATA *      pRegData;
            UGL_DEVICE_ID       displayId;
            UGL_RGB             color;
            UGL_ORD             hue;
            UGL_ORD             lightness;
            UGL_ORD             saturation;
            UGL_FONT_DRIVER_ID  fntDrvId;
            UGL_FONT_DEF        fntDef;

            pRegData = uglRegistryFind(UGL_DISPLAY_TYPE, UGL_NULL, 0, 0);
            if (pRegData == UGL_NULL) {
                return UGL_STATUS_ERROR;
            }
            displayId = (UGL_DEVICE_ID) pRegData->data;

            /* Allocate colors for window frame */
            color = WWM_FRAME_COLOR_ACTIVE;
            uglColorAlloc(
                displayId,
                &color,
                UGL_NULL,
                &wwmFrameClassData.frameColorActive,
                1
                );

            uglRGB2HLS(color, &hue, &lightness, &saturation);
            uglHLS2RGB(hue, (255 + lightness) / 2, saturation, &color);
            uglColorAlloc(
                displayId,
                &color,
                UGL_NULL,
                &wwmFrameClassData.shineColorActive,
                1
                );

            uglHLS2RGB(hue, lightness / 2, saturation, &color);
            uglColorAlloc(
                displayId,
                &color,
                UGL_NULL,
                &wwmFrameClassData.shadowColorActive,
                1
                );

            color = WWM_FRAME_COLOR_INACTIVE;
            uglColorAlloc(
                displayId,
                &color,
                UGL_NULL,
                &wwmFrameClassData.frameColorInactive,
                1
                );

            uglRGB2HLS(color, &hue, &lightness, &saturation);
            uglHLS2RGB(hue, (255 + lightness) / 2, saturation, &color);
            uglColorAlloc(
                displayId,
                &color,
                UGL_NULL,
                &wwmFrameClassData.shineColorInactive,
                1
                );

            uglHLS2RGB(hue, lightness / 2, saturation, &color);
            uglColorAlloc(
                displayId,
                &color,
                UGL_NULL,
                &wwmFrameClassData.shadowColorInactive,
                1
                );

            /* Allocate frame text colors and set frame font */
            color = WWM_FRAME_TEXT_COLOR_ACTIVE;
            uglColorAlloc(
                displayId,
                &color,
                UGL_NULL,
                &wwmFrameClassData.textColorActive,
                1
                );

            color = WWM_FRAME_TEXT_COLOR_INACTIVE;
            uglColorAlloc(
                displayId,
                &color,
                UGL_NULL,
                &wwmFrameClassData.textColorInactive,
                1
                );

            pRegData = uglRegistryFind(UGL_FONT_ENGINE_TYPE, UGL_NULL, 0, 0);
            if (pRegData == UGL_NULL) {
                return UGL_STATUS_ERROR;
            }

            fntDrvId  = (UGL_FONT_DRIVER_ID) pRegData->data;
            uglFontFindString(fntDrvId, WWM_FRAME_FONT, &fntDef);
            wwmFrameClassData.fontId = uglFontCreate(fntDrvId, &fntDef);
            } break;

        case MSG_CLASS_DEINIT: {
            UGL_REG_DATA *  pRegData;
            UGL_DEVICE_ID   displayId;

            pRegData = uglRegistryFind(UGL_DISPLAY_TYPE, UGL_NULL, 0, 0);
            if (pRegData == UGL_NULL) {
                return UGL_STATUS_ERROR;
            }
            displayId = (UGL_DEVICE_ID) pRegData->data;

            uglColorFree(displayId, &wwmFrameClassData.textColorInactive, 1);
            uglColorFree(displayId, &wwmFrameClassData.textColorActive, 1);
            uglColorFree(displayId, &wwmFrameClassData.shadowColorInactive, 1);
            uglColorFree(displayId, &wwmFrameClassData.shineColorInactive, 1);
            uglColorFree(displayId, &wwmFrameClassData.frameColorInactive, 1);
            uglColorFree(displayId, &wwmFrameClassData.shadowColorActive, 1);
            uglColorFree(displayId, &wwmFrameClassData.shineColorActive, 1);
            uglColorFree(displayId, &wwmFrameClassData.frameColorActive, 1);

            if (wwmFrameClassId != UGL_NULL) {
                if (winClassDestroy(wwmFrameClassId) == UGL_STATUS_OK) {
                    wwmFrameClassId = UGL_NULL;
                }
            }

            return UGL_STATUS_FINISHED;
            } break;

        case MSG_DRAW:
            wwmFrameDraw(
                winId,
                pMsg->data.draw.gcId,
                &pMsg->data.draw.rect,
                pFrameData
                );
            return UGL_STATUS_FINISHED;

        case MSG_RECT_CHANGING:
            if ((winStateGet(winId) & WIN_STATE_MAXIMIZED) != 0x00) {
                memcpy(
                    &pMsg->data.rectChanging.newRect,
                    &pMsg->data.rectChanging.maxRect,
                    sizeof(UGL_RECT)
                    );
                pMsg->data.rectChanging.newRect.left-= WWM_FRAME_BORDER_SIZE;
                pMsg->data.rectChanging.newRect.top -= WWM_FRAME_BORDER_SIZE;
                pMsg->data.rectChanging.newRect.right += WWM_FRAME_BORDER_SIZE;
                pMsg->data.rectChanging.newRect.bottom += WWM_FRAME_BORDER_SIZE;
            }
            else {

                /* Ensure minmum size of frame window */
                if (UGL_RECT_WIDTH(pMsg->data.rectChanging.newRect) <
                    WWM_FRAME_MIN_WIDTH) {

                    if ((pMsg->data.rectChanging.newRect.left ==
                         pMsg->data.rectChanging.oldRect.left) ||
                        (pMsg->data.rectChanging.newRect.right !=
                         pMsg->data.rectChanging.oldRect.right)) {

                        pMsg->data.rectChanging.newRect.right =
                            pMsg->data.rectChanging.newRect.left +
                            WWM_FRAME_MIN_WIDTH - 1;
                    }
                    else {
                        pMsg->data.rectChanging.newRect.left =
                            pMsg->data.rectChanging.newRect.right -
                            WWM_FRAME_MIN_WIDTH + 1;
                    }
                }

                if (UGL_RECT_HEIGHT(pMsg->data.rectChanging.newRect) <
                    WWM_FRAME_MIN_HEIGHT) {

                    if ((pMsg->data.rectChanging.newRect.top ==
                         pMsg->data.rectChanging.oldRect.top) ||
                        (pMsg->data.rectChanging.newRect.bottom !=
                         pMsg->data.rectChanging.oldRect.bottom)) {

                        pMsg->data.rectChanging.newRect.bottom =
                            pMsg->data.rectChanging.newRect.top +
                            WWM_FRAME_MIN_HEIGHT - 1;
                    }
                    else {
                        pMsg->data.rectChanging.newRect.top =
                            pMsg->data.rectChanging.newRect.bottom -
                            WWM_FRAME_MIN_HEIGHT + 1;
                    }
                }
            }
            break;

        case MSG_RECT_CHILD_CHANGING:
            memcpy(
                &pMsg->data.rectChildChanging.newRect,
                &pFrameData->contentRect,
                sizeof(UGL_RECT)
                );
            break;

        case MSG_RECT_CHANGED: {
            UGL_ORD  newWidth = UGL_RECT_WIDTH(pMsg->data.rectChanged.newRect);
            UGL_ORD  oldWidth = UGL_RECT_WIDTH(pMsg->data.rectChanged.oldRect);

            if (newWidth != oldWidth) {
                if (newWidth > oldWidth) {
                    UGL_RECT  rect;
                    memcpy(&rect, &pFrameData->captionRect, sizeof(UGL_RECT));
                    rect.left = rect.right;
                    winRectInvalidate(winId, &rect);
                }

                pMsg->data.rectChanged.validRect.right -= WWM_FRAME_BORDER_SIZE;
                pFrameData->contentRect.right += newWidth - oldWidth;
                pFrameData->captionRect.right += newWidth - oldWidth;

                if (newWidth < oldWidth) {
                    UGL_RECT  rect;
                    memcpy(&rect, &pFrameData->captionRect, sizeof(UGL_RECT));
                    rect.left = rect.right;
                    winRectInvalidate(winId, &rect);
                }
            }

            if (UGL_RECT_HEIGHT(pMsg->data.rectChanged.newRect) !=
                UGL_RECT_HEIGHT(pMsg->data.rectChanged.oldRect)) {

                pMsg->data.rectChanged.validRect.bottom -=
                    WWM_FRAME_BORDER_SIZE;
            }

            winDrawRectGet(winId, &pFrameData->contentRect);
            pFrameData->contentRect.left += WWM_FRAME_BORDER_SIZE;
            pFrameData->contentRect.top += pFrameData->captionRect.bottom + 1;
            pFrameData->contentRect.right -= WWM_FRAME_BORDER_SIZE;
            pFrameData->contentRect.bottom -= WWM_FRAME_BORDER_SIZE;
            pFrameData->captionRect.right = pFrameData->contentRect.right;
            } break;

        case MSG_FRAME_CONTENT_RECT_SET: {
            UGL_RECT  rect;

            memcpy(&rect, &pMsg->data.frameContentRect, sizeof(UGL_RECT));

            rect.left   -= WWM_FRAME_BORDER_SIZE;
            rect.top    -= WWM_FRAME_BORDER_SIZE +
                UGL_RECT_HEIGHT(pFrameData->captionRect);
            rect.right  += WWM_FRAME_BORDER_SIZE;
            rect.bottom += WWM_FRAME_BORDER_SIZE;

            winRectSet(winId, &rect);
            } break;

        case MSG_FRAME_CAPTION_GET:
            pMsg->data.frameCaptionData.caption.pText =
                (UGL_CHAR *) pFrameData->pTitleText;
            pMsg->data.frameCaptionData.isWide = UGL_FALSE;
            break;

        case MSG_FRAME_CAPTION_SET:
            if (pFrameData->pTitleText != UGL_NULL) {
                UGL_FREE(pFrameData->pTitleText);
                pFrameData->pTitleText = UGL_NULL;
            }

            if (pMsg->data.frameCaptionData.caption.pText != UGL_NULL) {
                pFrameData->pTitleText = UGL_MALLOC(
                    strlen(pMsg->data.frameCaption) + 1);

                if (pFrameData->pTitleText == UGL_NULL) {
                    return UGL_STATUS_ERROR;
                }

                strcpy(pFrameData->pTitleText, pMsg->data.frameCaption);
            }

            winRectInvalidate(winId, UGL_NULL);

            break;

        default:
            /* TODO: Catch all other message types */
            break;
    }

    return winMsgHandle(winId, classId, pMsg);
}

