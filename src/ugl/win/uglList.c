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
 * RETURNS: List id or UGL_NULL
 */

UGL_LIST_ID uglListCreate (
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
    UGL_LIST_ID  listId
    ) {
    UGL_STATUS  status;

    if (listId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        memset(listId, 0, sizeof(UGL_LIST));
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
    UGL_LIST_ID  listId
    ) {
    UGL_STATUS  status;
    UGL_NODE *  pNode;
    UGL_NODE *  pNextNode;

    if (listId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pNode = listId->pFirst;

        /* Free all nodes */
        while (pNode != UGL_NULL) {

            pNextNode = pNode->pNext;
            UGL_FREE(pNode);
            pNode = pNextNode;
        }

        status = UGL_STATUS_OK;
    }

    UGL_FREE(listId);

    return status;
}

/******************************************************************************
 *
 * uglListInsert - Insert element in list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglListInsert (
    UGL_LIST_ID  listId,
    UGL_NODE *   pNode,
    UGL_NODE *   pNextNode
    ) {
    UGL_STATUS  status;
  
    if (listId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (listId->count == 0 || pNextNode == UGL_NULL) {
            status = uglListAdd(listId, pNode);
        }
        else if (pNextNode == listId->pFirst) {
            pNode->pNext = listId->pFirst;
            pNode->pPrev = UGL_NULL;

            listId->pFirst->pPrev = pNode;
            listId->pFirst        = pNode;
        }
        else {
            pNode->pNext = pNextNode;
            pNode->pPrev = pNextNode->pPrev;

            pNextNode->pPrev = pNode;

            pNode->pPrev->pNext = pNode;
        }

        listId->count++;
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
    UGL_LIST_ID  listId,
    UGL_NODE *   pNode
    ) {
    UGL_STATUS  status;

    if (listId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (listId->count == 0) {
            listId->pFirst = pNode;
            listId->pLast  = pNode;

            pNode->pNext = UGL_NULL;
            pNode->pPrev = UGL_NULL;
        }
        else {
            pNode->pPrev = listId->pLast;
            pNode->pNext = UGL_NULL;

            listId->pLast->pNext = pNode;
            listId->pLast        = pNode;
        }

        listId->count++;
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
    UGL_LIST_ID  listId,
    UGL_NODE *   pNode
    ) {
    UGL_STATUS  status;

    if (listId == UGL_NULL || listId->count == 0) {
        status = UGL_STATUS_ERROR;
    }
    else {
        listId->count--;

        if (listId->count == 0) {
            listId->pFirst = UGL_NULL;
            listId->pLast  = UGL_NULL;
        }
        else if (pNode == listId->pFirst) {
            listId->pFirst        = pNode->pNext;
            listId->pFirst->pPrev = UGL_NULL;
        }
        else if (pNode == listId->pLast) {
            listId->pLast        = pNode->pPrev;
            listId->pLast->pNext = UGL_NULL;
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
    UGL_LIST_ID  listId
    ) {
    UGL_SIZE  count;

    if (listId == NULL) {
        count = 0;
    }
    else {
        count = listId->count;
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
    UGL_LIST_ID  listId
    ) {
    UGL_NODE *  pNode;

    if (listId == UGL_NULL) {
        pNode = UGL_NULL;
    }
    else {
        pNode = listId->pFirst;
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
    UGL_LIST_ID  listId
    ) {
    UGL_NODE *  pNode;

    if (listId == UGL_NULL) {
        pNode = UGL_NULL;
    }
    else {
        pNode = listId->pLast;
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
    UGL_LIST_ID  listId,
    UGL_ORD      n
    ) {
    UGL_NODE *  pNode;

    if (listId == UGL_NULL) {
        pNode = UGL_NULL;
    }
    else {
        if (n >= 0) {
            for (pNode = listId->pFirst;
                 n > 0 && pNode != UGL_NULL;
                 n--, pNode = pNode->pNext);
        }
        else {
            for (pNode = listId->pLast;
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

