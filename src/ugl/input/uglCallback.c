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

/* uglCallback.c - Universal graphics library callbacks */

#include <string.h>
#include "ugl.h"
#include "uglinput.h"
#include "uglMsg.h"
#include "private/uglMsgP.h"

#define UGL_CB_ARRAY_START_SIZE         4
#define UGL_CB_ARRAY_SIZE_DELTA(_sz)    ((_sz) = (_sz) * 3 / 2)

/* Locals */

UGL_LOCAL UGL_STATUS uglCbArrayRealloc (
    UGL_CB_LIST_ID  cbListId
    );

/******************************************************************************
 *
 * uglCbListCreate - Create callback list
 *
 * RETURNS: UGL_CB_LIST_ID or UGL_NULL
 */

UGL_CB_LIST_ID uglCbListCreate (
    const  UGL_CB_ITEM  *pCbArray
    ) {
    UGL_CB_LIST  *pCbList;

    pCbList = (UGL_CB_LIST *) UGL_CALLOC(1, sizeof(UGL_CB_LIST));
    if (pCbList != UGL_NULL) {
        if (uglCbAddArray(pCbList, pCbArray) != UGL_STATUS_OK) {
            UGL_FREE(pCbList);
            pCbList = UGL_NULL;
        }
    }

    return pCbList;
}

/******************************************************************************
 *
 * uglCbListDestroy - Destroy callback list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbListDestroy (
    UGL_CB_LIST_ID  cbListId
    ) {
    UGL_STATUS  status;

    if (cbListId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (cbListId->arraySize > 0) {
            UGL_FREE(cbListId->pCbArray);
        }

        UGL_FREE(cbListId);
        status = UGL_STATUS_OK;
    }

    return status;
}

/******************************************************************************
 *
 * uglCbAdd - Add callback to callback list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbAdd (
    UGL_CB_LIST_ID  cbListId,
    UGL_UINT32      filterMin,
    UGL_UINT32      filterMax,
    UGL_CB         *pCallback,
    void           *pParam
    ) {
    UGL_STATUS  status;
    UGL_ORD     i;

    if (cbListId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (cbListId->numCallbacks >= cbListId->arraySize) {
            status = uglCbArrayRealloc(cbListId);
            if (status == UGL_STATUS_OK) {
                i = cbListId->numCallbacks;

                cbListId->pCbArray[i].filterMin = filterMin;
                cbListId->pCbArray[i].filterMax = filterMax;
                cbListId->pCbArray[i].pCallback = pCallback;
                cbListId->pCbArray[i].pParam    = pParam;
                cbListId->numCallbacks++;
            }
        }
        else {
            status = UGL_STATUS_OK;
        }
    }

    return status;
}

/******************************************************************************
 *
 * uglCbAddArray - Add array of callbacks to callback list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbAddArray (
    UGL_CB_LIST_ID    cbListId,
    const UGL_CB_ITEM *pCbArray
    ) {

    return uglCbAdd(cbListId, 0, 0, UGL_NULL, (void *) pCbArray);
}

/******************************************************************************
 *
 * uglCbRemove - Remove callback from list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbRemove (
    UGL_CB_LIST_ID  cbListId,
    UGL_CB         *pCallback
    ) {
    UGL_STATUS  status;
    UGL_ORD     i;
    UGL_ORD     index = -1;

    if (cbListId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (cbListId->pCbArray != UGL_NULL) {

            /* Find callback in list */
            for (i = 0; i < cbListId->numCallbacks; i++) {
                if (cbListId->pCbArray[i].pCallback == pCallback) {

                    index = i;
                    break;
                }
            }
        }

        if (index == -1) {
            status = UGL_STATUS_ERROR;
        }
        else {
            memmove(&cbListId->pCbArray[index],
                    &cbListId->pCbArray[index + 1],
                    (cbListId->numCallbacks - index - 1) * sizeof(UGL_CB_ITEM));
            cbListId->numCallbacks--;
            status = UGL_STATUS_OK;
        }
    }

    return status;
}

