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

/* Window manager cursor */

/* Includes */

#include "ugl.h"

/* Locals */

/* Cursor arrow (11x19) */
UGL_LOCAL const UGL_UINT8 cursorArrow[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define O 1,
    B T T T T T T T T T T
    B B T T T T T T T T T
    B O B T T T T T T T T
    B O O B T T T T T T T
    B O O O B T T T T T T
    B O O O O B T T T T T
    B O O O O O B T T T T
    B O O O O O O B T T T
    B O O O O O O O B T T
    B O O O O O O O O B T
    B O O O O O B B B B B
    B O O B O O B T T T T
    B O B T B O O B T T T
    B B T T B O O B T T T
    B T T T T B O O B T T
    T T T T T B O O B T T
    T T T T T T B O O B T
    T T T T T T B O O B T
    T T T T T T T B B T T
#undef T
#undef B
#undef O
};

/* Cursor edit (5x15) */
UGL_LOCAL const UGL_UINT8 cursorEdit[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define I UGL_CURSOR_COLOR_INVERT,
    I I T I I
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    T T I T T
    I I T I I
#undef T
#undef I
};

/* Cursor wait (15x26) */
UGL_LOCAL const UGL_UINT8 cursorWait[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define O 1,
    B B B B B B B B B B B B B B B
    B B B B B B B B B B B B B B B
    B B O O O O O O O O O O O B B
    T B B B B B B B B B B B B B T
    T B B O O O O O O O O O B B T
    T B B O O O O O O O O O B B T
    T B B O O O O O O B O O B B T
    T B B O B O B O B O B O B B T
    T B B O O B O B O B O O B B T
    T T B B O O B O B O O B B T T
    T T T B B O O B O O B B T T T
    T T T T B B O O O B B T T T T
    T T T T T B B O B B T T T T T
    T T T T T B B O B B T T T T T
    T T T T T B B O B B T T T T T
    T T T T B B O O O B B T T T T
    T T T B B O O O O O B B T T T
    T T B B O O O B O O O B B T T
    T B B O O O O O O O O O B B T
    T B B O O O O B O O O O B B T
    T B B O O O B O B O O O B B T
    T B B O O B O B O B O O B B T
    T B B O B O B O B O B O B B T
    B B B B B B B B B B B B B B B
    B B O O O O O O O O O O O B B
    B B B B B B B B B B B B B B B
#undef T
#undef B
#undef O
};

/* Cursor invalid (20x20) */
UGL_LOCAL const UGL_UINT8 cursorInvalid[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define O 1,
    T T T T T T T O O O O O O T T T T T T T
    T T T T T O O B B B B B B O O T T T T T
    T T T T O B B B B B B B B B B O T T T T
    T T T O B B B B O O O O B B B B O T T T
    T T O B B B O O T T T T O O B B B O T T
    T O B B B B B O T T T T T T O B B B O T
    T O B B O B B B O T T T T T T O B B O T
    O B B B O O B B B O T T T T T O B B B O
    O B B O T T O B B B O T T T T T O B B O
    O B B O T T T O B B B O T T T T O B B O
    O B B O T T T T O B B B O T T T O B B O
    O B B O T T T T T O B B B O T T O B B O
    O B B B O T T T T T O B B B O O B B B O
    T O B B O T T T T T T O B B B O B B O T
    T O B B B O T T T T T T O B B B B B O T
    T T O B B B O O T T T T O O B B B O T T
    T T T O B B B B O O O O B B B B O T T T
    T T T T O B B B B B B B B B B O T T T T
    T T T T T O O B B B B B B O O T T T T T
    T T T T T T T O O O O O O T T T T T T T
#undef T
#undef B
#undef O
};

/* Cursor move (19x19) */
UGL_LOCAL const UGL_UINT8 cursorMove[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define O 1,
    T T T T T T T T T B T T T T T T T T T
    T T T T T T T T B O B T T T T T T T T
    T T T T T T T B O O O B T T T T T T T
    T T T T T T B O O O O O B T T T T T T
    T T T T T B B B B O B B B B T T T T T
    T T T T B T T T B O B T T T B T T T T
    T T T B B T T T B O B T T T B B T T T
    T T B O B T T T B O B T T T B O B T T
    T B O O B B B B B O B B B B B O O B T
    B O O O O O O O O O O O O O O O O O B
    T B O O B B B B B O B B B B B O O B T
    T T B O B T T T B O B T T T B O B T T
    T T T B B T T T B O B T T T B B T T T
    T T T T B T T T B O B T T T B T T T T
    T T T T T B B B B O B B B B T T T T T
    T T T T T T B O O O O O B T T T T T T
    T T T T T T T B O O O B T T T T T T T
    T T T T T T T T B O B T T T T T T T T
    T T T T T T T T T B T T T T T T T T T
#undef T
#undef B
#undef O
};

