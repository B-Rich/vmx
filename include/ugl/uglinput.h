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

/* uglinput.h - Input device support for universal graphics library */

#ifndef _uglinput_h
#define _uglinput_h

#include "uglMsg.h"

/* Basic settings */
#define UGL_MAX_INPUT_DEVICES            10
#define UGL_INPUT_TASK_PRIORITY          60
#define UGL_INPUT_TASK_STACK_SIZE    0x1000
#define UGL_IO_SELECT_WAIT                1

/* Device types */
#define UGL_DEVICE_KEYBOARD               1
#define UGL_DEVICE_POINTER_ABSOLUTE       2
#define UGL_DEVICE_POINTER_RELATIVE       3

/* Input device requests */
#define ICR_SET_PTR_POS                   1
#define ICR_SET_PTR_CONSTRAINT            2
#define ICR_SET_AUTO_LED_CONTROL          3
#define ICR_SET_LED                       4
#define ICR_CLEAR_LED                     5
#define ICR_GET_LED_STATE                 6
#define ICR_SET_LED_STATE                 7
#define ICR_CALIBRATE                     8
#define ICR_GET_PTR_TYPE                  9
#define ICR_READ                         10

/* Pointer device types */
#define UGL_PTR_TYPE_MOUSE                1
#define UGL_PTR_TYPE_TOUCH_SCREEN         2
#define UGL_PTR_TYPE_PEN_TABLET           3

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef int UGL_DEVICE_TYPE;
typedef int UGL_DEVICE_REQ;

struct ugl_input_service;
typedef struct ugl_input_service *UGL_INPUT_SERVICE_ID;

typedef struct ugl_input_dev {
    int                       fd;
    UGL_DEVICE_TYPE           deviceType;
    struct ugl_input_service *inputServiceId;
    struct ugl_input_drv     *pDriver;
} UGL_INPUT_DEV;

typedef UGL_INPUT_DEV *UGL_INPUT_DEV_ID;

typedef struct ugl_input_drv {
    UGL_INPUT_DEV*   (*open)  (
        UGL_CHAR *name,
        struct ugl_input_drv *pDriver
        );

    UGL_STATUS    (*close)  (
        UGL_INPUT_DEV *pDevice
        );

    UGL_STATUS    (*control)  (
        UGL_INPUT_DEV *pDevice,
        UGL_DEVICE_REQ request,
        void *pArg
        );
} UGL_INPUT_DRV;

typedef struct {
    UGL_UINT32    filterMin;
    UGL_UINT32    filterMax;
    void         *pParam;
} UGL_INPUT_CB_ITEM;

/* Functions */

/******************************************************************************
 *
 * uglInputDevAdd - Add input device to input system
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputDevAdd (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_INPUT_DEV_ID      devId
    );

/******************************************************************************
 *
 * uglInputDevRemove - Remove input device from input system
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputDevRemove (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_INPUT_DEV_ID      devId
    );

/******************************************************************************
 *
 * uglInputDevOpen - Open input device
 *
 * RETURNS: UGL_INPUT_DEV_ID or UGL_NULL
 */

UGL_INPUT_DEV_ID uglInputDevOpen (
    UGL_CHAR       *name,
    UGL_INPUT_DRV  *pDriver
    );

/******************************************************************************
 *
 * uglInputDevClose - Close input device
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputDevClose (
    UGL_INPUT_DEV_ID  devId
    );

/******************************************************************************
 *
 * uglInputDevControl - Input device control
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputDevControl (
    UGL_INPUT_DEV_ID  devId,
    UGL_DEVICE_REQ    request,
    void             *pArg
    );

/******************************************************************************
 *
 * uglInputMsgPost - Post message to input queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS uglInputMsgPost (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_MSG              *pMsg
    );

/******************************************************************************
 *
 * uglInputMsgGet - Get message from input queue
 *
 * RETURNS: UGL_STATUS_OK or error code
 */

UGL_STATUS uglInputMsgGet (
    UGL_INPUT_SERVICE_ID  srvId,
    UGL_MSG              *pMsg,
    UGL_TIMEOUT           timeout
    );

/******************************************************************************
 *
 * uglInputServiceCreate - Create input service
 *
 * RETURNS: UGL_INPUT_SERVICE_ID or UGL_NULL
 */

UGL_INPUT_SERVICE_ID uglInputServiceCreate (
    UGL_RECT           *pInputRect,
    UGL_INPUT_CB_ITEM  *pInputCbArray
    );

/******************************************************************************
 *
 * uglInputServiceDestroy - Destroy input service
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglInputServiceDestroy (
    UGL_INPUT_SERVICE_ID  srvId
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglinput_h */

