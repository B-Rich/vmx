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

/* winFrame.c - Universal grahpics library window frame */

#include "ugl.h"
#include "uglWin.h"
#include "private/uglWinP.h"

/******************************************************************************
 *
 * winFrameCaptionSet - Set window frame caption
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winFrameCaptionSet (
    WIN_ID      winId,
    UGL_CHAR *  pCaption
    ) {
    UGL_STATUS    status;
    WIN_MSG       msg;
    UGL_WINDOW *  pWindow;

    if (winId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if ((winId->attributes & WIN_ATTRIB_FRAMED) == 0x00) {
            pWindow = winId->pParent;
        }
        else {
            pWindow = winId;
        }

        msg.type                                 = MSG_FRAME_CAPTION_SET;
        msg.data.frameCaptionData.caption.pText  = pCaption;
        msg.data.frameCaptionData.isWide         = UGL_FALSE;

        status = winMsgSend(pWindow, &msg);
    }

    return status;
}

