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

/* uglcolor.c - Universal graphics library color support */

#include <string.h>

#include "ugl.h"
#include "driver/graphics/generic/udgen.h"

/* Locals */

UGL_LOCAL UGL_ORD  uglHLS2RGBValue (
    UGL_ORD  n1,
    UGL_ORD  n2,
    UGL_ORD  hue
    );

/******************************************************************************
 *
 * uglClutSet
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglClutSet (
    UGL_DEVICE_ID  devId,
    UGL_ORD        offset,
    UGL_ARGB *     pColors,
    UGL_SIZE       numColors
    ) {

    if (devId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Lock device */
    if (uglOSLock (devId->lockId) != UGL_STATUS_OK) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific function */
    if ((*devId->clutSet) (devId, offset, pColors, numColors) !=
        UGL_STATUS_OK) {
        uglOSUnlock (devId->lockId);
        return (UGL_STATUS_ERROR);
    }

    /* Unlock */
    uglOSUnlock(devId->lockId);

    return (UGL_STATUS_OK);
}

/******************************************************************************
 *
 * uglClutGet
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglClutGet (
    UGL_DEVICE_ID  devId,
    UGL_ORD        offset,
    UGL_ARGB *     pColors,
    UGL_SIZE       numColors
    ) {

    if (devId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Lock device */
    if (uglOSLock (devId->lockId) != UGL_STATUS_OK) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    if ((*devId->clutGet) (devId, offset, pColors, numColors) !=
        UGL_STATUS_OK) {
        uglOSUnlock (devId->lockId);
        return (UGL_STATUS_ERROR);
    }

    /* Unlock */
    uglOSUnlock (devId->lockId);

    return (UGL_STATUS_OK);
}

/******************************************************************************
 *
 * uglColorAllocExt - Allocate color extended
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglColorAllocExt (
    UGL_DEVICE_ID  devId,
    UGL_ARGB *     pReqColors,
    UGL_ORD *      pIndex,
    UGL_ARGB *     pActualColors,
    UGL_COLOR *    pUglColors,
    UGL_SIZE       numColors
    ) {

    if (devId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Lock device */
    if (uglOSLock (devId->lockId) != UGL_STATUS_OK) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    if ((*devId->colorAlloc) (devId, pReqColors, pIndex, pActualColors,
                              pUglColors, numColors) != UGL_STATUS_OK) {
        uglOSUnlock (devId->lockId);
        return (UGL_STATUS_ERROR);
    }

    /* Unlock */
    uglOSUnlock (devId->lockId);

    return (UGL_STATUS_OK);
}

/******************************************************************************
 *
 * uglColorAlloc - Allocate color
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglColorAlloc (
    UGL_DEVICE_ID  devId,
    UGL_ARGB *     pReqColors,
    UGL_ORD *      pIndex,
    UGL_COLOR *    pUglColors,
    UGL_SIZE       numColors
    ) {

    if (devId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Lock device */
    if (uglOSLock (devId->lockId) != UGL_STATUS_OK) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    if ((*devId->colorAlloc) (devId, pReqColors, pIndex, UGL_NULL,
                              pUglColors, numColors) != UGL_STATUS_OK) {
        uglOSUnlock (devId->lockId);
        return (UGL_STATUS_ERROR);
    }

    /* Unlock */
    uglOSUnlock (devId->lockId);

    return (UGL_STATUS_OK);
}

/******************************************************************************
 *
 * uglColorFree - Free color
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglColorFree (
    UGL_DEVICE_ID  devId,
    UGL_COLOR *    pColors,
    UGL_SIZE       numColors
    ) {

    if (devId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Lock device */
    if (uglOSLock (devId->lockId) != UGL_STATUS_OK) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    if ((*devId->colorFree) (devId, pColors, numColors) != UGL_STATUS_OK) {
        uglOSUnlock (devId->lockId);
        return (UGL_STATUS_ERROR);
    }

    /* Unlock */
    uglOSUnlock (devId->lockId);

    return (UGL_STATUS_OK);
}

/******************************************************************************
 *
 * uglARGBSpecGet - Decode UGL_COLOR_FORMAT to ARGB
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglARGBSpecGet (
    UGL_COLOR_FORMAT  format,
    UGL_ARGB_SPEC *   pSpec
    ) {

    /* Check params */
    if (pSpec == UGL_NULL || (format & 0xfff00000) != UGL_CM_ARGB) {
        return (UGL_STATUS_ERROR);
    }

    pSpec->numBytesPerARGB = (UGL_UINT8) ((format & 0x000f0000) >> 16);
    if (pSpec->numBytesPerARGB > 4) {
        return (UGL_STATUS_ERROR);
    }

    /* Get number of bits per component */
    pSpec->nAlphaBits = (UGL_UINT8) ((format & 0x0000f000) >> 12);
    pSpec->nRedBits   = (UGL_UINT8) ((format & 0x00000f00) >> 8);
    pSpec->nGreenBits = (UGL_UINT8) ((format & 0x000000f0) >> 4);
    pSpec->nBlueBits  = (UGL_UINT8) (format & 0x0000000f);

    /* Check result */
    if (pSpec->nAlphaBits > 8 || pSpec->nRedBits > 8 ||
        pSpec->nGreenBits > 8 || pSpec->nBlueBits > 8) {
        return (UGL_STATUS_ERROR);
    }

    if ((pSpec->nAlphaBits + pSpec->nRedBits +
         pSpec->nGreenBits + pSpec->nBlueBits) > (pSpec->numBytesPerARGB * 8)) {
        return (UGL_STATUS_ERROR);
    }

    /* Calculate mask per component */
    pSpec->alphaMask = (UGL_UINT8) ((1 << pSpec->nAlphaBits) - 1);
    pSpec->redMask   = (UGL_UINT8) ((1 << pSpec->nRedBits) - 1);
    pSpec->greenMask = (UGL_UINT8) ((1 << pSpec->nGreenBits) - 1);
    pSpec->blueMask  = (UGL_UINT8) ((1 << pSpec->nBlueBits) - 1);

    /* Calculate shift per component */
    pSpec->alphaShift = (UGL_UINT8) (pSpec->nRedBits + pSpec->nGreenBits +
                                     pSpec->nBlueBits);
    pSpec->redShift    = (UGL_UINT8) (pSpec->nGreenBits + pSpec->nBlueBits);
    pSpec->greenShift  = (UGL_UINT8) pSpec->nBlueBits;
    pSpec->blueShift   = (UGL_UINT8) 0;

    return (UGL_STATUS_OK);
}

