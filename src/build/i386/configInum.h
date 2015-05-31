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

/* configInum.h - */

#ifndef _configInum_h
#define _configInum_h

/* Defines */

#define INT_NUM_IOAPIC_IRQ0     0xcb
#define INT_NUM_IOAPIC_IRQ1     0xc4
#define INT_NUM_IOAPIC_IRQ2     0xac
#define INT_NUM_IOAPIC_IRQ3     0xa4
#define INT_NUM_IOAPIC_IRQ4     0x9c
#define INT_NUM_IOAPIC_IRQ5     0x94
#define INT_NUM_IOAPIC_IRQ6     0x8c
#define INT_NUM_IOAPIC_IRQ7     0x84
#define INT_NUM_IOAPIC_IRQ8     0x7c
#define INT_NUM_IOAPIC_IRQ9     0x74
#define INT_NUM_IOAPIC_IRQA     0x6c
#define INT_NUM_IOAPIC_IRQB     0x64
#define INT_NUM_IOAPIC_IRQC     0x5c
#define INT_NUM_IOAPIC_IRQD     0x54
#define INT_NUM_IOAPIC_IRQE     0x4c
#define INT_NUM_IOAPIC_IRQF     0x44

#define INT_NUM_COM1            (INT_NUM_IOAPIC_IRQ4)
#define INT_NUM_COM2            (INT_NUM_IOAPIC_IRQ3)
#define INT_NUM_COM3            (INT_NUM_IOAPIC_IRQ5)
#define INT_NUM_COM4            (INT_NUM_IOAPIC_IRQ9)

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _configInum_h */

