/*
 * sort_main.c
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
#include <string.h>
#include <assert.h>
#if MEASURE == 1
#include <time.h>
#endif

#include "sort.h"

#define SORTED ".sorted"
#define REFERENCE ".reference"

static int
ascending(const void* a, const void* b)
{
	int aa = *(value*) a;
	int bb = *(value*) b;

	return aa > bb;
}

int
main(int argc, char** argv)
{
	struct array *array, *check;
	char * output, *ref;
	int i;

#if MEASURE == 1
	struct timespec start, stop;
#endif

	output = NULL;
	output = malloc(sizeof(char) * strlen(argv[1]) + strlen(SORTED) + 1);
	ref = malloc(sizeof(char) * strlen(argv[1]) + strlen(REFERENCE) + 1);
	assert(output != NULL);
	output[0] = '\0';
	ref[0] = '\0';
	sprintf(output, "%s%s", argv[1], SORTED);
	sprintf(ref, "%s%s", argv[1], REFERENCE);

	array_init(argc, argv);

	array = array_read(argv[1]);

	// Keep a copy in memory for later verification
	check = array_alloc(array->capacity);
	memcpy(check->data, array->data, sizeof(value) * array->length);
	check->length = array->length;

#if MEASURE == 1
	clock_gettime(CLOCK_MONOTONIC, &start);
#endif
	sort(array);
#if MEASURE == 1
	clock_gettime(CLOCK_MONOTONIC, &stop);
#endif

	qsort(check->data, check->length, sizeof(value), ascending);

	for (i = 0; i < array->length; i++)
	{
		if (array_get(array, i) != array_get(check, i))
		{
			fprintf(stderr, "[ERROR] Value at index %i (%i) was different than in qsort'ed array at same index (%i)\n", i, array_get(array, i), array_get(check, i));
			array_write(array, output);
			array_write(check, ref);
			return EXIT_FAILURE;
		}
	}

	array_free(array);
	free(output);

#if MEASURE == 1
#undef printf
	printf("%li %li %li %li\n", start.tv_sec, start.tv_nsec, stop.tv_sec, stop.tv_nsec);
#endif

	return EXIT_SUCCESS;
}
