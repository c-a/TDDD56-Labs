/*
 * dec.c
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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "array.h"

inline volatile long long
rdtsc()
{
	register long long tsc asm("eax");
	asm volatile (".byte 15, 49" : : : "eax", "edx");

	return tsc;
}

static int
descending(const void* a, const void* b)
  {
    int aa = *(value*)a;
    int bb = *(value*)b;

    return aa < bb;
  }

int
sort(struct array * array)
  {
    qsort(array->data, array->length, sizeof(value), descending);

    return 0;
  }

int
main(int argc, char ** argv)
{
	int i, max, min, count, amplitude;
	struct array * array;

	srandom(rdtsc() + time(NULL));

	if(argc < 4)
	{
		printf("Usage: %s <min_value> <max_value> <number of values>\n");
		return EXIT_FAILURE;
	}
	else
	{
		array_init(argc, argv);

		min = atoi(argv[1]);
		max = atoi(argv[2]);
		count = atoi(argv[3]);

		array = array_alloc(count);		

		amplitude = max - min + 1;

		for(i = 0; i < count; i++)
		{
			array_put(array, random() % amplitude + min);
		}
		sort(array);

		printf("%d ", count);
		for(i = 0; i < count; i++)
		{
			printf("%d ", array_get(array, i));
		}
		
		array_free(array);
	}
	
	return 0;
}
