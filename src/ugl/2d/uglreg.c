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

/* uglreg.c - Universal graphics library registry */

#include "ugl.h"
#include "ugllog.h"

/* Defines */

#define UGL_REG_MAX_ENTRIES      10

#define UGL_REG_SEARCH_TYPE    0x01
#define UGL_REG_SEARCH_ID      0x02
#define UGL_REG_SEARCH_DATA    0x04
#define UGL_REG_SEARCH_NAME    0x08

#define UGL_REG_NODE_AVAILABLE ((UGL_REG_NODE *) (-1))

/* Types */

typedef struct ugl_reg_node {
    UGL_REG_DATA            data;
    struct ugl_reg_node    *pNext;
} UGL_REG_NODE;

/* Globals */

UGL_LOCK_ID uglRegistryLock = UGL_NULL;

/* Locals */

UGL_LOCAL UGL_REG_NODE *pRegistryList = UGL_NULL;
UGL_LOCAL UGL_REG_NODE  registryHeap[UGL_REG_MAX_ENTRIES];
UGL_LOCAL UGL_ORD       nodeCount = 0;
UGL_LOCAL UGL_UINT32    uglRegistrySearchFields = 0;
UGL_LOCAL UGL_REG_DATA  uglRegistrySearchData;

UGL_LOCAL UGL_BOOL uglRegistryCompare (
    UGL_REG_NODE *pNode,
    UGL_REG_DATA *pData,
    UGL_UINT32    searchFields
    );

/******************************************************************************
 *
 * uglRegistryInit - Initialize registry
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegistryInit (
    void
    ) {
    UGL_STATUS  status;

    uglRegistryLock = uglOSLockCreate();
    if (uglRegistryLock == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglRegistryDeinit - De-initialize registry
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegistryDeinit (
    void
    ) {
    UGL_STATUS  status;

    if (uglOSLockDestroy(uglRegistryLock) != UGL_STATUS_OK) {
        status = UGL_STATUS_ERROR;
    }
    else {
        uglRegistryLock = UGL_NULL;
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglRegistryAdd - Add entry to registry
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegistryAdd (
    UGL_UINT32  type,
    UGL_ARG     data,
    UGL_UINT32  id,
    UGL_CHAR   *name
    ) {
    UGL_STATUS    status;
    UGL_ORD       i;
    UGL_REG_NODE *pNode = UGL_NULL;
    UGL_REG_DATA *pData = UGL_NULL;

    uglOSLock(uglRegistryLock);

    /* Initialize if no nodes yet */
    if (nodeCount == 0) {
        for (i = 0; i < UGL_REG_MAX_ENTRIES; i++) {
            registryHeap[i].pNext = UGL_REG_NODE_AVAILABLE;
        }
    }

    if (nodeCount == UGL_REG_MAX_ENTRIES) {
        status = UGL_STATUS_ERROR;
    }
    else {
        nodeCount++;

        /* Find first free slot */
        for (i = 0; i < UGL_REG_MAX_ENTRIES; i++) {
            if (registryHeap[i].pNext == UGL_REG_NODE_AVAILABLE) {
                pNode = &registryHeap[i];
                break;
            }
        }

        if (pNode == UGL_NULL) {
            uglLog(
                UGL_ERR_TYPE_WARN,
                "Internal registry error.\n",
                (UGL_ARG) 0,
                (UGL_ARG) 0,
                (UGL_ARG) 0,
                (UGL_ARG) 0,
                (UGL_ARG) 0
                );
            status = UGL_STATUS_ERROR;
        }
        else {
            pData = (UGL_REG_DATA *) pNode;

            /* Place new entry in registry */
            pData->type = type;
            pData->id   = id;
            pData->data = data;
            if (name != UGL_NULL) {
                strncpy(pData->name, name, UGL_REG_MAX_NAME_LENGTH);
            }
            else {
                strncpy(pData->name, "", UGL_REG_MAX_NAME_LENGTH);
            }

            /* Add new node to head of list */
            pNode->pNext = pRegistryList;
            pRegistryList = pNode;
            status = UGL_STATUS_OK;
        }
    }

    uglOSUnlock(uglRegistryLock);

    return status;
}

