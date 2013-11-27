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

#include <stdio.h>
#include <stdlib.h>

#if MEASURE == 1
// Disable printf so that no printf interfere with automated test
void
no_printf(char * str, ...)
{
	// Do nothing
}
#endif

void
no_qsort(void* base, size_t nmemb, size_t size, int(*compar)(const void *, const void *))
{
	fprintf(stderr, "[ERROR] qsort is not allowed, doing nothing\n");
	exit(1);
	// Do nothing
}
