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

/* uglListP.h - Private linked list for windows */

#ifndef _uglListP_h
#define _uglListP_h

#include "ugl.h"

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ugl_node {
    struct ugl_node *  pNext;
    struct ugl_node *  pPrev;
} UGL_NODE;

typedef struct ugl_list {
    UGL_NODE *  pFirst;
    UGL_NODE *  pLast;
    UGL_SIZE    count;
} UGL_LIST;

/******************************************************************************
 *
 * uglListCreate - Create window list
 *
 * RETURNS: Pointer to list or UGL_NULL
 */

UGL_LIST * uglListCreate (
    void
    );

/******************************************************************************
 *
 * uglListInit - Initialize linked list datastruct
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

STATUS uglListInit (
    UGL_LIST *  pList
    );

/******************************************************************************
 *
 * uglListInsert - Insert element in list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglListInsert (
    UGL_LIST *  pList,
    UGL_NODE *  pNode,
    UGL_NODE *  pNextNode
    );

/******************************************************************************
 *
 * uglListAdd - Add an element to the list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglListAdd (
    UGL_LIST *  pList,
    UGL_NODE *  pNode
    );

/******************************************************************************
 *
 * uglListRemove - Remove an element from the list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglListRemove (
    UGL_LIST *  pList,
    UGL_NODE *  pNode
    );

/******************************************************************************
 *
 * uglListCount - Get number of elements in list
 *
 * RETURNS: Number of list elements
 */

UGL_SIZE uglListCount (
    UGL_LIST *  pList
    );

/******************************************************************************
 *
 * uglListFirst - Get first node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

UGL_NODE * uglListFirst (
    UGL_LIST *  pList
    );

/******************************************************************************
 *
 * uglListLast - Get last node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

UGL_NODE * uglListLast (
    UGL_LIST *  pList
    );

/******************************************************************************
 *
 * uglListNth - Get n-th node on list
 *
 * RETURNS: Pointer to list node or UGL_NULL
 */

UGL_NODE * uglListNth (
    UGL_LIST *  pList,
    UGL_ORD     n
    );

/******************************************************************************
 *
 * uglListNext - Get next node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

UGL_NODE * uglListNext (
    UGL_NODE *  pNode
    );

/******************************************************************************
 *
 * uglListPrev - Get previous node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

UGL_NODE * uglListPrev (
    UGL_NODE *  pNode
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglListP_h */

