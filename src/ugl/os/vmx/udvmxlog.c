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

/* udvmxlog.c - Operating system specific log function */

#include <string.h>
#include <vmx/taskLib.h>
#include "ugl.h"
#include "ugllog.h"

#define UGL_LOG_STRING_LEN    70
#define UGL_LOG_PREFIX_LEN    30

/* Locals */

UGL_LOCAL UGL_CHAR *uglMsgClass[] = {
    "USER",
    "INFO",
    "WARN",
    "FATAL"
    };

/******************************************************************************
 *
 * uglLog - Log message to operating system
 *
 * RETURNS: UGL_STATUS_OK
 */

UGL_STATUS uglLog (
    UGL_LOG_ID  msgType,
    UGL_CHAR   *fmt,
    UGL_ARG     arg0,
    UGL_ARG     arg1,
    UGL_ARG     arg2,
    UGL_ARG     arg3,
    UGL_ARG     arg4
    ) {
    static UGL_CHAR  str[UGL_LOG_STRING_LEN + UGL_LOG_PREFIX_LEN];

    strcpy(str, "UGL <%s> -> ");
    strncat(str, fmt, UGL_LOG_STRING_LEN);

    printf(str,
           (int) uglMsgClass[(msgType >> 24) & 3],
           (int) arg0,
           (int) arg1,
           (int) arg2,
           (int) arg3,
           (int) arg4
           );

    if ((msgType & UGL_ERR_TYPE_MASK) == UGL_ERR_TYPE_FATAL) {
        printf(" Fatal UGL error, aborting task ....\n");
        while (1) {
            taskSuspend(0);
        }
    }

    return UGL_STATUS_OK;
}

