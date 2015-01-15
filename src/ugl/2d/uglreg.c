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

/* Macros */

#define UGL_REG_NODE_EMPTY        ((UGL_REG_NODE *) (-1))

/* Types */

typedef struct ugl_reg_node {
    UGL_REG_DATA            data;
    struct ugl_reg_node    *pNext;
} UGL_REG_NODE;

/* Globals */

UGL_LOCK_ID uglRegistryLock = UGL_NULL;

/* Locals */

UGL_LOCAL UGL_REG_NODE  *pRegistryList = UGL_NULL;
UGL_LOCAL UGL_REG_NODE  registryHeap[UGL_REG_MAX_ENTRIES];
UGL_LOCAL UGL_ORD       nodeCount = 0;

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
            registryHeap[i].pNext = UGL_REG_NODE_EMPTY;
        }
    }

    if (nodeCount == UGL_REG_MAX_ENTRIES) {
        status = UGL_STATUS_ERROR;
    }
    else {
        nodeCount++;

        /* Find first free slot */
        for (i = 0; i < UGL_REG_MAX_ENTRIES; i++) {
            if (registryHeap[i].pNext == UGL_REG_NODE_EMPTY) {
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
            status = UGL_STATUS_ERROR;
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
            pNode->pNext = UGL_REG_NODE_EMPTY;
            nodeCount--;
            status = UGL_STATUS_OK;
        }
        else {
            status = UGL_STATUS_ERROR;

            while (pCurrNode->pNext != UGL_NULL) {

                /* Check for node */
                if (pCurrNode->pNext == pNode) {
                    pCurrNode->pNext = pNode->pNext;
                    pNode->pNext = UGL_REG_NODE_EMPTY;
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

