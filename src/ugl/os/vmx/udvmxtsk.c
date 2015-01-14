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

/* udvmxtsk.c - Universal graphics library task support */

#include <vmx/taskLib.h>

#include "ugl.h"

/* Imports */

int sysClockRateGet (void);

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
    ) {

    return taskSpawn(name, priority, options, stackSize, (FUNCPTR) entry,
                     arg0, arg1, arg2, arg3, arg4,
                     (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0);
}

/******************************************************************************
 *
 * uglOSTaskDelay - Put task to sleep
 *
 * RETURNS: N/A
 */

UGL_VOID uglOSTaskDelay (
    UGL_UINT32  msecs
    ) {
    UGL_UINT32  ticks;
    UGL_UINT32  rate;

    /* Get clock rate */
    rate = sysClockRateGet ();

    /* Calculate timeout */
    ticks = (msecs * rate + 999) / 1000;

    /* Sleep */
    taskDelay (ticks);
}

/******************************************************************************
 *
 * uglOSTaskLock - Lock task
 *
 * RETURNS: UGL_STATUS_OR or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSTaskLock (
    void
    ) {

    if (taskLock () != OK) {
        return (UGL_STATUS_ERROR);
    }

    return (UGL_STATUS_OK);
}

/******************************************************************************
 *
 * uglOSTaskUnlock - Unlock task
 *
 * RETURNS: UGL_STATUS_OR or UGL_STATUS_ERROR
 */

UGL_STATUS uglOSTaskUnlock (
    void
    ) {

    if (taskUnlock () != OK) {
        return (UGL_STATUS_ERROR);
    }

    return (UGL_STATUS_OK);
}

