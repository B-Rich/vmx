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

/* uglRegionP.h - Private header for regions */

#ifndef _uglRegionP_h
#define _uglRegionP_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct ugl_region_rect {
    UGL_RECT                 rect;
    struct ugl_region_rect * pNextTL2BR;
    struct ugl_region_rect * pPrevTL2BR;
    struct ugl_region_rect * pNextTR2BL;
    struct ugl_region_rect * pPrevTR2BL;
} UGL_REGION_RECT;

typedef struct ugl_region_block {
    UGL_REGION_RECT *         pRectBlock;
    struct ugl_region_block * pNextBlock;
} UGL_REGION_BLOCK;

typedef struct ugl_region {
    UGL_REGION_RECT * pFirstTL2BR;
    UGL_REGION_RECT * pLastTL2BR;
    UGL_REGION_RECT * pFirstTR2BL;
    UGL_REGION_RECT * pLastTR2BL;
} UGL_REGION;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglRegionP_h */

