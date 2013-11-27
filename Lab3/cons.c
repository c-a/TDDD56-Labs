/*
 * cons.c
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
#include <stdio.h>

int
main(int argc, char ** argv)
{
	int i, min, count;

	if(argc < 4)
	{
		printf("Usage: %s <min_value> <max_value> <number of values>\n");
		return EXIT_FAILURE;
	}
	else
	{
		min = atoi(argv[1]);
		count = atoi(argv[3]);

		printf("%d ", count);
		for(i = 0; i < count; i++)
		{
			printf("%d ", min);
		}
	}
	
	return 0;
}
