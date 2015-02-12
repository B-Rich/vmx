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

/* uglfont.c - Universal graphics library font driver support */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ugl.h"

/* Types */

typedef struct ugl_font_desc_list_node {
    UGL_FONT_DESC  fontDesc;
    UGL_BOOL       discardKeep;
    UGL_BOOL       discard;
} UGL_FONT_DESC_LIST_ELMT;

typedef enum {
    UGL_NO_PRIORITY = UGL_FONT_DONT_CARE,
    UGL_FACE_NAME_PRIORITY,
    UGL_FAMILY_NAME_PRIORITY,
    UGL_PIXEL_SIZE_PRIORITY,
    UGL_WEIGHT_PRIORITY,
    UGL_ITALIC_PRIORITY,
    UGL_SPACING_PRIORITY,
    UGL_CHAR_SET_PRIORITY
} UGL_PRIORITY_TYPE;

/* Locals */

UGL_LOCAL UGL_VOID  uglFontDefInit (
    UGL_FONT_DEF *             pFontDefinition,
    UGL_FONT_DESC *            pFontDescriptor,
    UGL_FONT_DESC_LIST_ELMT *  pListArray,
    UGL_BOOL                   pixelSize,
    UGL_BOOL                   weight,
    UGL_ORD                    index,
    UGL_FONT_DESC_PRIORITY *   pFontDescPriority
    );

UGL_LOCAL UGL_VOID  uglFontFindHighestPriority (
    UGL_PRIORITY_TYPE *       priority,
    UGL_FONT_DESC_PRIORITY *  pFontDescPriority
    );

/******************************************************************************
 *
 * uglFontDriverInfo - Get information about font driver
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglFontDriverInfo (
    UGL_FONT_DRIVER_ID  fontDriverId,
    UGL_INFO_REQ        infoRequest,
    void *              pInfo
    ) {
    UGL_STATUS  status;

    /* Validate */
    if (fontDriverId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*fontDriverId->fontDriverInfo) (fontDriverId, infoRequest, pInfo);

    return (status);
}

/******************************************************************************
 *
 * uglFontDriverDestroy - Destroy font driver
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglFontDriverDestroy (
    UGL_FONT_DRIVER_ID  fontDriverId
    ) {
    UGL_STATUS  status;

    /* Validate */
    if (fontDriverId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*fontDriverId->fontDriverDestroy) (fontDriverId);

    return (status);
}

/******************************************************************************
 *
 * uglFontFindFirst - Get the first avilable font
 *
 * RETURNS: UGL_SEARCH_ID or UGL_NULL
 */

UGL_SEARCH_ID uglFontFindFirst (
    UGL_FONT_DRIVER_ID  fontDriverId,
    UGL_FONT_DESC *     pFontDescriptor
    ) {
    UGL_SEARCH_ID  searchId;

    /* Validate */
    if (fontDriverId == UGL_NULL) {
        return (UGL_NULL);
    }

    /* Call driver specific method */
    searchId = (*fontDriverId->fontFindFirst) (fontDriverId, pFontDescriptor);

    return (searchId);
}

/******************************************************************************
 *
 * uglFontFindNext - Get the next avilable font
 *
 * RETURNS: UGL_STATUS_OK, UGL_STATUS_FINISHED or UGL_STATUS_ERROR
 */

UGL_STATUS uglFontFindNext (
    UGL_FONT_DRIVER_ID  fontDriverId,
    UGL_FONT_DESC *     pFontDescriptor,
    UGL_SEARCH_ID       searchId
    ) {
    UGL_STATUS  status;

    /* Validate */
    if (fontDriverId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*fontDriverId->fontFindNext) (fontDriverId, pFontDescriptor,
                                            searchId);

    return (status);
}

/******************************************************************************
 *
 * uglFontFindClose - Terminate font search
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglFontFindClose (
    UGL_FONT_DRIVER_ID  fontDriverId,
    UGL_SEARCH_ID       searchId
    ) {
    UGL_STATUS  status;

    /* Validate */
    if (fontDriverId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*fontDriverId->fontFindClose) (fontDriverId, searchId);

    return (status);
}