/******************************************************************************
 *
 * uglRegistryRemove - Remove entry from registry
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegistryRemove (
    UGL_REG_DATA  *pData
    ) {
    UGL_STATUS     status;
    UGL_REG_NODE  *pNode     = (UGL_REG_NODE *) pData;
    UGL_REG_NODE  *pCurrNode = pRegistryList;

    if (pNode == UGL_NULL || pCurrNode == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {

        uglOSLock(uglRegistryLock);

        /* Check if head node matches */
        if (pCurrNode == pNode) {
            pRegistryList = pNode->pNext;
            pNode->pNext = UGL_REG_NODE_AVAILABLE;
            nodeCount--;
            status = UGL_STATUS_OK;
        }
        else {
            status = UGL_STATUS_ERROR;

            while (pCurrNode->pNext != UGL_NULL) {

                /* Check for node */
                if (pCurrNode->pNext == pNode) {
                    pCurrNode->pNext = pNode->pNext;
                    pNode->pNext = UGL_REG_NODE_AVAILABLE;
                    nodeCount--;
                    status = UGL_STATUS_OK;
                    break;
                }

                /* Advance */
                pCurrNode = pNode->pNext;
            }
        }

        uglOSUnlock(uglRegistryLock);
    }

    return status;
}

/******************************************************************************
 *
 * uglRegistryFind - Find entry in registry
 *
 * RETURNS: Pointer to registry data or UGL_NULL
 */

UGL_REG_DATA* uglRegistryFind (
    UGL_UINT32  type,
    UGL_ARG    *pData,
    UGL_UINT32  id,
    UGL_CHAR   *name
    ) {
    UGL_REG_NODE  *pNode = pRegistryList;

    uglOSLock(uglRegistryLock);

    uglRegistrySearchFields = 0x00;

    if (type != 0) {
        uglRegistrySearchFields    |= UGL_REG_SEARCH_TYPE;
        uglRegistrySearchData.type  = type;
    }

    if (id != 0) {
        uglRegistrySearchFields  |= UGL_REG_SEARCH_ID;
        uglRegistrySearchData.id  = id;
    }

    if (pData != UGL_NULL) {
        uglRegistrySearchFields    |= UGL_REG_SEARCH_DATA;
        uglRegistrySearchData.data  = *pData;
    }

    if (name != UGL_NULL) {
        uglRegistrySearchFields |= UGL_REG_SEARCH_NAME;
        strncpy(uglRegistrySearchData.name, name, UGL_REG_MAX_NAME_LENGTH);
    }

    /* Search for node */
    while (pNode != UGL_NULL) {

        if (uglRegistryCompare(
                pNode,
                &uglRegistrySearchData,
                uglRegistrySearchFields
                ) == UGL_TRUE) {
            break;
        }

        /* Advance */
        pNode = pNode->pNext;
    }

    uglOSUnlock(uglRegistryLock);

    return (UGL_REG_DATA *) pNode;
}

/******************************************************************************
 *
 * uglRegistryFindNext - Find next entry in registry
 *
 * RETURNS: Pointer to registry data or UGL_NULL
 */

UGL_REG_DATA* uglRegistryFindNext (
    UGL_REG_DATA *pData
    ) {
    UGL_BOOL      found = UGL_FALSE;
    UGL_REG_NODE *pNode = (UGL_REG_NODE *) pData;

    if (pNode != UGL_NULL) {

        uglOSLock(uglRegistryLock);

        while (pNode->pNext != UGL_NULL) {
            pNode = pNode->pNext;

            if (uglRegistryCompare(
                    pNode,
                    &uglRegistrySearchData,
                    uglRegistrySearchFields
                    ) == UGL_TRUE) {

                found = UGL_TRUE;
                break;
            }
        }

        uglOSUnlock(uglRegistryLock);
    }

    if (found != UGL_TRUE) {
        pNode = UGL_NULL;
    }

    return (UGL_REG_DATA *) pNode;
}

/******************************************************************************
 *
 * uglRegistryCompare - Compare registry nodes
 *
 * RETURNS: UGL_TRUE or UGL_FALSE
 */

UGL_LOCAL UGL_BOOL uglRegistryCompare (
    UGL_REG_NODE *pNode,
    UGL_REG_DATA *pData,
    UGL_UINT32    searchFields
    ) {
    UGL_BOOL      result = UGL_TRUE;
    UGL_REG_DATA *pCurrData = (UGL_REG_DATA *) pNode;

    if ((searchFields & UGL_REG_SEARCH_TYPE) != 0x00) {
        if (pCurrData->type != pData->type) {
            result = UGL_FALSE;
        }
    }

    if ((searchFields & UGL_REG_SEARCH_ID) != 0x00) {
        if (pCurrData->id != pData->id) {
            result = UGL_FALSE;
        }
    }

    if ((searchFields & UGL_REG_SEARCH_DATA) != 0x00) {
        if (pCurrData->data != pData->data) {
            result = UGL_FALSE;
        }
    }

    if ((searchFields & UGL_REG_SEARCH_NAME) != 0x00) {
        if (strcmp(pCurrData->name, pData->name) != 0) {
            result = UGL_FALSE;
        }
    }

    return result;
}

