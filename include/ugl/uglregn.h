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

/* uglregn.h - Universal graphics library region support */

#ifndef _uglregn_h
#define _uglregn_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Defines */

#define UGL_BLT_LEFT                    0x00000000
#define UGL_BLT_UP                      0x00000000
#define UGL_BLT_RIGHT                   0x00000001
#define UGL_BLT_DOWN                    0x00000010

#define UGL_BLT_UP_AND_LEFT             (UGL_BLT_LEFT | UGL_BLT_UP)
#define UGL_BLT_DOWN_AND_RIGHT          (UGL_BLT_DOWN | UGL_BLT_RIGHT)
#define UGL_BLT_UP_AND_RIGHT            (UGL_BLT_UP   | UGL_BLT_RIGHT)
#define UGL_BLT_DOWN_AND_LEFT           (UGL_BLT_DOWN | UGL_BLT_LEFT)

#define UGL_TL2BR                       UGL_BLT_UP_AND_LEFT
#define UGL_BR2TL                       UGL_BLT_DOWN_AND_RIGHT
#define UGL_TR2BL                       UGL_BLT_UP_AND_RIGHT
#define UGL_BL2TR                       UGL_BLT_DOWN_AND_LEFT

/* Structs */

struct  ugl_region;

/* Types */

typedef struct ugl_region * UGL_REGION_ID;
typedef UGL_UINT32          UGL_BLT_DIR;

/* Functions */

/******************************************************************************
 *
 * uglRegionCreate - Create a new region
 *
 * RETURNS: REGION_ID or UGL_NULL
 */

UGL_REGION_ID uglRegionCreate (
    void
    );

/******************************************************************************
 *
 * uglRegionInit - Initialize region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionInit (
    UGL_REGION_ID  regionId
    );

/******************************************************************************
 *
 * uglRegionDeinit - Deinitialize region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionDeinit (
    UGL_REGION_ID  regionId
    );

/******************************************************************************
 *
 * uglRegionDestroy - Destroy region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionDestroy (
    UGL_REGION_ID  regionId
    );

/******************************************************************************
 *
 * uglRegionIsEmpty - Check if region is empty
 *
 * RETURNS: UGL_TRUE or UGL_FALSE
 */

UGL_STATUS uglRegionIsEmpty (
    const UGL_REGION_ID  regionId
    );

/******************************************************************************
 *
 * uglRegionEmpty - Clear region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionEmpty (
    UGL_REGION_ID  regionId
    );

/******************************************************************************
 *
 * uglRegionCopy - Copy region from source to destination
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionCopy (
    const UGL_REGION_ID  srcRegionId,
    UGL_REGION_ID        destRegionId
    );

/******************************************************************************
 *
 * uglRegionMove - Move region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionMove (
    const UGL_REGION_ID  regionId,
    const UGL_ORD        dx,
    const UGL_ORD        dy
    );

/******************************************************************************
 *
 * uglRegionRectInclude - Include rectangle in region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionRectInclude (
    UGL_REGION_ID     regionId,
    const UGL_RECT  * pRect
    );

/******************************************************************************
 *
 * uglRegionRectExclude - Exclude rectangle from region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionRectExclude (
    UGL_REGION_ID     regionId,
    const UGL_RECT  * pRect
    );

/******************************************************************************
 *
 * uglRegionExclude - Exclude one region from another region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionExclude (
    UGL_REGION_ID  regionId,
    UGL_REGION_ID  excludeRegionId
    );

/******************************************************************************
 *
 * uglRegionIntersect - Calculate intersection between two regions
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionIntersect (
    UGL_REGION_ID  regionAId,
    UGL_REGION_ID  regionBId,
    UGL_REGION_ID  intersectRegionId
    );

/******************************************************************************
 *
 * uglRegionRectUnion - Create union between two regions
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionUnion(
    UGL_REGION_ID  regionAId,
    UGL_REGION_ID  regionBId,
    UGL_REGION_ID  unionRegionId
    );

/******************************************************************************
 *
 * uglRegionClipToRect - Clip region to rectangle
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionClipToRect (
    UGL_REGION_ID    regionId,
    const UGL_RECT * pClipRect
    );

/******************************************************************************
 *
 * uglRegionRectSortedGet - Get sorted list of rectangles from region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionRectSortedGet (
    UGL_REGION_ID     regionId,
    const UGL_RECT ** ppRect,
    UGL_BLT_DIR       rectOrder
    );

/******************************************************************************
 *
 * uglRegionRectGet - Get rectangles from region
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionRectGet (
    UGL_REGION_ID     regionId,
    const UGL_RECT ** ppRect
    );

/******************************************************************************
 *
 * uglRegionShow - Display region info as text to console
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglRegionShow (
    UGL_REGION_ID  regionId
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglregn_h */

