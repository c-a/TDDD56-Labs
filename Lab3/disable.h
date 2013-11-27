/*
 * rand.c
 *
 *  Created on: 5 Sep 2011
 *  Copyright 2011 Nicolas Melot
 *
 * This file is part of TDDD56.
 * 
 *     TDDD56 is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     TDDD56 is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with TDDD56. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef _DISABLE_H_
#define _DISABLE_H_

#include <sys/types.h>

#if MEASURE == 1
// Disable printf so that no printf interfere with automated test
void
no_printf(char * str, ...);
#define printf no_printf

// Disable qsort
#define qsort no_qsort
void
no_qsort(void*, size_t, size_t, int(*compar)(const void *, const void *));
#endif

#endif
