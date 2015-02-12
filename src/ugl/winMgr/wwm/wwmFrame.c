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

#include "ugl.h"
#include "uglWin.h"
#include "winMgr/wwm/wwmConfig.h"

/* Types */

typedef struct wwm_frame_data {
    void *         pTitleText;
    UGL_BOOL       titleIsWide;
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

/* Locals */

UGL_LOCAL WWM_FRAME_CLASS_DATA  wwmFrameClassData;
UGL_LOCAL WIN_CLASS_ID          wwmFrameClassId = UGL_NULL;

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

        case MSG_CLASS_INIT: {
            UGL_REG_DATA *  pRegData;
            UGL_DEVICE_ID   displayId;
            UGL_RGB         color;
            UGL_ORD         hue;
            UGL_ORD         lightness;
            UGL_ORD         saturation;
            UGL_FONT_ID *   pFontTable;

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

            pFontTable = winMgrFontTableGet(winMgrGet(winId), UGL_NULL);
            wwmFrameClassData.fontId = pFontTable[WIN_FONT_INDEX_SYSTEM];

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

            return (UGL_STATUS_FINISHED);
            } break;

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
            rect.top    -= WWM_FRAME_BORDER_SIZE;
                /* TODO: + UGL_RECT_HEIGHT(pFrameData->captionRect); */
            rect.right  += WWM_FRAME_BORDER_SIZE;
            rect.bottom += WWM_FRAME_BORDER_SIZE;

            winRectSet(winId, &rect);
            } break;

        default:
            /* TODO: Catch all other message types */
            break;
    }

    return winMsgHandle(winId, classId, pMsg);
}

