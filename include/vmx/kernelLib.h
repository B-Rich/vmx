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

/* kernelLib.h - Kernel library */

#ifndef _kernelLib_h
#define _kernelLib_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * kernelInit - Initialize kernel
 *
 * RETURNS: N/A
 */

void kernelInit(
    FUNCPTR rootFunc,
    unsigned rootMemSize,
    char *pMemPoolStart,
    char *pMemPoolEnd,
    unsigned excStackSize,
    int lockOutLevel
    );

/******************************************************************************
 * kernelTimeSlice - Enable/Disable round robin task scheduling
 *
 * RETURNS: OK
 */

STATUS kernelTimeSlice(
    unsigned ticks
    );

/******************************************************************************
 * kernelVersion - Return a string with kernel version
 *
 * RETURNS: N/A
 */

char* kernelVersion(
    void
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _kernelLib_h */

