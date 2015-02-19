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

/* i8250Sio.c - PC compatibles serial driver */

#include <vmx.h>
#include <drv/sio/i8250Sio.h>

/* Locals */

LOCAL SIO_DRV_FUNCS  i8250SioDrvFuncs;
LOCAL BAUD           baudTable [] = {
    {     50, 2304 },
    {     75, 1536 },
    {    110, 1047 },
    {    134,  857 },
    {    150,  768 },
    {    300,  384 },
    {    600,  192 },
    {   1200,   96 },
    {   2000,   58 },
    {   2400,   48 },
    {   3600,   32 },
    {   4800,   24 },
    {   7200,   16 },
    {   9600,   12 },
    {  19200,    6 },
    {  38400,    3 },
    {  57600,    2 },
    { 115200,    1 }
};

