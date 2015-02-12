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

/* wwmConfig.h - Window manager configuration */

#ifndef _wwmConfig_h
#define _wwmConfig_h

/* Defines */

#define WWM_FRAME_BORDER_SIZE     4
#define WWM_FRAME_MIN_WIDTH      60
#define WWM_FRAME_MIN_HEIGHT     30

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#define WWM_FRAME_COLOR_ACTIVE           UGL_MAKE_RGB(128,   0,   0)
#define WWM_FRAME_COLOR_INACTIVE         UGL_MAKE_RGB(128, 128, 128)
#define WWM_FRAME_TEXT_COLOR_ACTIVE      UGL_MAKE_RGB(255, 255,  84)
#define WWM_FRAME_TEXT_COLOR_INACTIVE    UGL_MAKE_RGB(255, 255, 255)

#define WWM_SYSTEM_FONT                  "pixelSize = 10; familyName = Times"
#define WWM_SMALL_FONT                   "pixelSize =  6; familyName = Times"
#define WWM_FIXED_FONT                   "pixelSize = 10; familyName = Courier"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _wwmConfig_h */

