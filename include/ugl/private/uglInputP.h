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

/* uglInputP.h - Private input header */

#include "uglinput.h"

#ifndef _uglInputP_h
#define _uglInputP_h

/* Defines */

#define UGL_INPUT_Q_SIZE           64

#define UGL_SERVICE_RUN             0
#define UGL_SERVICE_IDLE            1
#define UGL_SERVICE_DESTROYED       2
#define UGL_SERVICE_IDLE_REQ        3
#define UGL_SERVICE_RESUME_REQ      4
#define UGL_SERVICE_DESTROY_REQ     5

#define UGL_SERVICE_DELAY_CYCLE   100
#define UGL_SELECT_TIMEOUT       1000

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct ugl_input_service {
    UGL_LOCK_ID            lockId;
    UGL_TASK_ID            taskId;
    UGL_RECT               boundingRect;
    UGL_POINT              position;
    UGL_UINT32             modifiers;
    volatile UGL_UINT32    taskState;
    UGL_BOOL               autoLedControl;
    UGL_KBD_MAP           *pKbdMap;
    struct ugl_cb_list    *pCbList;
    struct ugl_msg_queue  *pDefaultQ;
    struct ugl_msg_queue  *pInputQ;
    struct ugl_input_dev  *ppDevice[UGL_MAX_INPUT_DEVICES];
} UGL_INPUT_SERVICE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglInputP_h */

