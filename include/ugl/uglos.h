/******************************************************************************
 *   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 *   This file is part of Real VMX.
 *   Copyright (C) 2013 Surplus Users Ham Society
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

/* uglos.h - Universal graphics library os calls */

#ifndef _uglos_h
#define _uglos_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include "ugltypes.h"
#ifndef LINUX
#include "os/uglvmx.h"
#else
struct ugl_mem_pool;
struct ugl_lock;

typedef struct ugl_mem_pool * UGL_MEM_POOL_ID;
typedef struct ugl_lock *     UGL_LOCK_ID;

#endif

/* Memory pools */

/******************************************************************************
 *
 * uglOSMemPoolCreate - Create memory pool
 *
 * RETURNS: Memory pool id, or UGL_NULL
 */

UGL_MEM_POOL_ID uglOSMemPoolCreate (
    void *    poolAddr,
    UGL_SIZE  poolSize
    );

/******************************************************************************
 *
 * uglOSMemPoolDestroy - Destroy memory pool
 *
 * RETURNS: UGL_STATUS_ERROR
 */

UGL_STATUS uglOSMemPoolDestroy (
    UGL_MEM_POOL_ID  poolId
    );

/******************************************************************************
 *
 * uglOSMemAlloc - Allocate memory from memory pool
 *
 * RETURNS: Pointer to memory, or UGL_NULL
 */

void * uglOSMemAlloc (
    UGL_MEM_POOL_ID  poolId,
    UGL_SIZE         memSize
    );

/******************************************************************************
 *
 * uglOSMemCalloc - Allocate memory objects from memory pool and clear it
 *
 * RETURNS: Pointer to memory, or UGL_NULL
 */

void * uglOSMemCalloc (
    UGL_MEM_POOL_ID  poolId,
    UGL_ORD          numItems,
    UGL_SIZE         itemSize
    );

/******************************************************************************
 *
 * uglOSMemRealloc - Change size of allocated memory from memory pool
 *
 * RETURNS: Pointer to memory, or UGL_NULL
 */

void * uglOSMemRealloc (
    UGL_MEM_POOL_ID  poolId,
    void *           pMem,
    UGL_SIZE         memSize
    );

/******************************************************************************
 *
 * uglOSMemFree - Free memory allocated from memory pool
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSMemFree (
    void * pMem
    );

/******************************************************************************
 *
 * uglOSMemSizeGet - Get size of allocated memory
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSMemSizeGet (
    UGL_SIZE * pMemSize,
    void *     pMem
    );

/* Locks */

/******************************************************************************
 *
 * uglOSLockCreate - Create a locking mechanism
 * 
 * RETURNS: Lock identifier
 */

UGL_LOCK_ID uglOSLockCreate (
    void
    );

/******************************************************************************
 *
 * uglOSLockDestroy - Free a locking mechanism
 *
 * RETURNS: UGL_STATUS_OR or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSLockDestroy (
    UGL_LOCK_ID  lockId
    );

/******************************************************************************
 *
 * uglOSLock - Lock
 *
 * RETURNS: UGL_STATUS_OR or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSLock (
    UGL_LOCK_ID  lockId
    );

/******************************************************************************
 *
 * uglOSUnlock - Unlock
 * 
 * RETURNS: UGL_STATUS_OR or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSUnlock (
    UGL_LOCK_ID  lockId
    );

/* Task control */

/******************************************************************************
 *
 * uglOSTaskCreate - Create a task
 *
 * RETURNS: UGL_TASK_ID or error code
 */

UGL_TASK_ID uglOSTaskCreate (
    char       *name,
    UGL_FPTR    entry,
    UGL_UINT32  priority,
    UGL_UINT32  options,
    UGL_SIZE    stackSize,
    UGL_ARG     arg0,
    UGL_ARG     arg1,
    UGL_ARG     arg2,
    UGL_ARG     arg3,
    UGL_ARG     arg4
    );

/******************************************************************************
 *
 * uglOSTaskDelay - Put task to sleep
 *
 * RETURNS: N/A
 */

UGL_VOID uglOSTaskDelay (
    UGL_UINT32  msecs
    );

/******************************************************************************
 *  
 * uglOSTaskLock - Lock task
 *  
 * RETURNS: UGL_STATUS_OR or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSTaskLock (
    void
    );

/******************************************************************************
 *
 * uglOSTaskUnlock - Unlock task
 *
 * RETURNS: UGL_STATUS_OR or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSTaskUnlock (
    void
    );

/******************************************************************************
 *
 * uglOSMsgQCreate - Create message queue
 *
 * RETURNS: UGL_OS_MSG_Q_ID or UGL_NULL
 */

UGL_OS_MSG_Q_ID uglOSMsgQCreate (
    UGL_SIZE  maxMsgs,
    UGL_SIZE  maxMsgSize
    );

/******************************************************************************
 *
 * uglOSMsgQDelete - Delete message queue
 * 
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */
 
UGL_STATUS uglOSMsgQDelete(
    UGL_OS_MSG_Q_ID  qId
    );

/******************************************************************************
 *
 * uglOSMsgQPost - Post message on message queue
 *
 * RETURNS: UGL_STATUS_OK, UGL_STATUS_Q_FULL or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSMsgQPost(
    UGL_OS_MSG_Q_ID  qId,
    UGL_TIMEOUT      timeout,
    void            *pMsg,
    UGL_SIZE         msgSize
    );

/******************************************************************************
 *
 * uglOSMsgQGet - Get message from message queue
 *
 * RETURNS: UGL_STATUS_OK, UGL_STATUS_Q_EMPTY or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSMsgQGet(
    UGL_OS_MSG_Q_ID  qId,
    UGL_TIMEOUT      timeout,
    void            *pMsg,
    UGL_SIZE         msgSize
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglos_h */

