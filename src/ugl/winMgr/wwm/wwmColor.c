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

/* wwmColor.c - Window manager colormap */

#include "ugl.h"

const UGL_RGB wwmRGBColorTable[16] = {
    /* BLACK        */    UGL_MAKE_ARGB(255,   0,   0,   0),
    /* BLUE         */    UGL_MAKE_ARGB(255,   0,   0, 168),
    /* GREEN        */    UGL_MAKE_ARGB(255,   0, 168,   0),
    /* CYAN         */    UGL_MAKE_ARGB(255,   0, 168, 168),
    /* RED          */    UGL_MAKE_ARGB(255, 168,   0,   0),
    /* MAGENTA      */    UGL_MAKE_ARGB(255, 168,   0, 168),
    /* BROWN        */    UGL_MAKE_ARGB(255, 168,  84,   0),
    /* LIGHTGRAY    */    UGL_MAKE_ARGB(255, 168, 168, 168),
    /* DARKGRAY     */    UGL_MAKE_ARGB(255,  84,  84,  84),
    /* LIGHTBLUE    */    UGL_MAKE_ARGB(255,  84,  84, 255),
    /* LIGHTGREEN   */    UGL_MAKE_ARGB(255,  84, 255,  84),
    /* LIGHTCYAN    */    UGL_MAKE_ARGB(255,  84, 255, 255),
    /* LIGHTRED     */    UGL_MAKE_ARGB(255, 255,  84,  84),
    /* LIGHTMAGENTA */    UGL_MAKE_ARGB(255, 255,  84, 255),
    /* YELLOW       */    UGL_MAKE_ARGB(255, 255, 255,  84),
    /* WHITE        */    UGL_MAKE_ARGB(255, 255, 255, 255)
};

