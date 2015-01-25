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

/* winMsg.h - Universal graphics library window messages */

#ifndef _winMsg_h
#define _winMsg_h

#include "uglMsg.h"
#include "winTypes.h"

/* Defines */

#define WIN_HIT_TEST_LEFT      0x00000001
#define WIN_HIT_TEST_TOP       0x00000002
#define WIN_HIT_TEST_RIGHT     0x00000004
#define WIN_HIT_TEST_BOTTOM    0x00000008
#define WIN_HIT_TEST_MOVE      0x0000000f
#define WIN_HIT_TEST_MISS      0x00000080

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct win_draw_data {
    UGL_GC_ID        gcId;
    UGL_RECT         rect;
    UGL_DEVICE_ID    displayId;
} WIN_DRAW_DATA;

typedef struct win_rect_data {
    UGL_RECT         oldRect;
    UGL_RECT         newRect;
} WIN_RECT_DATA;

typedef struct win_rect_changed_data {
    UGL_RECT         oldRect;
    UGL_RECT         newRect;
    UGL_RECT         validRect;
} WIN_RECT_CHANGED_DATA;

typedef struct win_rect_changing_data {
    UGL_RECT         oldRect;
    UGL_RECT         newRect;
    UGL_RECT         maxRect;
} WIN_RECT_CHANGING_DATA;

typedef struct win_rect_child_changing_data {
    UGL_RECT         oldRect;
    UGL_RECT         newRect;
    WIN_ID           childId;
} WIN_RECT_CHILD_CHANGING_DATA;

typedef struct win_zpos_data {
    WIN_ID           changeId;
    UGL_ORD          oldPos;
    UGL_POS          newPos;
} WIN_ZPOS_DATA;

typedef struct win_hit_test_data {
   UGL_POINT         position;
   UGL_UINT32        hitResult;
} WIN_HIT_TEST_DATA;

typedef struct win_frame_image_data {
    UGL_SIZE         width;
    UGL_SIZE         height;
    UGL_BITMAP_ID    bitmapId;
} WIN_FRAME_IMAGE_DATA;

typedef struct win_max_min_data {
    UGL_UINT32       oldState;
    UGL_UINT32       newState;
} WIN_MAXIMIZED_DATA, WIN_MINIMIZED_DATA, WIN_RESTORED_DATA;

typedef struct win_frame_caption_data {
    union {
        UGL_CHAR *   pText;
        UGL_WCHAR *  pTextW;
    } caption;
    UGL_BOOL         isWide;
} WIN_FRAME_CAPTION_DATA;

typedef union win_msg_data {
    UGL_MSG_DATA                    uglData;
    UGL_POINTER_DATA                ptr;
    UGL_KEYBOARD_DATA               kbd;
    WIN_DRAW_DATA                   draw;
    WIN_HIT_TEST_DATA               hitTest;
    WIN_RECT_DATA                   rect;
    WIN_RECT_CHANGED_DATA           rectChanged;
    WIN_RECT_CHANGING_DATA          rectChanging;
    WIN_RECT_CHILD_CHANGING_DATA    rectChildChanging;
    WIN_ZPOS_DATA                   zPos;
    WIN_ID                          attachId;
    WIN_ID                          detachId;
    WIN_ID                          activateId;
    WIN_MAXIMIZED_DATA              maximized;
    WIN_MINIMIZED_DATA              minimized;
    WIN_RESTORED_DATA               restored;
    UGL_CHAR *                      frameCaption;
    WIN_FRAME_CAPTION_DATA          frameCaptionData;
    UGL_RECT                        frameContentRect;
    UGL_BITMAP_ID                   frameImageId;
    UGL_BOOL                        frameResizable;
    UGL_UINT32                      appExitTimeout;
} WIN_MSG_DATA;

typedef struct win_msg {
    UGL_MSG_TYPE              type;
    WIN_ID                    winId;
    WIN_MSG_DATA              data;
} WIN_MSG;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _winMsg_h */

