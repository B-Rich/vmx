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

/* uglMsgTypes.h - Message types for universal graphics library */

#ifndef _uglMsgTypes_h
#define _uglMsgTypes_h

#include "ugl/ugltypes.h"

/* Raw input */
#define MSG_RAW_INPUT_FIRST           1
#define MSG_RAW_KBD                   (MSG_RAW_INPUT_FIRST + 0)
#define MSG_RAW_PTR                   (MSG_RAW_INPUT_FIRST + 1)
#define MSG_RAW_INPUT_LAST            (MSG_RAW_INPUT_FIRST + 99)

/* Mapped input */
#define MSG_INPUT_FIRST               100
#define MSG_KEYBOARD                  (MSG_INPUT_FIRST + 0)
#define MSG_POINTER                   (MSG_INPUT_FIRST + 1)
#define MSG_INPUT_LAST                (MSG_INPUT_FIRST + 99)

/* Pointer messages */
#define MSG_PTR_FIRST                 200

#define MSG_PTR_BTN_FIRST             (MSG_PTR_FIRST + 0)

#define MSG_PTR_BTN_DOWN_FIRST        (MSG_PTR_FIRST + 0)
#define MSG_PTR_BTN1_DOWN             (MSG_PTR_FIRST + 0)
#define MSG_PTR_BTN2_DOWN             (MSG_PTR_FIRST + 1)
#define MSG_PTR_BTN3_DOWN             (MSG_PTR_FIRST + 2)
#define MSG_PTR_BTN_DOWN_LAST         (MSG_PTR_FIRST + 9)

#define MSG_PTR_BTN_UP_FIRST          (MSG_PTR_FIRST + 10)
#define MSG_PTR_BTN1_UP               (MSG_PTR_FIRST + 10)
#define MSG_PTR_BTN2_UP               (MSG_PTR_FIRST + 11)
#define MSG_PTR_BTN3_UP               (MSG_PTR_FIRST + 12)
#define MSG_PTR_BTN_UP_LAST           (MSG_PTR_FIRST + 19)

#define MSG_PTR_BTN_DBL_FIRST         (MSG_PTR_FIRST + 20)
#define MSG_PTR_BTN1_DBL_CLICK        (MSG_PTR_FIRST + 20)
#define MSG_PTR_BTN2_DBL_CLICK        (MSG_PTR_FIRST + 21)
#define MSG_PTR_BTN3_DBL_CLICK        (MSG_PTR_FIRST + 22)
#define MSG_PTR_BTN_DBL_LAST          (MSG_PTR_FIRST + 29)

#define MSG_PTR_BTN_TRI_FIRST         (MSG_PTR_FIRST + 30)
#define MSG_PTR_BTN1_TRI_CLICK        (MSG_PTR_FIRST + 30)
#define MSG_PTR_BTN2_TRI_CLICK        (MSG_PTR_FIRST + 31)
#define MSG_PTR_BTN3_TRI_CLICK        (MSG_PTR_FIRST + 32)
#define MSG_PTR_BTN_TRI_LAST          (MSG_PTR_FIRST + 39)

#define MSG_PTR_BTN_LAST              (MSG_PTR_BTN_FIRST + 99)

#define MSG_PTR_MOVE                  (MSG_PTR_FIRST + 100)
#define MSG_PTR_DRAG                  (MSG_PTR_FIRST + 101)

#define MSG_PTR_ENTER                 (MSG_PTR_FIRST + 110)
#define MSG_PTR_LEAVE                 (MSG_PTR_FIRST + 111)

#define MSG_HIT_TEST                  (MSG_PTR_FIRST + 120)

#define MSG_PTR_LAST                  (MSG_PTR_FIRST + 199)

#define MSG_WIN_FIRST                 1000

#define MSG_CREATE                    (MSG_WIN_FIRST + 0)
#define MSG_DESTROY                   (MSG_WIN_FIRST + 1)
#define MSG_MANAGE                    (MSG_WIN_FIRST + 2)
#define MSG_UNMANAGE                  (MSG_WIN_FIRST + 3)
#define MSG_ATTACH                    (MSG_WIN_FIRST + 4)
#define MSG_DETACH                    (MSG_WIN_FIRST + 5)
#define MSG_ACTIVATE                  (MSG_WIN_FIRST + 6)
#define MSG_DEACTIVATE                (MSG_WIN_FIRST + 7)
#define MSG_CLOSE                     (MSG_WIN_FIRST + 8)
#define MSG_CLASS_INIT                (MSG_WIN_FIRST + 50)
#define MSG_CLASS_DEINIT              (MSG_WIN_FIRST + 51)
#define MSG_HIDE                      (MSG_WIN_FIRST + 52)
#define MSG_SHOW                      (MSG_WIN_FIRST + 53)

#define MSG_DRAW_FIRST                (MSG_WIN_FIRST + 100)
#define MSG_EXPOSE                    (MSG_WIN_FIRST + 100)
#define MSG_DRAW                      (MSG_WIN_FIRST + 101)
#define MSG_DRAW_LAST                 (MSG_WIN_FIRST + 199)

#define MSG_FRAME_FIRST               (MSG_WIN_FIRST + 400)
#define MSG_FRAME_CAPTION_GET         (MSG_WIN_FIRST + 400)
#define MSG_FRAME_CAPTION_SET         (MSG_WIN_FIRST + 401)
#define MSG_FRAME_CONTENT_RECT_SET    (MSG_WIN_FIRST + 402)
#define MSG_FRAME_IMAGE_GET           (MSG_WIN_FIRST + 403)
#define MSG_FRAME_IMAGE_SET           (MSG_WIN_FIRST + 404)
#define MSG_FRAME_MAXIMIZE            (MSG_WIN_FIRST + 405)
#define MSG_FRAME_MINIMIZE            (MSG_WIN_FIRST + 406)
#define MSG_FRAME_RESIZABLE_SET       (MSG_WIN_FIRST + 407)
#define MSG_FRAME_RESTORE             (MSG_WIN_FIRST + 408)
#define MSG_FRAM_LAST                 (MSG_WIN_FIRST + 499)

#define MSG_WIN_LAST                  (MSG_WIN_FIRST + 999)

#define MSG_APP_FIRST                 2000

#define MSG_APP_CREATE                (MSG_APP_FIRST + 0)
#define MSG_APP_DESTROY               (MSG_APP_FIRST + 1)
#define MSG_APP_EXIT                  (MSG_APP_FIRST + 2)
#define MSG_APP_NO_WINDOWS            (MSG_APP_FIRST + 3)
#define MSG_APP_ACTIVATE              (MSG_APP_FIRST + 4)
#define MSG_APP_DEACTIVATE            (MSG_APP_FIRST + 5)

#define MSG_APP_LST                   (MSG_APP_FIRST + 999)

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglMsgTypes_h */

