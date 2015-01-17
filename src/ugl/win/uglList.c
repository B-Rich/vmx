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

/* uglList.c - List for windows */

#include <string.h>
#include "uglmem.h"
#include "private/uglListP.h"

/******************************************************************************
 *
 * uglListCreate - Create window list
 *
 * RETURNS: Pointer to list or UGL_NULL
 */

UGL_LIST * uglListCreate (
    void
    ) {
    UGL_LIST *  pList;

    pList = UGL_CALLOC(1, sizeof(UGL_LIST));

    return pList;
}

/******************************************************************************
 *
 * uglListInit - Initialize linked list datastruct
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

STATUS uglListInit (
    UGL_LIST *  pList
    ) {
    UGL_STATUS  status;

    if (pList == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        memset(&pList, 0, sizeof(UGL_LIST));
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglListDestroy - Destroy window list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglListDestroy (
    UGL_LIST *  pList
    ) {
    UGL_STATUS  status;
    UGL_NODE *  pNode;
    UGL_NODE *  pNextNode;

    if (pList == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pNode = pList->pFirst;

        /* Free all nodes */
        while (pNode != UGL_NULL) {

            pNextNode = pNode->pNext;
            UGL_FREE(pNode);
            pNode = pNextNode;
        }

        status = UGL_STATUS_OK;
    }

    UGL_FREE(pList);

    return status;
}

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
    ) {
    UGL_STATUS  status;
  
    if (pList == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (pList->count == 0 || pNextNode == UGL_NULL) {
            status = uglListAdd(pList, pNode);
        }
        else if (pNextNode == pList->pFirst) {
            pNode->pNext = pList->pFirst;
            pNode->pPrev = UGL_NULL;

            pList->pFirst->pPrev = pNode;
            pList->pFirst        = pNode;
        }
        else {
            pNode->pNext = pNextNode;
            pNode->pPrev = pNextNode->pPrev;

            pNextNode->pPrev = pNode;

            pNode->pPrev->pNext = pNode;
        }

        pList->count++;
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglListAdd - Add an element to the list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglListAdd (
    UGL_LIST *  pList,
    UGL_NODE *  pNode
    ) {
    UGL_STATUS  status;

    if (pList == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (pList->count == 0) {
            pList->pFirst = pNode;
            pList->pLast  = pNode;

            pNode->pNext = UGL_NULL;
            pNode->pPrev = UGL_NULL;
        }
        else {
            pNode->pPrev = pList->pLast;
            pNode->pNext = UGL_NULL;

            pList->pLast->pNext = pNode;
            pList->pLast        = pNode;
        }

        pList->count++;
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglListRemove - Remove an element from the list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglListRemove (
    UGL_LIST *  pList,
    UGL_NODE *  pNode
    ) {
    UGL_STATUS  status;

    if (pList == UGL_NULL || pList->count == 0) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pList->count--;

        if (pList->count == 0) {
            pList->pFirst = UGL_NULL;
            pList->pLast  = UGL_NULL;
        }
        else if (pNode == pList->pFirst) {
            pList->pFirst        = pNode->pNext;
            pList->pFirst->pPrev = UGL_NULL;
        }
        else if (pNode == pList->pLast) {
            pList->pLast        = pNode->pPrev;
            pList->pLast->pNext = UGL_NULL;
        }
        else {
            pNode->pNext->pPrev = pNode->pPrev;
            pNode->pPrev->pNext = pNode->pNext;
        }

        pNode->pNext = UGL_NULL;
        pNode->pPrev = UGL_NULL;

        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglListCount - Get number of elements in list
 *
 * RETURNS: Number of list elements
 */

UGL_SIZE uglListCount (
    UGL_LIST *  pList
    ) {
    UGL_SIZE  count;

    if (pList == NULL) {
        count = 0;
    }
    else {
        count = pList->count;
    }

    return count;
}

/******************************************************************************
 *
 * uglListFirst - Get first node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

UGL_NODE * uglListFirst (
    UGL_LIST *  pList
    ) {
    UGL_NODE *  pNode;

    if (pList == UGL_NULL) {
        pNode = UGL_NULL;
    }
    else {
        pNode = pList->pFirst;
    }

    return pNode;
}

/******************************************************************************
 *
 * uglListLast - Get last node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

UGL_NODE * uglListLast (
    UGL_LIST *  pList
    ) {
    UGL_NODE *  pNode;

    if (pList == UGL_NULL) {
        pNode = UGL_NULL;
    }
    else {
        pNode = pList->pLast;
    }

    return pNode;
}

/******************************************************************************
 *
 * uglListNth - Get n-th node on list
 *
 * RETURNS: Pointer to list node or UGL_NULL
 */

UGL_NODE * uglListNth (
    UGL_LIST *  pList,
    UGL_ORD     n
    ) {
    UGL_NODE *  pNode;

    if (pList == UGL_NULL) {
        pNode = UGL_NULL;
    }
    else {
        if (n >= 0) {
            for (pNode = pList->pFirst;
                 n > 0 && pNode != UGL_NULL;
                 n--, pNode = pNode->pNext);
        }
        else {
            for (pNode = pList->pLast;
                 n < -1 && pNode != UGL_NULL;
                 n++, pNode = pNode->pPrev);
        }
    }

    return pNode;
}

/******************************************************************************
 *
 * uglListNext - Get next node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

UGL_NODE * uglListNext (
    UGL_NODE *  pNode
    ) {
    UGL_NODE *  pResult;

    if (pNode == UGL_NULL) {
        pResult = UGL_NULL;
    }
    else {
        pResult = pNode->pNext;
    }

    return pResult;
}

/******************************************************************************
 *
 * uglListPrev - Get previous node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

UGL_NODE * uglListPrev (
    UGL_NODE *  pNode
    ) {
    UGL_NODE *  pResult;

    if (pNode == UGL_NULL) {
        pResult = UGL_NULL;
    }
    else {
        pResult = pNode->pPrev;
    }

    return pResult;
}

