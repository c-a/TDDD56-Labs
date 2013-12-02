/*
 * sort.c
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

// Do not touch or move these lines
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include "disable.h"

#ifndef DEBUG
#define NDEBUG
#endif

#include "array.h"
#include "sort.h"
#include "simple_quicksort.h"
#include "quicksort.h"

#if VARIANT == 2
int parallell_mergesort(struct array* array);

#elif VARIANT == 1
#if NB_THREADS > 1

#define MIN(x, y) ((x) < (y) ? (x) : (y))

typedef struct
{
  int id;
  int* data;
  int from, to;

  struct partition {
    int* data;
    int length;
  } partitions[NB_THREADS];
} thread_data_t;

static int pivots[NB_THREADS];
static thread_data_t thread_data[NB_THREADS];

pthread_barrier_t thread_barrier;

static void*
thread_func(void* user_data)
{
  thread_data_t* td = user_data;
  struct partition* p = td->partitions;

  int i, from, to;
  int* data;

  /* Partition our segment */
  from = td->from;
  to = td->to;
  data = td->data;
  for (i = from; i <= to; i++) {
    int val, j;
    val = data[i];
    for (j = 0; j < NB_THREADS; j++) {
      if (val <= pivots[j]) {
        p[j].data[p[j].length++] = val;
        break;
      }
    }
  }

  /* Wait for other threads to finish partitioning */
  pthread_barrier_wait(&thread_barrier);

  /* Calculate where in the array the partition we should sort should be put */
  from = 0;
  for (i = 0; i < NB_THREADS; i++) {
    thread_data_t *tdi = &thread_data[i];
    int j;
    for (j = 0; j < td->id; j++) {
      from += tdi->partitions[j].length;
    }
  }

  /* Copy data from per thread partitions into array */
  to = from - 1;
  for (i = 0; i < NB_THREADS; i++) {
    int length, j;
    int* a;

    a = thread_data[i].partitions[td->id].data;
    length = thread_data[i].partitions[td->id].length;

    for (j = 0; j < length; j++)
      data[++to] = a[j];
  }

  /* Sort our part of the array */
  quicksort(data, from, to);

  return NULL;
}

static int
parallel_samplesort(struct array* array)
{
  int i;
  int n_samples, data_step_size, sample_step_size;
  int* samples;
  int partition_size;

  pthread_attr_t thread_attr;
  pthread_t thread[NB_THREADS];

  /* Just do a single quicksort if array size is small */
  if (array->length <= 10000) {
    quicksort(array->data, 0, array->length - 1);
    return 0;
  }

  /* Find pivots to partition around */
  n_samples = sqrt(array->length);
  data_step_size = array->length / n_samples;
  samples = malloc(n_samples * sizeof(int));
  for (i = 0; i < n_samples; i++)
    samples[i] = array->data[i*data_step_size];

  quicksort(samples, 0, n_samples - 1);
  sample_step_size = n_samples / (NB_THREADS - 1);
  for (i = 0; i < NB_THREADS - 1; i++)
    pivots[i] = samples[sample_step_size / 2 + i*sample_step_size];
  pivots[NB_THREADS - 1] = INT_MAX;


  /* Initialise thread synchronisation */
  pthread_barrier_init(&thread_barrier, NULL, NB_THREADS);

  /* Initialize thread attributes */
  pthread_attr_init(&thread_attr);
  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

  /* Start the worker threads */
  partition_size = (array->length + NB_THREADS - 1) / NB_THREADS;
  for (i = 0; i < NB_THREADS; i++) {
    thread_data_t* td = &thread_data[i];
    int j;

    td->id = i;
    td->data = array->data;
    td->from = i*partition_size;
    td->to = MIN(td->from + partition_size, array->length) - 1;

    for (j = 0; j < NB_THREADS; j++) {
      td->partitions[j].data = malloc(partition_size*sizeof(int));
      assert(td->partitions[j].data != NULL);
      td->partitions[j].length = 0;
    }

    pthread_create(&thread[i], &thread_attr, &thread_func, td);
  }

  /* Wait for the threads to finish */
  for (i = 0; i < NB_THREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  /* Free/destroy data */
#if 0
  pthread_barrier_destroy(&thread_barrier);

  for (i = 0; i < NB_THREDS; j++) {
    thread_data_t* td = &thread_data[i];
    int j;

    for (j = 0; j < NB_THREADS; j++) {
      free(td->partitions[j].data);
    }
  }
#endif

  return 0;
}
#else
static int parallel_samplesort(struct array* array)
{
  quicksort(array->data, 0, array->length - 1);
  return 0;
}
#endif
#endif

int
sort(struct array * array)
{
#if VARIANT == 0
  quicksort(array->data, 0, array->length - 1);
#elif VARIANT == 1
  parallel_samplesort(array);
#else
  parallell_mergesort(array);
#endif

  return 0;
}
