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

/* uglMsgP.h - Private header for message queue */

#ifndef _uglMsgP_h
#define _uglMsgP_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct ugl_msg_queue {
    UGL_OS_MSG_Q_ID     osQId;
    UGL_MSG             peekMsg;
    UGL_UINT32          useCount;
} UGL_MSG_QUEUE;

typedef struct ugl_cb_list {
    UGL_SIZE             arraySize;
    UGL_SIZE             numCallbacks;
    struct ugl_cb_item  *pCbArray;
} UGL_CB_LIST;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglMsgP_h */

