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

/* winClass.h - Universal graphics library window class header */

#ifndef _winClass_h
#define _winClass_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef UGL_STATUS  (WIN_MSG_HANDLER) (
    WIN_ID        winId,
    WIN_CLASS_ID  classId,
    WIN_MSG *     pMsg,
    void *        pClassData
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
    );

/******************************************************************************
 *
 * winClassDestroy - Destroy window class
 * 
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */
 
UGL_STATUS  winClassDestroy (
    WIN_CLASS_ID  classId
    );

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
    );

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
    );

/******************************************************************************
 *
 * winClassLookup - Lookup window class
 *
 * RETURNS: Window class id or UGL_NULL
 */

WIN_CLASS_ID  winClassLookup (
    const UGL_CHAR *  pClassName
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _winClass_h */

