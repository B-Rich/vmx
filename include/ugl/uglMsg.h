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

/* uglMsg.h - Universal graphics library messages header */

#ifndef _uglMsg_h
#define _uglMsg_h

#include "uglMsgTypes.h"

/* Pointer buttons */
#define UGL_PTR_BUTTON1        0x00000001
#define UGL_PTR_BUTTON2        0x00000002
#define UGL_PTR_BUTTON3        0x00000004
#define UGL_PTR_BUTTON4        0x00000008
#define UGL_PTR_BUTTON5        0x00000010
#define UGL_PTR_BUTTON_MASK    0x000000ff
#define UGL_PTR_MOD_MASK       0x000000ff

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

struct ugl_input_dev;

typedef struct ugl_raw_ptr_data {
    struct ugl_input_dev  *deviceId;
    UGL_UINT16             buttonState;
    UGL_BOOL               isAbsolute;

    union {
        UGL_POINT          absolute;
        UGL_VECTOR         relative;
    } pos;
} UGL_RAW_PTR_DATA;

typedef struct ugl_ptr_data {
    UGL_RAW_PTR_DATA    rawPtr;
    UGL_TIMESTAMP       timeStamp;
    UGL_POINT           position;
    UGL_VECTOR          delta;
    UGL_UINT32          buttonState;
    UGL_UINT32          buttonChange;
} UGL_POINTER_DATA;

/* Message data */
typedef UGL_UINT32 UGL_MSG_TYPE;

typedef union ugl_msg_data {
    UGL_POINTER_DATA    pointer;
    UGL_RAW_PTR_DATA    rawPtr;
} UGL_MSG_DATA;

/* Messages */
typedef struct ugl_msg {
    UGL_MSG_TYPE    type;
    UGL_ID          objectId;
    UGL_MSG_DATA    data;
} UGL_MSG;

struct ugl_msg_queue;
typedef struct ugl_msg_queue *UGL_MSG_Q_ID;

/* Callbacks */

typedef UGL_STATUS  (UGL_CB) (UGL_ID objectId, struct ugl_msg *pMsg,
                              void *pObjectData, void *pParam);

typedef struct ugl_cb_item {
    UGL_UINT32    filterMin;
    UGL_UINT32    filterMax;
    UGL_CB       *pCallback;
    void         *pParam;
} UGL_CB_ITEM;

struct ugl_cb_list;
typedef struct ugl_cb_list *UGL_CB_LIST_ID;

/* Functions */
/******************************************************************************
 *
 * uglMsgQCreate - Create message queue
 *
 * RETURNS: UGL_MSG_Q_ID or UGL_NULL
 */

UGL_MSG_Q_ID uglMsgQCreate (
    UGL_SIZE  queueSize
    );

/******************************************************************************
 *
 * uglMsgQDestroy - Destroy message queue
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglMsgQDestroy (
    UGL_MSG_Q_ID  qId
    );

/******************************************************************************
 *
 * uglMsgQPost - Post message to message queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS uglMsgQPost (
    UGL_MSG_Q_ID  qId,
    UGL_MSG      *pMsg,
    UGL_TIMEOUT   timeout
    );

/******************************************************************************
 *
 * uglMsgQGet - Get message from message queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS uglMsgQGet (
    UGL_MSG_Q_ID  qId,
    UGL_MSG      *pMsg,
    UGL_TIMEOUT   timeout
    );

/******************************************************************************
 *
 * uglCbListCreate - Create callback list
 *
 * RETURNS: UGL_CB_LIST_ID or UGL_NULL
 */

UGL_CB_LIST_ID uglCbListCreate (
    const  UGL_CB_ITEM  *pCbArray
    );

/******************************************************************************
 *
 * uglCbListDestroy - Destroy callback list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */
 
UGL_STATUS uglCbListDestroy (
    UGL_CB_LIST_ID  cbListId
    );

/******************************************************************************
 *
 * uglCbAdd - Add callback to callback list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbAdd (
    UGL_CB_LIST_ID  cbListId,
    UGL_UINT32      filterMin,
    UGL_UINT32      filterMax,
    UGL_CB         *pCallback,
    void           *pParam
    );

/******************************************************************************
 *
 * uglCbAddArray - Add array of callbacks to callback list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbAddArray (
    UGL_CB_LIST_ID    cbListId,
    const UGL_CB_ITEM *pCbArray
    );

/******************************************************************************
 *
 * uglCbRemove - Remove callback from list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbRemove (
    UGL_CB_LIST_ID  cbListId,
    UGL_CB         *pCallback
    );

/******************************************************************************
 *
 * uglCbRemoveArray - Remove array of callbacks from list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbRemoveArray (
    UGL_CB_LIST_ID     cbListId,
    const UGL_CB_ITEM *pCbArray
    );

/******************************************************************************
 *
 * uglCbListExecute - Execute callback list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbListExecute (
    UGL_CB_LIST_ID  cbListId,
    UGL_ID          objectId,
    UGL_MSG        *pMsg,
    void           *pParam
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglMsg_h */