/* Cursor horizontal size (16x12) */
UGL_LOCAL const UGL_UINT8 cursorSizeHoriz[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define O 1,
    T T T T T B T T T T B T T T T T
    T T T T B B T T T T B B T T T T
    T T T B O B T T T T B O B T T T
    T T B O O B T T T T B O O B T T
    T B O O O B B B B B B O O O B T
    B O O O O O O O O O O O O O O B
    B O O O O O O O O O O O O O O B
    T B O O O B B B B B B O O O B T
    T T B O O B T T T T B O O B T T
    T T T B O B T T T T B O B T T T
    T T T T B B T T T T B B T T T T
    T T T T T B T T T T B T T T T T
#undef T
#undef B
#undef O
};

/* Cursor vertical size (12x16) */
UGL_LOCAL const UGL_UINT8 cursorSizeVert[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define O 1,
    T T T T T B B T T T T T
    T T T T B O O B T T T T
    T T T B O O O O B T T T
    T T B O O O O O O B T T
    T B O O O O O O O O B T
    B B B B B O O B B B B B
    T T T T B O O B T T T T
    T T T T B O O B T T T T
    T T T T B O O B T T T T
    T T T T B O O B T T T T
    B B B B B O O B B B B B
    T B O O O O O O O O B T
    T T B O O O O O O B T T
    T T T B O O O O B T T T
    T T T T B O O B T T T T
    T T T T T B B T T T T T
#undef T
#undef B
#undef O
};

/* Cursor diagonal size top-left or bottom-right (12x12) */
UGL_LOCAL const UGL_UINT8 cursorSizeDiagTlBr[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define O 1,
    B B B B B B B B T T T T
    B O O O O O B T T T T T
    B O O O O B T T T T T T
    B O O O O B T T T T T T
    B O O O O O B T T T T B
    B O B B O O O B T T B B
    B B T T B O O O B B O B
    B T T T T B O O O O O B
    T T T T T T B O O O O B
    T T T T T T B O O O O B
    T T T T T B O O O O O B
    T T T T B B B B B B B B
#undef T
#undef B
#undef O
};

/* Cursor diagonal size top-right or bottom-left (12x12) */
UGL_LOCAL const UGL_UINT8 cursorSizeDiagTrBl[] = {
#define T UGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define O 1,
    T T T T B B B B B B B B
    T T T T T B O O O O O B
    T T T T T T B O O O O B
    T T T T T T B O O O O B
    B T T T T B O O O O O B
    B B T T B O O O B B O B
    B O B B O O O B T T B B
    B O O O O O B T T T T B
    B O O O O B T T T T T T
    B O O O O B T T T T T T
    B O O O O O B T T T T T
    B B B B B B B B T T T T
#undef T
#undef B
#undef O
};

/* Cursor vertical size (12x16) */
UGL_LOCAL const UGL_ARGB cursorClut[] = {
    UGL_MAKE_RGB(  0,   0,   0),
    UGL_MAKE_RGB(255, 255, 255)
};

/* Globals */

const UGL_CDIB wwmCDibArrow = {
    11,                                       /* width */
    19,                                       /* height */
    11,                                       /* stride */
    {
        0,                                    /* hotSpot.x */
        0                                     /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorArrow                 /* pData */
};

const UGL_CDIB wwmCDibEdit = {
     5,                                       /* width */
    15,                                       /* height */
     5,                                       /* stride */
    {
        2,                                    /* hotSpot.x */
        6                                     /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorEdit                  /* pData */
};

const UGL_CDIB wwmCDibWait = {
    15,                                       /* width */
    26,                                       /* height */
    15,                                       /* stride */
    {
         7,                                   /* hotSpot.x */
        13                                    /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorWait                  /* pData */
};

const UGL_CDIB wwmCDibInvalid = {
    20,                                       /* width */
    20,                                       /* height */
    20,                                       /* stride */
    {
        10,                                   /* hotSpot.x */
        10                                    /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorInvalid               /* pData */
};

const UGL_CDIB wwmCDibMove = {
    19,                                       /* width */
    19,                                       /* height */
    19,                                       /* stride */
    {
        9,                                    /* hotSpot.x */
        9                                     /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorMove                  /* pData */
};

const UGL_CDIB wwmCDibSizeHoriz = {
    16,                                       /* width */
    12,                                       /* height */
    16,                                       /* stride */
    {
        7,                                    /* hotSpot.x */
        5                                     /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorSizeHoriz             /* pData */
};

const UGL_CDIB wwmCDibSizeVert = {
    12,                                       /* width */
    16,                                       /* height */
    12,                                       /* stride */
    {
        5,                                    /* hotSpot.x */
        7                                     /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorSizeVert              /* pData */
};

const UGL_CDIB wwmCDibSizeDiagTlBr = {
    12,                                       /* width */
    12,                                       /* height */
    12,                                       /* stride */
    {
        5,                                    /* hotSpot.x */
        5                                     /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorSizeDiagTlBr          /* pData */
};

const UGL_CDIB wwmCDibSizeDiagTrBl = {
    12,                                       /* width */
    12,                                       /* height */
    12,                                       /* stride */
    {
        5,                                    /* hotSpot.x */
        5                                     /* hotSpot.y */
    },
    sizeof(cursorClut) / sizeof(UGL_ARGB),    /* clutSize */
    (UGL_ARGB *) cursorClut,                  /* pClut */
    (UGL_UINT8 *) cursorSizeDiagTrBl          /* pData */
};

