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

/* excI386Show.h - Exception info show header */

#ifndef _excI386Show_h
#define _excI386Show_h

#include <vmx.h>

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Functions */

/******************************************************************************
 * excInfoShow - Show exception info
 *
 * RETURNS: N/A
 */

void excInfoShow(
    EXC_INFO *pExcInfo,
    BOOL doBell
    );

/******************************************************************************
 * excIntInfoShow - Show interrupt info
 *
 * RETURNS: N/A
 */

void excIntInfoShow(
    int vecNum,
    ESF0 *pEsf,
    REG_SET *pRegSet,
    EXC_INFO *pExcInfo
    );

/******************************************************************************
 * excPanicShow - Show fatal exception info
 *
 * RETURNS: N/A
 */
void excPanicShow(
    int vecNum,
    ESF0 *pEsf,
    REG_SET *pRegSet,
    EXC_INFO *pExcInfo
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _excI386Show_h */