/******************************************************************************
 *
 * uglCbRemoveArray - Remove array of callbacks from list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbRemoveArray (
    UGL_CB_LIST_ID     cbListId,
    const UGL_CB_ITEM *pCbArray
    ) {
    UGL_STATUS  status;
    UGL_ORD     i;
    UGL_ORD     index = -1;

    if (cbListId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        if (cbListId->pCbArray != UGL_NULL) {

            /* Find array in callback list */
            for (i= 0; i < cbListId->numCallbacks; i++) {
                if (cbListId->pCbArray[i].pCallback == UGL_NULL &&
                    cbListId->pCbArray[i].pParam == pCbArray) {

                    index = i;
                    break;
                }
            }
        }

        if (index == -1) {
            status = UGL_STATUS_ERROR;
        }
        else {
            memmove(&cbListId->pCbArray[index],
                    &cbListId->pCbArray[index + 1],
                    (cbListId->numCallbacks - index - 1) * sizeof(UGL_CB_ITEM));
            cbListId->numCallbacks--;
            status = UGL_STATUS_OK;
        }
    }

    return status;
}

/******************************************************************************
 *
 * uglCbListExecute - Execute callback list
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglCbListExecute (
    UGL_CB_LIST_ID  cbListId,
    UGL_ID          objectId,
    UGL_MSG        *pMsg,
    void           *pParam
    ) {
    UGL_CB_ITEM *pCbArray;
    UGL_CB_ITEM *pCbItem;
    UGL_ORD      i;
    UGL_ORD      j;
    UGL_UINT32   filterMin;
    UGL_UINT32   filterMax;
    UGL_STATUS   status = UGL_STATUS_OK;

    if (cbListId == UGL_NULL) {
        status = UGL_STATUS_ERROR;
    }
    else {
        pCbArray = cbListId->pCbArray;

        for (i = cbListId->numCallbacks - 1; i >= 0; i--) {
            filterMin = pCbArray[i].filterMin;
            filterMax = pCbArray[i].filterMax;

            if (filterMin == 0) {
                filterMax = 0xffffffff;
            }
            else if (filterMax == 0) {
                filterMax = filterMin;
            }

            if (filterMin <= pMsg->type &&
                filterMax >= pMsg->type) {

                if (pCbArray[i].pCallback != UGL_NULL) {
                    status = (*pCbArray[i].pCallback)(
                        objectId,
                        pMsg,
                        pParam,
                        pCbArray[i].pParam
                        );
                }
                else if (pCbArray[i].pParam != UGL_NULL) {
                    pCbItem = (UGL_CB_ITEM *) pCbArray[i].pParam;

                    for (j = 0; pCbItem[j].pCallback != UGL_NULL; j++) {
                        filterMin = pCbItem[j].filterMin;
                        filterMax = pCbItem[j].filterMax;

                        if (filterMin == 0) {
                            filterMax = 0xffffffff;
                        }
                        else if (filterMax == 0) {
                            filterMax = filterMin;
                        }

                        if (filterMin <= pMsg->type &&
                            filterMax >= pMsg->type) {

                            status = (*pCbItem[j].pCallback)(
                                objectId,
                                pMsg,
                                pParam,
                                pCbItem[j].pParam
                            );
                        }
                    }
                }
            }

            if (status != UGL_STATUS_OK) {
                break;
            }
        }
    }

    return status;
}

/******************************************************************************
 *
 * uglCbArrayRealloc - Reallocate storage for array
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_LOCAL UGL_STATUS uglCbArrayRealloc (
    UGL_CB_LIST_ID  cbListId
    ) {
    UGL_STATUS  status;
    UGL_SIZE    arraySize = cbListId->arraySize;

    if (cbListId->numCallbacks >= cbListId->arraySize) {
        if (cbListId->pCbArray == UGL_NULL) {
            arraySize = UGL_CB_ARRAY_START_SIZE;
        }
        else {
            arraySize = cbListId->numCallbacks;
            UGL_CB_ARRAY_SIZE_DELTA(arraySize);
        }

        cbListId->pCbArray = (UGL_CB_ITEM *) UGL_REALLOC(
            cbListId->pCbArray,
            arraySize * sizeof(UGL_CB_ITEM)
            );

        if (cbListId->pCbArray == UGL_NULL) {
            status = UGL_STATUS_ERROR;
        }
        else {
            status = UGL_STATUS_OK;
        }
    }
    else {
        status = UGL_STATUS_OK;
    }

    return status;
}

