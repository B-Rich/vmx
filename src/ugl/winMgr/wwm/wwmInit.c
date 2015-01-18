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

/* wwmInit.c - Generic window manager */

#include "ugl.h"
#include "uglWin.h"

/* Locals */

UGL_LOCAL void *  wwmCreate (
    WIN_MGR_ID            winMgrId,
    UGL_DEVICE_ID         displayId,
    UGL_INPUT_SERVICE_ID  inputServiceId 
    );

UGL_LOCAL UGL_STATUS wwmDestroy (
    WIN_MGR_ID  winMgrId,
    void *      pData
    );

UGL_LOCAL WIN_MGR_ENGINE  wwmEngine = {
    wwmCreate,
    wwmDestroy,
    UGL_NULL
};

/* Globals */

WIN_MGR_ENGINE *  wwmEngineId = &wwmEngine;

/******************************************************************************
 *
 * wwmCreate - Create instance of window manager
 *
 * RETURNS: Pointer to display or UGL_NULL
 */

UGL_LOCAL void *  wwmCreate (
    WIN_MGR_ID            winMgrId,
    UGL_DEVICE_ID         displayId,
    UGL_INPUT_SERVICE_ID  inputServiceId 
    ) {

    /* TODO */

    return displayId;
}

/******************************************************************************
 *
 * wwmDestroy - Destroy instance of window manager
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS wwmDestroy (
    WIN_MGR_ID  winMgrId,
    void *      pData
    ) {

    /* TODO */

    return UGL_STATUS_ERROR;
}

