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

/* winList.c - List for windows */

#include "uglmem.h"
#include "winList.h"

/******************************************************************************
 *
 * winListCreate - Create window list
 *
 * RETURNS: Pointer to list or UGL_NULL
 */

WIN_LIST * winListCreate (
    void
    ) {
    WIN_LIST *  pList;

    pList = UGL_CALLOC(1, sizeof(WIN_LIST));

    return pList;
}

/******************************************************************************
 *
 * winListInit - Initialize linked list datastruct
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

STATUS winListInit (
    WIN_LIST *  pList
    ) {
    UGL_STATUS  status;

    if (pList == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pList->pFirst  = UGL_NULL;
        pList->pLast   = UGL_NULL;
        pList->count   = 0;
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winListDestroy - Destroy window list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winListDestroy (
    WIN_LIST *  pList
    ) {
    UGL_STATUS  status;
    WIN_NODE *  pNode;
    WIN_NODE *  pNextNode;

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
 * winListInsert - Insert element in list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winListInsert (
    WIN_LIST *  pList,
    WIN_NODE *  pNode,
    WIN_NODE *  pNextNode
    ) {
    UGL_STATUS  status;
  
    if (pList == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (pList->count == 0 || pNextNode == UGL_NULL) {
            status = winListAdd(pList, pNode);
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
 * winListAdd - Add an element to the list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winListAdd (
    WIN_LIST *  pList,
    WIN_NODE *  pNode
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
 * winListRemove - Remove an element from the list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winListRemove (
    WIN_LIST *  pList,
    WIN_NODE *  pNode
    ) {
    UGL_STATUS  status;

    if (pList == UGL_NULL || pNode == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {

        if (pNode->pPrev == UGL_NULL) {
            pList->pFirst = pNode->pNext;
        }
        else {
            pNode->pPrev->pNext = pNode->pNext;
        }

        if (pNode->pNext == UGL_NULL) {
            pList->pLast = pNode->pPrev;
        }
        else {
            pNode->pNext->pPrev = pNode->pPrev;
        }

        pList->count--;
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * winListCount - Get number of elements in list
 *
 * RETURNS: Number of list elements
 */

UGL_SIZE winListCount (
    WIN_LIST *  pList
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
 * winListFirst - Get first node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

WIN_NODE * winListFirst (
    WIN_LIST *  pList
    ) {
    WIN_NODE *  pNode;

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
 * winListLast - Get last node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

WIN_NODE * winListLast (
    WIN_LIST *  pList
    ) {
    WIN_NODE *  pNode;

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
 * winListGet - Get Node from list
 *
 * RETURNS: Pointer to list node or UGL_NULL
 */

WIN_NODE * winListGet (
    WIN_LIST *  pList
    ) {
    WIN_NODE *  pNode = UGL_NULL;

    if (pList != UGL_NULL) {
        pNode = pList->pFirst;

        if (pNode != UGL_NULL) {
            pList->pFirst = pNode->pNext;

            if (pNode->pNext == UGL_NULL) {
                pList->pLast = UGL_NULL;
            }
            else {
                pNode->pNext->pPrev = UGL_NULL;
            }

            pList->count--;
        }
    }

    return pNode;
}

/******************************************************************************
 *
 * winListNth - Get n-th node on list
 *
 * RETURNS: Pointer to list node or UGL_NULL
 */

WIN_NODE * winListNth (
    WIN_LIST *  pList,
    UGL_ORD     n
    ) {
    WIN_NODE *  pNode;

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
                 pNode = pNode->pPrev);
        }
    }

    return pNode;
}

/******************************************************************************
 *
 * winListNext - Get next node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

WIN_NODE * winListNext (
    WIN_NODE *  pNode
    ) {
    WIN_NODE *  pResult;

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
 * winListPrev - Get previous node on list
 *
 * RETURNS: Pointer to node or UGL_NULL
 */

WIN_NODE * winListPrev (
    WIN_NODE *  pNode
    ) {
    WIN_NODE *  pResult;

    if (pNode == UGL_NULL) {
        pResult = UGL_NULL;
    }
    else {
        pResult = pNode->pPrev;
    }

    return pResult;
}

