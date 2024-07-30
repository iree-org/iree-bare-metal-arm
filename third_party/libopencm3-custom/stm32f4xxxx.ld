/*
 * This file is intended to be used with the libopencm3 project.
 *
 * Copyright 2021 The IREE bare-metal Arm Authors
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

/* Linker script for a custom STM32F4 (256MB flash, 256MB RAM). */

MEMORY
{
	rom (rx) : ORIGIN = 0xC0000000, LENGTH = 256M
	ram (rwx) : ORIGIN = 0xD0000000, LENGTH = 256M
}

INCLUDE cortex-m-generic.ld
