/*
 * array.h
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

#ifndef DEBUG
#define NDEBUG
#endif

#ifndef ARRAY_H_
#define ARRAY_H_

typedef int value;

struct array
{
  int length;
  int capacity;
  value* data;
};

void array_init(int, char **);

struct array * array_read(char *);
void array_printf(struct array*);
struct array * array_alloc(int);
void array_free(struct array*);
int array_write(struct array*, char*);
int array_check_ascending(struct array*);
int array_trycheck_ascending(struct array*);

int array_put(struct array*, value);
int array_insert(struct array*, value, int);
value array_get(struct array*, int);

#endif /* ARRAY_H_ */
