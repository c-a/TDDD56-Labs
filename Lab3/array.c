/*
 * array.c
 *
 *  Created on: 5 Sep 2011
 *  Copyright 2011 Nicolas Melot
 *
 * This file is part of pelib.
 * 
 *     pelib is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     pelib is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with pelib. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "disable.h"
#include "array.h"

static sem_t sem_printf;

void
array_init(int argc, char ** argv)
{
  sem_init(&sem_printf, 0, 1);
}

static int
read_value(FILE * file, value * buffer)
{
  assert(file != NULL);
  int num, total;
  char read, has_more;

  read = 0;
  has_more = 1;
  num = 0;
  total = 0;

  while (read != ' ')
    {
      if ((has_more = fread(&read, 1, 1, file)) == 0 || read == ' ')
        {
          break;
        }

      total += has_more;
      num = num * 10 + read - '0';
    }

  *buffer = num;
  return total;
}

static struct array*
array_prealloc()
  {
    struct array* array;

    array = malloc(sizeof(struct array));
    assert(array != NULL);

    array->length = 0;
    array->capacity = 0;

    return array;
  }

struct array*
array_alloc(int size)
  {
    struct array* array;

    array = array_prealloc();
    array->capacity = size;

    if(array->capacity > 0)
    {
      array->data = calloc(array->capacity, sizeof(int));
      assert((array->data != NULL && size > 0) || (array->data == NULL && size <= 0));
    }

    return array;
  }

struct array*
array_read(char * filename)
{
  int i;
  FILE * h;
  int num;
  value value;
  struct array * res;

  h = fopen(filename, "r");
  i = read_value(h, &num);
  assert(i > 0);
  
  res = array_alloc(num);

  num = 1;
  for (i = 0; i < res->capacity && num > 0; i++)
    {
      num = read_value(h, &value);
      array_put(res, value);
    }

  fclose(h);

  return res;
}

#if 0
static int
min(int a, int b)
{
  if (a < b)
    {
      return a;
    }
  else
    {
      return b;
    }
}
#endif

void
array_printf(struct array* a)
{
  int i;

  sem_wait(&sem_printf);
  for (i = 0; i < a->length; i++)
    {
      printf("%d ", a->data[i]);
    }
  printf("\n");
  sem_post(&sem_printf);
}

int
array_write(struct array* a, char* filename)
{
  int i;
  FILE * h;

  h = fopen(filename, "w");

  fprintf(h, "%i ", a->length);
  for (i = 0; i < a->length; i++)
    {
      fprintf(h, "%i ", a->data[i]);
    }

  return -1;
}

#if 0
int
array_check_ascending(struct array* a)
{
  int i;

  for (i = 0; i < a->length - 1; i++)
    {
      if (a->data[i] > a->data[i + 1])
        {
          array_printf(a);
          printf("[ERROR] Inconsistency at index %i: %i followed by %i\n", i,
              a->data[i], a->data[i + 1]);
          assert(a->data[i] <= a->data[i + 1]);
          return -1;
        }
    }

  return a->length;
}
#endif

int
array_trycheck_ascending(struct array* a)
{
  int i;

  for (i = 0; i < a->length - 1; i++)
    {
      if (a->data[i] > a->data[i + 1])
        {
          //array_printf(a);
          printf("[ERROR] Inconsistency at index %i: %i followed by %i\n", i,
              a->data[i], a->data[i + 1]);
          return 1;
        }
    }

  return 0;
}

static int
array_prefree(struct array* array)
{
  free(array);

  return 0;
}

void
array_free(struct array* array)
{
  free(array->data);
  array_prefree(array);
}

int
array_get(struct array *array, int pos)
{
  if (array->capacity > pos)
    {
      return array->data[pos];
    }
  else
    {
      return -1;
    }
}

int
array_insert(struct array* array, int val, int pos)
{
  if (array->capacity > pos)
    {
      array->data[pos] = val;
      return array->length;
    }
  else
    {
      return 0;
    }
}

int
array_put(struct array* array, int val)
{
  if (array->length < array->capacity)
    {
      array->data[array->length] = val;
      array->length++;

      return array->length;
    }
  else
    {
      return 0;
    }
}

