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

/* Locals */

UGL_LOCAL WIN_CLASS_ID  wwmFrameClassId = UGL_NULL;

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

    /* TODO */

    return winMsgHandle(winId, classId, pMsg);
}