/******************************************************************************
 *
 * uglFontFind - Find font based on search criteria
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  uglFontFind (
    UGL_FONT_DRIVER_ID        fontDriverId,
    UGL_FONT_DESC *           pFontDescriptor,
    UGL_FONT_DESC_PRIORITY *  pFontDescPriority,
    UGL_FONT_DEF *            pFontDefinition
    ) {
    UGL_FONT_DESC              fontDesc;
    UGL_SEARCH_ID              searchId;
    UGL_FONT_DESC_LIST_ELMT *  pListArray;
    UGL_FONT_DESC_PRIORITY     fontDescPriority;
    UGL_PRIORITY_TYPE          priority;
    UGL_ORD                    i;
    UGL_ORD                    j;
    UGL_ORD                    k;
    UGL_ORD                    deltaPrev;
    UGL_ORD                    deltaNew;
    UGL_ORD                    deltaMin;
    UGL_ORD                    deltaMax;
    UGL_ORD *                  pMatchIndices;
    UGL_STATUS                 status     = UGL_STATUS_ERROR;
    UGL_BOOL                   pixelSize  = UGL_FALSE;
    UGL_BOOL                   weight     = UGL_FALSE;
    UGL_BOOL                   exactMatch = UGL_FALSE;
    UGL_SIZE                   numFonts   = 0;
    UGL_ORD                    matchIndex = 0;
    UGL_ORD                    numValid   = 0;

    /* Check parameters */
    if (fontDriverId == UGL_NULL || pFontDescriptor == UGL_NULL ||
        pFontDescPriority == UGL_NULL || pFontDefinition == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    searchId = (*fontDriverId->fontFindFirst)(fontDriverId, &fontDesc);
    if (searchId != UGL_NULL) {
        numFonts = 1;
    }
    else {
        return (UGL_STATUS_ERROR);
    }

    while (status != UGL_STATUS_FINISHED) {
        status = (*fontDriverId->fontFindNext)(
            fontDriverId,
            &fontDesc,
            searchId
            );

        if (status != UGL_STATUS_FINISHED) {
            numFonts++;
        }
    }

    status = (*fontDriverId->fontFindClose)(fontDriverId, searchId);

    if (status == UGL_STATUS_ERROR || numFonts < 1) {
        return (UGL_STATUS_ERROR);
    }

    /* Allocate and setup font list array */
    pListArray = (UGL_FONT_DESC_LIST_ELMT *) UGL_CALLOC(
        numFonts,
        sizeof(UGL_FONT_DESC_LIST_ELMT)
        );
    if (pListArray == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    searchId = (*fontDriverId->fontFindFirst)(fontDriverId, &fontDesc);
    if (searchId == UGL_NULL) {
        UGL_FREE(pListArray);
        return (UGL_STATUS_ERROR);
    }

    memcpy(&pListArray[0].fontDesc, &fontDesc, sizeof(UGL_FONT_DESC));
    pListArray[0].discardKeep = UGL_FALSE;
    pListArray[0].discard     = UGL_FALSE;
    for (i = 1; status != UGL_STATUS_FINISHED && i < numFonts; i++) {
        status = (*fontDriverId->fontFindNext)(
            fontDriverId,
            &fontDesc,
            searchId
            );

        if (status != UGL_STATUS_FINISHED) {
            memcpy(&pListArray[i].fontDesc, &fontDesc, sizeof(UGL_FONT_DESC));
            pListArray[i].discardKeep = UGL_FALSE;
            pListArray[i].discard     = UGL_FALSE;
        }
    }

    (*fontDriverId->fontFindClose)(fontDriverId, searchId);

    if (pFontDescPriority->pixelSize != UGL_FONT_DONT_CARE) {
        if (pFontDescriptor->pixelSize.min > pFontDescriptor->pixelSize.max) {
            UGL_FREE(pListArray);
            return (UGL_STATUS_ERROR);
        }
    }

    if (pFontDescPriority->weight != UGL_FONT_DONT_CARE) {
        if (pFontDescriptor->weight.min > pFontDescriptor->weight.max) {
            UGL_FREE(pListArray);
            return (UGL_STATUS_ERROR);
        }
    }

    memcpy(
        &fontDescPriority,
        pFontDescPriority,
        sizeof(UGL_FONT_DESC_PRIORITY)
        );

    while (1) {
        if (numValid >= 1) {
            if (numValid == 1) {
                uglFontDefInit(
                    pFontDefinition,
                    pFontDescriptor,
                    pListArray,
                    pixelSize,
                    weight,
                    matchIndex,
                    &fontDescPriority
                    );

                UGL_FREE(pListArray);
                return (UGL_STATUS_OK);
            }
            else {
                for (i = 0; i < numFonts; i++) {
                    if (pListArray[i].discardKeep == UGL_TRUE &&
                        pListArray[i].discard == UGL_FALSE) {

                        pListArray[i].discard = UGL_TRUE;
                    }
                }
            }
        }
        else {
            for (i = 0; i < numFonts; i++) {
                if (pListArray[i].discardKeep == UGL_TRUE &&
                    pListArray[i].discard == UGL_FALSE) {

                    pListArray[i].discardKeep = UGL_FALSE;
                }
            }
        }

        uglFontFindHighestPriority(&priority, &fontDescPriority);

        if (priority == UGL_FACE_NAME_PRIORITY) {
            for (i = 0, numValid = 0; i < numFonts; i++) {
                if (pListArray[i].discard == UGL_FALSE) {
                    if (strncmp(
                        pListArray[i].fontDesc.faceName,
                        pFontDescriptor->faceName,
                        UGL_FONT_FACE_NAME_MAX_LENGTH - 1
                        ) == 0) {

                        numValid++;
                        matchIndex = i;
                    }
                    else {
                        pListArray[i].discardKeep = UGL_TRUE;
                    }
                }
            }

            fontDescPriority.faceName = UGL_FONT_DONT_CARE;
        }
        else if (priority == UGL_FAMILY_NAME_PRIORITY) {
            for (i = 0, numValid = 0; i < numFonts; i++) {
                if (pListArray[i].discard == UGL_FALSE) {
                    if (strncmp(
                        pListArray[i].fontDesc.familyName,
                        pFontDescriptor->familyName,
                        UGL_FONT_FAMILY_NAME_MAX_LENGTH - 1
                        ) == 0) {

                        numValid++;
                        matchIndex = i;
                    }
                    else {
                        pListArray[i].discardKeep = UGL_TRUE;
                    }
                }
            }

            fontDescPriority.familyName = UGL_FONT_DONT_CARE;
        }
        else if (priority == UGL_PIXEL_SIZE_PRIORITY) {
            k = 0;
            exactMatch = UGL_FALSE;
            pMatchIndices = (UGL_ORD *) UGL_MALLOC(numFonts * sizeof(UGL_ORD));
            if (pMatchIndices == UGL_NULL) {
                UGL_FREE(pListArray);
                return (UGL_STATUS_ERROR);
            }

            memset(pMatchIndices, -1, numFonts * sizeof(UGL_ORD));
            deltaPrev = (UGL_ORD) 0x7fff;

            for (i = 0, numValid = 0, matchIndex = 0; i < numFonts; i++) {
                if (pListArray[i].discard == UGL_FALSE) {
                    if (!(pListArray[i].fontDesc.pixelSize.min >
                          pFontDescriptor->pixelSize.max) &&
                        !(pListArray[i].fontDesc.pixelSize.max <
                          pFontDescriptor->pixelSize.min)) {

                        if (exactMatch == UGL_FALSE) {
                            numValid = 0;
                            for (j = 0, k = 0;
                                 j < numFonts && pMatchIndices[j] != -1;
                                 j++) {

                                pListArray[pMatchIndices[j]].discardKeep =
                                    UGL_TRUE;
                                pMatchIndices[j] = -1;
                            }
                        }

                        numValid++;
                        matchIndex = i;
                        pListArray[matchIndex].discardKeep = UGL_FALSE;
                        exactMatch = UGL_TRUE;
                        pixelSize = UGL_TRUE;
                    }
                    else {
                        if (exactMatch == UGL_FALSE) {
                            deltaMin = abs(
                                pListArray[i].fontDesc.pixelSize.min -
                                pFontDescriptor->pixelSize.min
                                );
                            deltaMax = abs(
                                pFontDescriptor->pixelSize.max -
                                pListArray[i].fontDesc.pixelSize.max
                                );
                            deltaNew = min(deltaMax, deltaMin);
                            if (deltaNew <= deltaPrev) {
                                if (deltaNew == deltaPrev) {
                                    numValid++;
                                    matchIndex = i;
                                    pListArray[matchIndex].discardKeep =
                                        UGL_FALSE;
                                    pMatchIndices[k++] = matchIndex;
                                    pixelSize = UGL_TRUE;
                                    deltaPrev = deltaNew;
                                }
                                else {
                                    numValid = 0;
                                    for (j = 0, k = 0;
                                         j < numFonts && pMatchIndices[j] != -1;
                                         j++) {

                                        pListArray[
                                            pMatchIndices[j]
                                            ].discardKeep = UGL_TRUE;
                                        pMatchIndices[j] = -1;
                                    }

                                    numValid++;
                                    matchIndex = i;
                                    pListArray[matchIndex].discardKeep =
                                        UGL_FALSE;
                                    pMatchIndices[k++] = matchIndex;
                                    pixelSize = UGL_TRUE;
                                    deltaPrev = deltaNew;
                                }
                            }
                            else {
                                pListArray[i].discardKeep = UGL_TRUE;
                            }
                        }
                        else {
                            pListArray[i].discardKeep = UGL_TRUE;
                        }
                    }
                }
            }

            UGL_FREE(pMatchIndices);
            fontDescPriority.pixelSize = UGL_FONT_DONT_CARE;
        }
        else if (priority == UGL_WEIGHT_PRIORITY) {
            k = 0;
            exactMatch = UGL_FALSE;
            pMatchIndices = (UGL_ORD *) UGL_MALLOC(numFonts * sizeof(UGL_ORD));
            if (pMatchIndices == UGL_NULL) {
                UGL_FREE(pListArray);
                return (UGL_STATUS_ERROR);
            }

            memset(pMatchIndices, -1, numFonts * sizeof(UGL_ORD));
            deltaPrev = (UGL_ORD) 0x7fff;

            for (i = 0, numValid = 0, matchIndex = 0; i < numFonts; i++) {
                if (pListArray[i].discard == UGL_FALSE) {
                    if (!(pListArray[i].fontDesc.weight.min >
                          pFontDescriptor->weight.max) &&
                        !(pListArray[i].fontDesc.weight.max <
                          pFontDescriptor->weight.min)) {

                        if (exactMatch == UGL_FALSE) {
                            numValid = 0;
                            for (j = 0, k = 0;
                                 j < numFonts && pMatchIndices[j] != -1;
                                 j++) {

                                pListArray[pMatchIndices[j]].discardKeep =
                                    UGL_TRUE;
                                pMatchIndices[j] = -1;
                            }
                        }

                        numValid++;
                        matchIndex = i;
                        pListArray[matchIndex].discardKeep = UGL_FALSE;
                        exactMatch = UGL_TRUE;
                        weight = UGL_TRUE;
                    }
                    else {
                        if (exactMatch == UGL_FALSE) {
                            deltaMin = abs(
                                pListArray[i].fontDesc.weight.min -
                                pFontDescriptor->weight.min
                                );
                            deltaMax = abs(
                                pFontDescriptor->weight.max -
                                pListArray[i].fontDesc.weight.max
                                );
                            deltaNew = min(deltaMax, deltaMin);
                            if (deltaNew <= deltaPrev) {
                                if (deltaNew == deltaPrev) {
                                    numValid++;
                                    matchIndex = i;
                                    pListArray[matchIndex].discardKeep =
                                        UGL_FALSE;
                                    pMatchIndices[k++] = matchIndex;
                                    weight = UGL_TRUE;
                                    deltaPrev = deltaNew;
                                }
                                else {
                                    numValid = 0;
                                    for (j = 0, k = 0;
                                         j < numFonts && pMatchIndices[j] != -1;
                                         j++) {

                                        pListArray[
                                            pMatchIndices[j]
                                            ].discardKeep = UGL_TRUE;
                                        pMatchIndices[j] = -1;
                                    }

                                    numValid++;
                                    matchIndex = i;
                                    pListArray[matchIndex].discardKeep =
                                        UGL_FALSE;
                                    pMatchIndices[k++] = matchIndex;
                                    pixelSize = UGL_TRUE;
                                    deltaPrev = deltaNew;
                                }
                            }
                            else {
                                pListArray[i].discardKeep = UGL_TRUE;
                            }
                        }
                        else {
                            pListArray[i].discardKeep = UGL_TRUE;
                        }
                    }
                }
            }

            UGL_FREE(pMatchIndices);
            fontDescPriority.weight = UGL_FONT_DONT_CARE;
        }
        else if (priority == UGL_ITALIC_PRIORITY) {
            for (i = 0, numValid = 0; i < numFonts; i++) {
                if (pListArray[i].discard == UGL_FALSE) {
                    if (pListArray[i].fontDesc.italic ==
                        pFontDescriptor->italic) {

                        numValid++;
                        matchIndex = i;
                    }
                    else {
                        pListArray[i].discardKeep = UGL_TRUE;
                    }
                }
            }

            fontDescPriority.italic = UGL_FONT_DONT_CARE;
        }
        else if (priority == UGL_SPACING_PRIORITY) {
            for (i = 0, numValid = 0; i < numFonts; i++) {
                if (pListArray[i].discard == UGL_FALSE) {
                    if (pListArray[i].fontDesc.spacing ==
                        pFontDescriptor->spacing) {

                        numValid++;
                        matchIndex = i;
                    }
                    else {
                        pListArray[i].discardKeep = UGL_TRUE;
                    }
                }
            }

            fontDescPriority.spacing = UGL_FONT_DONT_CARE;
        }
        else if (priority == UGL_CHAR_SET_PRIORITY) {
            for (i = 0, numValid = 0; i < numFonts; i++) {
                if (pListArray[i].discard == UGL_FALSE) {
                    if (pListArray[i].fontDesc.charSet ==
                        pFontDescriptor->charSet) {

                        numValid++;
                        matchIndex = i;
                    }
                    else {
                        pListArray[i].discardKeep = UGL_TRUE;
                    }
                }
            }

            fontDescPriority.charSet = UGL_FONT_DONT_CARE;
        }
        else {
            for (i = 0; i < numFonts; i++) {
                if (pListArray[i].discard == UGL_FALSE) {
                    break;
                }
            }

            uglFontDefInit(
                pFontDefinition,
                pFontDescriptor,
                pListArray,
                pixelSize,
                weight,
                i,
                &fontDescPriority
                );

            UGL_FREE(pListArray);
            return (UGL_STATUS_OK);
        }
    }

    return (UGL_STATUS_ERROR);
}

/******************************************************************************
 *
 * uglFontFindString - Find font based on format string
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  uglFontFindString (
    UGL_FONT_DRIVER_ID  fontDriverId,
    const char *        pFontPriorityString,
    UGL_FONT_DEF *      pFontDefinition
    ) {
    UGL_STATUS              status;
    UGL_FONT_DESC_PRIORITY  fontDescPriority;
    UGL_FONT_DESC           fontDescriptor;
    char *                  pString;
    char *                  pStringStart;
    char *                  pEnd;
    UGL_SIZE                numChars;
    UGL_SIZE                size;
    UGL_ORD                 priority = 100;
    char *                  pLast    = UGL_NULL;

    fontDescPriority.pixelSize  = UGL_FONT_DONT_CARE;
    fontDescPriority.weight     = 1;
    fontDescPriority.italic     = 1;
    fontDescPriority.spacing    = UGL_FONT_DONT_CARE;
    fontDescPriority.charSet    = UGL_FONT_DONT_CARE;
    fontDescPriority.faceName   = UGL_FONT_DONT_CARE;
    fontDescPriority.familyName = UGL_FONT_DONT_CARE;

    fontDescriptor.weight.min = UGL_FONT_BOLD_OFF;
    fontDescriptor.weight.max = UGL_FONT_BOLD_OFF;
    fontDescriptor.italic     = UGL_FONT_UPRIGHT;

    numChars = strlen(pFontPriorityString);
    if (numChars == 0) {
        status = uglFontFind(
            fontDriverId,
            &fontDescriptor,
            &fontDescPriority,
            pFontDefinition
            );
        return (status);
    }

    /* Process priority string */
    pString = (char *) UGL_CALLOC(1, (numChars + 1) * sizeof(char));
    if (pString == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    pStringStart = pString;
    strncpy(pString, pFontPriorityString, numChars + 1);
    pString[numChars] = '\0';
    pString = strtok_r(pString, ";", &pLast);

    /* Parse string tokens */
    while (pString != UGL_NULL && priority > UGL_FONT_DONT_CARE) {
        while (isspace((int) (*pString))) {
            pString++;
        }

        if (strncmp(pString, "pixelSize", strlen("pixelSize")) == 0) {
            pString = strpbrk(pString, "=");
            pString++;
            if (pString != UGL_NULL) {
                size = strtol(pString, &pEnd, 10);
                if (size > 0) {
                    fontDescriptor.pixelSize.min = size;
                    fontDescriptor.pixelSize.max = size;

                    fontDescPriority.pixelSize = priority--;
                }
            }
        }
        else if (strncmp(pString, "faceName", strlen("faceName")) == 0) {
            pString = strpbrk(pString, "=");
            pString++;
            if (pString != UGL_NULL) {
                while (isspace((int) (*pString))) {
                    pString++;
                }

                if (*pString != '\0') {
                    pEnd = strncpy(
                        fontDescriptor.faceName,
                        pString,
                        UGL_FONT_FACE_NAME_MAX_LENGTH
                        );
                    fontDescriptor.faceName[UGL_FONT_FACE_NAME_MAX_LENGTH - 1] =
                        '\0';

                    if (pEnd != UGL_NULL) {
                        fontDescPriority.faceName = priority--;
                    }
                }
            }
        }
        else if (strncmp(pString, "familyName", strlen("familyName")) == 0) {
            pString = strpbrk(pString, "=");
            pString++;
            if (pString != UGL_NULL) {
                while (isspace((int) (*pString))) {
                    pString++;
                }

                if (*pString != '\0') {
                    pEnd = strncpy(
                        fontDescriptor.familyName,
                        pString,
                        UGL_FONT_FAMILY_NAME_MAX_LENGTH
                        );
                    fontDescriptor.familyName[
                        UGL_FONT_FACE_NAME_MAX_LENGTH - 1] = '\0';

                    if (pEnd != UGL_NULL) {
                        fontDescPriority.familyName = priority--;
                    }
                }
            }
        }
        else if (strncmp(pString, "italic", strlen("italic")) == 0) {
            fontDescriptor.italic = UGL_FONT_ITALIC;
            fontDescPriority.italic = priority--;
        }
        else if (strncmp(pString, "mono", strlen("mono")) == 0) {
            if (fontDescPriority.spacing == UGL_FONT_DONT_CARE) {
                fontDescriptor.spacing = UGL_FONT_MONO_SPACED;
                fontDescPriority.spacing = priority--;
            }
        }
        else if (strncmp(
            pString,
            "proportional",
            strlen("proportional")
            ) == 0) {

            if (fontDescPriority.spacing == UGL_FONT_DONT_CARE) {
                fontDescriptor.spacing = UGL_FONT_PROPORTIONAL;
                fontDescPriority.spacing = priority--;
            }
        }
        else if (strncmp(pString, "bold", strlen("bold")) == 0) {
            fontDescriptor.weight.min = UGL_FONT_BOLD;
            fontDescriptor.weight.max = UGL_FONT_BOLD;
            fontDescPriority.weight = priority--;
        }

        /* Advance */
        pString = strtok_r(NULL, ";", &pLast);
    }

    UGL_FREE(pStringStart);

    status = uglFontFind(
        fontDriverId,
        &fontDescriptor,
        &fontDescPriority,
        pFontDefinition
        );

    return (status);
}

/******************************************************************************
 *
 * uglFontCreate - Create font
 *
 * RETURNS: UGL_FONT_ID or UGL_NULL
 */

UGL_FONT_ID uglFontCreate (
    UGL_FONT_DRIVER_ID  fontDriverId,
    UGL_FONT_DEF *      pFontDefinition
    ) {
    UGL_FONT * pFont;

    if (fontDriverId == UGL_NULL) {
        return (UGL_NULL);
    }

    /* Call driver specific method */
    pFont = (*fontDriverId->fontCreate) (fontDriverId, pFontDefinition);

    return (pFont);
}

/******************************************************************************
 *
 * uglFontDestroy - Destroy font
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglFontDestroy (
    UGL_FONT_ID  fontId
    ) {
    UGL_FONT_DRIVER * pFontDriver;
    UGL_STATUS        status;

    /* Validate */
    if (fontId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Get font driver */
    pFontDriver = fontId->pFontDriver;
    if (pFontDriver == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*pFontDriver->fontDestroy) (fontId);

    return (status);
}

/******************************************************************************
 *
 * uglFontInfo - Get information about font
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglFontInfo (
    UGL_FONT_ID   fontId,
    UGL_INFO_REQ  infoRequest,
    void *        pInfo
    ) {
    UGL_FONT_DRIVER_ID  drvId;
    UGL_STATUS          status;

    /* Validate */
    if (fontId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Get font driver */
    drvId = fontId->pFontDriver;
    if (drvId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*drvId->fontInfo) (fontId, infoRequest, pInfo);

    return (status);
}

/******************************************************************************
 *
 * uglFontMetricsGet - Get metrics information about font
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglFontMetricsGet (
    UGL_FONT_ID        fontId,
    UGL_FONT_METRICS * pFontMetrics
    ) {
    UGL_FONT_DRIVER_ID  drvId;
    UGL_STATUS          status;

    /* Validate */
    if (fontId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Get font driver */
    drvId = fontId->pFontDriver;
    if (drvId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*drvId->fontMetricsGet) (fontId, pFontMetrics);

    return (status);
}

/******************************************************************************
 *
 * uglTextSizeGet - Get text size
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglTextSizeGet (
    UGL_FONT_ID      fontId,
    UGL_SIZE *       pWidth,
    UGL_SIZE *       pHeight,
    UGL_SIZE         length,
    const UGL_CHAR * pText
    ) {
    UGL_FONT_DRIVER_ID  drvId;
    UGL_STATUS          status;

    /* Validate */
    if (fontId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Get font driver */
    drvId = fontId->pFontDriver;
    if (drvId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*drvId->textSizeGet) (fontId, pWidth, pHeight, length, pText);

    return (status);
}

/******************************************************************************
 *
 * uglTextDraw - Draw text
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS uglTextDraw (
    UGL_GC_ID        gc,
    UGL_POS          x,
    UGL_POS          y,
    UGL_SIZE         length,
    const UGL_CHAR * pText
    ) {
    UGL_FONT_DRIVER_ID  drvId;
    UGL_STATUS          status;

    /* Start batch job */
    if ((uglBatchStart (gc)) == UGL_STATUS_ERROR) {
        return (UGL_STATUS_ERROR);
    }

    /* Validate */
    if (gc->pFont == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Get font driver */
    drvId = gc->pFont->pFontDriver;
    if (drvId == UGL_NULL) {
        return (UGL_STATUS_ERROR);
    }

    /* Call driver specific method */
    status = (*drvId->textDraw) (gc, x, y, length, pText);

    /* End batch job */
    uglBatchEnd (gc);

    return (status);
}

/******************************************************************************
 *
 * uglFontDefInit - Initialize font definition
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  uglFontDefInit (
    UGL_FONT_DEF *             pFontDefinition,
    UGL_FONT_DESC *            pFontDescriptor,
    UGL_FONT_DESC_LIST_ELMT *  pListArray,
    UGL_BOOL                   pixelSize,
    UGL_BOOL                   weight,
    UGL_ORD                    index,
    UGL_FONT_DESC_PRIORITY *   pFontDescPriority
    ) {
    UGL_ORD  avg;

    pFontDefinition->charSet =
        pListArray[index].fontDesc.charSet;
    pFontDefinition->italic =
        pListArray[index].fontDesc.italic;
    pFontDefinition->structSize = sizeof(UGL_FONT_DEF);
    if (pixelSize == UGL_FALSE) {
        if (pFontDescPriority->pixelSize > UGL_FONT_DONT_CARE) {
           avg = (pFontDescriptor->pixelSize.min +
                  pFontDescriptor->pixelSize.max) / 2;

           if (avg < pListArray[index].fontDesc.pixelSize.min) {
               avg = pListArray[index].fontDesc.pixelSize.min;
           }

           if (avg > pListArray[index].fontDesc.pixelSize.max) {
               avg = pListArray[index].fontDesc.pixelSize.max;
           }

           pFontDefinition->pixelSize = avg;
        }
        else {
            pFontDefinition->pixelSize =
                (pListArray[index].fontDesc.pixelSize.min +
                 pListArray[index].fontDesc.pixelSize.max) / 2;
        }
    }
    else {
       avg = (pFontDescriptor->pixelSize.min +
              pFontDescriptor->pixelSize.max) / 2;

       if (avg < pListArray[index].fontDesc.pixelSize.min) {
           avg = pListArray[index].fontDesc.pixelSize.min;
       }

       if (avg > pListArray[index].fontDesc.pixelSize.max) {
           avg = pListArray[index].fontDesc.pixelSize.max;
       }

       pFontDefinition->pixelSize = avg;
    }

    if (weight == UGL_FALSE) {
        if (pFontDescPriority->weight > UGL_FONT_DONT_CARE) {
            avg = (pFontDescriptor->weight.min +
                   pFontDescriptor->weight.max) / 2;

            if (avg < pListArray[index].fontDesc.weight.min) {
                avg = pListArray[index].fontDesc.weight.min;
            }

            if (avg < pListArray[index].fontDesc.weight.max) {
                avg = pListArray[index].fontDesc.weight.max;
            }

            pFontDefinition->weight = avg;
        }
        else {
            pFontDefinition->weight =
                (pListArray[index].fontDesc.weight.min +
                 pListArray[index].fontDesc.weight.max) / 2;
        }
    }
    else {
        avg = (pFontDescriptor->weight.min +
               pFontDescriptor->weight.max) / 2;

        if (avg < pListArray[index].fontDesc.weight.min) {
            avg = pListArray[index].fontDesc.weight.min;
        }

        if (avg < pListArray[index].fontDesc.weight.max) {
            avg = pListArray[index].fontDesc.weight.max;
        }

        pFontDefinition->weight = avg;
    }

    strncpy(
        pFontDefinition->faceName,
        pListArray[index].fontDesc.faceName,
        UGL_FONT_FACE_NAME_MAX_LENGTH
        );
    pFontDefinition->faceName[UGL_FONT_FACE_NAME_MAX_LENGTH - 1] = '\0';

    strncpy(
        pFontDefinition->familyName,
        pListArray[index].fontDesc.familyName,
        UGL_FONT_FAMILY_NAME_MAX_LENGTH
        );
    pFontDefinition->familyName[UGL_FONT_FAMILY_NAME_MAX_LENGTH - 1] = '\0';
}

/******************************************************************************
 *
 * uglFontFindHighestPriority - Find highest priority for font
 *
 * RETURNS: N/A
 */

UGL_LOCAL UGL_VOID  uglFontFindHighestPriority (
    UGL_PRIORITY_TYPE *       priority,
    UGL_FONT_DESC_PRIORITY *  pFontDescPriority
    ) {
    UGL_ORD  maxPriority = UGL_FONT_DONT_CARE;

    *priority = UGL_NO_PRIORITY;

    if (pFontDescPriority->faceName > maxPriority) {
        *priority = UGL_FACE_NAME_PRIORITY;
        maxPriority = pFontDescPriority->faceName;
    }

    if (pFontDescPriority->familyName > maxPriority) {
        *priority = UGL_FAMILY_NAME_PRIORITY;
        maxPriority = pFontDescPriority->familyName;
    }

    if (pFontDescPriority->pixelSize > maxPriority) {
        *priority = UGL_PIXEL_SIZE_PRIORITY;
        maxPriority = pFontDescPriority->pixelSize;
    }

    if (pFontDescPriority->weight > maxPriority) {
        *priority = UGL_WEIGHT_PRIORITY;
        maxPriority = pFontDescPriority->weight;
    }

    if (pFontDescPriority->italic > maxPriority) {
        *priority = UGL_ITALIC_PRIORITY;
        maxPriority = pFontDescPriority->italic;
    }

    if (pFontDescPriority->charSet > maxPriority) {
        *priority = UGL_CHAR_SET_PRIORITY;
        maxPriority = pFontDescPriority->charSet;
    }
}

