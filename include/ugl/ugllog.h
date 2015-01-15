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

/* ugllog.h - Universal graphics library loggin function header */

#ifndef _ugllog_h
#define _ugllog_h

/* Error types */
#define UGL_ERR_TYPE_USER     (0 << 24)
#define UGL_ERR_TYPE_INFO     (1 << 24)
#define UGL_ERR_TYPE_WARN     (2 << 24)
#define UGL_ERR_TYPE_FATAL    (3 << 24)
#define UGL_ERR_TYPE_MASK     (3 << 24)

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int UGL_LOG_ID;

/* Functions */

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
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _ugllog_h */