/******************************************************************************
 *
 * uglARGBSpecSet - Encode ARGB to UGL_COLOR_FORMAT
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglARGBSpecSet (
    UGL_COLOR_FORMAT * pFormat,
    UGL_ARGB_SPEC *    pSpec
    ) {

    /* Check params */
    if (pFormat == UGL_NULL || pSpec == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    if (pSpec->numBytesPerARGB > 4 || pSpec->nAlphaBits > 8 ||
        pSpec->nBlueBits > 8 || pSpec->nGreenBits > 8 || pSpec->nBlueBits > 8) {
        return (UGL_STATUS_ERROR);
    }

    *pFormat = UGL_CM_ARGB | (pSpec->numBytesPerARGB << 16) | 
               (pSpec->nAlphaBits >> 12) | (pSpec->nRedBits << 8) |
               (pSpec->nGreenBits << 4) | (pSpec->nBlueBits);

    return (UGL_STATUS_OK);
}

/******************************************************************************
 *
 * uglRGB2HLS - Convert RGB color to HLS format
 *
 * RETURNS: N/A
 */

UGL_VOID  uglRGB2HLS (
    UGL_RGB    rgb,
    UGL_ORD *  pHue,
    UGL_ORD *  pLightness,
    UGL_ORD *  pSaturation
    ) {
    UGL_ORD  hue;
    UGL_ORD  lightness;
    UGL_ORD  saturation;
    UGL_ORD  minValue;
    UGL_ORD  maxValue;
    UGL_ORD  delta;

    UGL_UINT32  red   = UGL_RGB_RED(rgb);
    UGL_UINT32  green = UGL_RGB_GREEN(rgb);
    UGL_UINT32  blue  = UGL_RGB_BLUE(rgb);

    if (red > green) {
        minValue = green;
        maxValue = red;
    }
    else {
        minValue = red;
        maxValue = green;
    }

    if (blue > maxValue) {
        maxValue = blue;
    }
    else if (blue < minValue) {
        minValue = blue;
    }

    lightness = (maxValue - minValue) / 2;

    if (maxValue == minValue) {
        hue = 0;
        saturation = 0;
    }
    else {
        delta = maxValue - minValue;

        if (lightness < 128) {
            saturation = delta * 255 / (maxValue + minValue);
        }
        else {
            saturation = delta * 255 / (510 - maxValue - minValue);
        }

        if (red == maxValue) {
            hue = (green - blue) * 60 / delta;
        }
        else if (green == maxValue) {
            hue = (blue - red) * 60 / delta;
        }
        else {
            hue = (red - green) * 60 / delta;
        }

        if (hue < 0) {
            hue += 360;
        }
    }

    *pHue        = hue;
    *pLightness  = lightness;
    *pSaturation = saturation;
}

/******************************************************************************
 *
 * uglHLS2RGB - Convert HLS color to RGB format
 *
 * RETURNS: N/A
 */

UGL_VOID  uglHLS2RGB (
    UGL_ORD    hue,
    UGL_ORD    lightness,
    UGL_ORD    saturation,
    UGL_RGB *  pRgb
    ) {
    UGL_ORD  n1;
    UGL_ORD  n2;

    if (lightness < 128) {
        n2 = lightness * (saturation + 255) / 255;
    }
    else {
       n2 = lightness + saturation - lightness * saturation / 255;
    }

    n1 = 2 * lightness - n2;

    if (saturation == 0) {
        *pRgb = UGL_MAKE_RGB(lightness, lightness, lightness);
    }
    else {
        *pRgb = UGL_MAKE_RGB(
            uglHLS2RGBValue(n1, n2, hue + 120),
            uglHLS2RGBValue(n1, n2, hue),
            uglHLS2RGBValue(n1, n2, hue - 120)
            );
   }
}

/******************************************************************************
 *
 * uglHLS2RGBValue - Helper for conversion of HLS color to RGB format
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_ORD  uglHLS2RGBValue (
    UGL_ORD  n1,
    UGL_ORD  n2,
    UGL_ORD  hue
    ) {
    UGL_ORD  value;

    if (hue < 0) {
        hue += 360;
    }
    else if (hue > 360) {
        hue -= 360;
    }

    if (hue < 60) {
        value = n1 + (n2 - n1) * hue / 60;
    }
    else if (hue < 180) {
        value = n2;
    }
    else if (hue < 240) {
        value = n1 + (n2 - n1) * (240 - hue) / 60;
    }
    else {
        value = n1;
    }

    return value;
}

