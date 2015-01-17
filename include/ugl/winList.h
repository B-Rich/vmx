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

/* winList.h - Linked list for windows */

#ifndef _winList_h
#define _winList_h

#include "ugl.h"

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct win_node {
    struct win_node *  pNext;
    struct win_node *  pPrev;
} WIN_NODE;

typedef struct win_list {
    WIN_NODE *  pFirst;
    WIN_NODE *  pLast;
    UGL_SIZE    count;
} WIN_LIST;

/******************************************************************************
 *
 * winListCreate - Create window list
 *
 * RETURNS: Pointer to list or UGL_NULL
 */

WIN_LIST * winListCreate (
    void
    );

/******************************************************************************
 *
 * winListInit - Initialize linked list datastruct
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

STATUS winListInit (
    WIN_LIST *  pList
    );

/******************************************************************************
 *
 * winListInsert - Insert element in list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winListInsert (
    WIN_LIST *  pList,
    WIN_NODE *  pNode,
    WIN_NODE *  pNextNode
    );

/******************************************************************************
 *
 * winListAdd - Add an element to the list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winListAdd (
    WIN_LIST *  pList,
    WIN_NODE *  pNode
    );

/******************************************************************************
 *
 * winListRemove - Remove an element from the list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winListRemove (
    WIN_LIST *  pList,
    WIN_NODE *  pNode
    );

/******************************************************************************
 *
 * winListCount - Get number of elements in list
 *
 * RETURNS: Number of list elements
 */

UGL_SIZE winListCount (
    WIN_LIST *  pList
    );

/******************************************************************************
 *
 * winListFirst - Get first node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

WIN_NODE * winListFirst (
    WIN_LIST *  pList
    );

/******************************************************************************
 *
 * winListLast - Get last node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

WIN_NODE * winListLast (
    WIN_LIST *  pList
    );

/******************************************************************************
 *
 * winListNth - Get n-th node on list
 *
 * RETURNS: Pointer to list node or UGL_NULL
 */

WIN_NODE * winListNth (
    WIN_LIST *  pList,
    UGL_ORD     n
    );

/******************************************************************************
 *
 * winListNext - Get next node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

WIN_NODE * winListNext (
    WIN_NODE *  pNode
    );

/******************************************************************************
 *
 * winListPrev - Get previous node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

WIN_NODE * winListPrev (
    WIN_NODE *  pNode
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _winList_h */

