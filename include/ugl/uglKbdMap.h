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

/* uglKbdMap.h - Universal graphics library keyboard map */

#ifndef _uglKbdMap_h
#define _uglKbdMap_h

#include "ugltypes.h"

/* Defines */

#define UGL_MAX_KEY_MAP_VALUES    4

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct ugl_kbd_modifier {
    UGL_UINT32    value;
    UGL_UINT32    scancode;
    UGL_BOOL      extended;
    UGL_BOOL      toggle;
} UGL_KBD_MODIFIER;

typedef struct ugl_kbd_filter {
    UGL_UINT32    onMask;
    UGL_UINT32    offMask;
    UGL_ORD       index;
} UGL_KBD_FILTER;

typedef struct ugl_kbd_keymap {
    UGL_UINT32    mapMask;
    UGL_UINT32    extraModifiers;
    UGL_UINT16    keyValue[UGL_MAX_KEY_MAP_VALUES];
} UGL_KBD_KEYMAP;

typedef struct ugl_kbd_map {
    UGL_KBD_MODIFIER    *pModifiers;
    UGL_KBD_FILTER      *pFilters;
    UGL_KBD_KEYMAP      *pKeyMaps;
    UGL_ORD              numKeyMaps;
} UGL_KBD_MAP;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglKbdMap_h */

