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

/* isalpha.c - Test if character is an alaphabetical letter */

/* Includes */
#include <ctype.h>
#include <vmx.h>

/* Defines */

/* Imports */

/* Locals */

/* Globals */

/* Functions */

/******************************************************************************
 * isalnum - Test if character is an alphabetical letter
 *
 * RETURNS: Non-zero if alphabetical
 */

int isalpha(
    int c
    )
{
    return __isalpha(c);
}
