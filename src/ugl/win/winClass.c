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

/* winClass.c - Window class for universal graphics library */

#include <stdlib.h>
#include <string.h>
#include "uglWin.h"
#include "private/uglWinP.h"
#include "private/uglListP.h"
#include "uglmem.h"

/* Globals */

UGL_LIST_ID  classListId = UGL_NULL;

/* Locals */

UGL_LOCAL UGL_STATUS  winClassRegister(
    const UGL_CHAR *  pClassName,
    WIN_CLASS *       pClass
    );

UGL_LOCAL UGL_STATUS  winClassUnregister (
    WIN_CLASS *  pClass
    );

/******************************************************************************
 *
 * winClassCreate - Create new window class
 *
 * RETURNS: Window class id or UGL_NULL
 */

WIN_CLASS_ID  winClassCreate (
    const UGL_CHAR *   pName,
    const UGL_CHAR *   pParentName,
    WIN_MSG_HANDLER *  pMsgHandler,
    UGL_SIZE           dataSize,
    void *             pDefaultData
    ) {
    WIN_CLASS *  pClass;
    WIN_CLASS *  pParent;
    UGL_STATUS   status = UGL_STATUS_OK;

    if (pParentName == UGL_NULL) {

       if (pName == UGL_NULL) {

           /* Create root class */
           pName = (UGL_CHAR *) winRootClassName;
           pParent = UGL_NULL;
       }
       else {

           /* Parent is root */
           pParentName = winRootClassName;
           pParent = winClassLookup(pParentName);
       }
    }
    else if (pName == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
       pParent = winClassLookup(pParentName);
       if (pParent == UGL_NULL) {
           status = UGL_STATUS_ERROR;
       }
    }

    if (status == UGL_STATUS_OK) {

        /* Create new class */
        pClass = (WIN_CLASS *) UGL_CALLOC(1,
            sizeof(WIN_CLASS) + dataSize + strlen(pName) + 1);
        if (pClass != UGL_NULL) {

            /* Setup class struct */
            pClass->pParent     = pParent;
            pClass->pMsgHandler = pMsgHandler;
            pClass->dataSize    = dataSize;

            if (pParent != UGL_NULL) {
                pClass->dataOffset = pParent->dataOffset + pParent->dataSize;
                pClass->windowSize = pParent->windowSize + dataSize;
            }
            else {
                pClass->dataOffset = 0;
                pClass->windowSize = sizeof(UGL_WINDOW) + dataSize;
            }

            pClass->pDefaultData = &pClass[1];
            pClass->pName        =
                (UGL_CHAR *) pClass->pDefaultData + pClass->dataSize;

            if (pDefaultData != UGL_NULL) {
                memcpy(pClass->pDefaultData, pDefaultData, dataSize);
            }

            strcpy(pClass->pName, pName);

            /* Register class */
            if (winClassRegister(pName, pClass) != UGL_STATUS_OK) {
                UGL_FREE(pClass);
                pClass = UGL_NULL;
            }
        }
    }
    else {
        pClass = UGL_NULL;
    }

    return pClass;
}

/******************************************************************************
 *
 * winClassDestroy - Destroy window class
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winClassDestroy (
    WIN_CLASS_ID  classId
    ) {
    UGL_STATUS  status;

    if (classId == UGL_NULL || classId->useCount > 0) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = winClassUnregister(classId);
        UGL_FREE(classId);
    }

    return status;
}

/******************************************************************************
 *
 * winClassDataSet - Set data for class
 *
 * RETURNS: Pointer to default data or UGL_NULL
 */

void *  winClassDataSet (
    WIN_CLASS_ID  classId,
    void *        pData,
    UGL_SIZE      dataSize
    ) {

    if (pData != UGL_NULL) {
        memcpy(classId->pDefaultData, pData, min(dataSize, classId->dataSize));
    }

    return classId->pDefaultData;
}

/******************************************************************************
 *
 * winClassDataGet - Get data for class
 *
 * RETURNS: Pointer to default data or UGL_NULL
 */

void *  winClassDataGet (
    WIN_CLASS_ID  classId,
    void *        pData,
    UGL_SIZE      dataSize
    ) {

    if (pData != UGL_NULL) {
        memcpy(pData, classId->pDefaultData, min(dataSize, classId->dataSize));
    }

    return classId->pDefaultData;
}

/******************************************************************************
 *
 * winClassLookup - Lookup window class
 *
 * RETURNS: Window class id or UGL_NULL
 */

WIN_CLASS_ID  winClassLookup (
    const UGL_CHAR *  pClassName
    ) {
    WIN_CLASS *  pClass;

    if (classListId == UGL_NULL) {
        pClass = UGL_NULL;
    }
    else {
        if (pClassName == UGL_NULL) {

            /* Request root class */
            pClassName = winRootClassName;
        }

        for (pClass = (WIN_CLASS *) uglListFirst(classListId);
             pClass != UGL_NULL;
             pClass = (WIN_CLASS *) uglListNext(&pClass->node)) {

            /* Find class with name */
            if (strcmp(pClass->pName, pClassName) == 0) {
                break;
            }
        }
    }

    return pClass;
}

/******************************************************************************
 *
 * winClassRegister - Register window class
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winClassRegister(
    const UGL_CHAR *  pClassName,
    WIN_CLASS *       pClass
    ) {
    UGL_STATUS  status;

    if (pClassName == UGL_NULL || pClass == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (classListId == UGL_NULL) {
            classListId = uglListCreate();
        }

        if (classListId == UGL_NULL) {
            status = UGL_STATUS_ERROR;
        }
        else {
            status = uglListAdd(classListId, &pClass->node);
        }
    }

    return status;
}

/******************************************************************************
 *
 * winClassUnregister - Unregister window class
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS  winClassUnregister (
    WIN_CLASS *  pClass
    ) {
    UGL_STATUS  status;

    if (pClass == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = uglListRemove(classListId, &pClass->node);
        if (uglListCount(classListId) == 0) {

            uglListDestroy(classListId);
            classListId = UGL_NULL;
        }
    }

    return status;
}

