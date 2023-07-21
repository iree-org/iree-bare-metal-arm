/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>,
 * Copyright (C) 2012 chrysn <chrysn@fsfe.org>
 * Copyright 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten
 *                Forschung e.V.
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
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>
#include <vector_chipset.c>

static void pre_main();

typedef void (*funcp_t) (void);
extern funcp_t __preinit_array_start, __preinit_array_end;
extern funcp_t __init_array_start, __init_array_end;
extern funcp_t __fini_array_start, __fini_array_end;

extern unsigned _got_loadaddr, _got, _egot;
extern unsigned _got_plt_loadaddr, _got_plt, _egot_plt;
extern unsigned _bss;

void __attribute__ ((weak)) reset_handler(void)
{
	volatile unsigned *src, *dest;
	funcp_t *fp;

	for (src = &_data_loadaddr, dest = &_data;
		dest < &_edata;
		src++, dest++) {
		*dest = *src;
	}

	for (src = &_got_loadaddr, dest = &_got;
		dest < &_egot;
		src++, dest++) {
		*dest = *src;
	}

	for (src = &_got_plt_loadaddr, dest = &_got_plt;
		dest < &_egot_plt;
		src++, dest++) {
		*dest = *src;
	}

	for (dest = &_bss; dest < &_ebss; dest++) {
		*dest = 0;
	}

	/* Ensure 8-byte alignment of stack pointer on interrupts */
	/* Enabled by default on most Cortex-M parts, but not M3 r1 */
	SCB_CCR |= SCB_CCR_STKALIGN;

	/* might be provided by platform specific vector.c */
	pre_main();

	/* Constructors. */
	for (fp = &__preinit_array_start; fp < &__preinit_array_end; fp++) {
		(*fp)();
	}
	for (fp = &__init_array_start; fp < &__init_array_end; fp++) {
		(*fp)();
	}

	/* Call the application's entry point. */
	(void)main();

	/* Destructors. */
	for (fp = &__fini_array_start; fp < &__fini_array_end; fp++) {
		(*fp)();
	}
}
