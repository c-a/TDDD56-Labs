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
#include <unistd.h>
#include "disable.h"

#ifndef DEBUG
#define NDEBUG
#endif

#include "array.h"
#include "sort.h"
#include "simple_quicksort.h"
#include "quicksort.h"

#if 0

typedef struct
{
  int start, int middle, int end;
  int *in, int *out;
} task_info_t;

static volatile int finished = 0;
static volatile int* task_counters;
static task_info_t* task_infos;

static void
merge_task(merge_task_t* task)
{
}

static void
sort_task(sort_task_t* task)
{
  // TODO: Implement sort

  int parent_task = (task->id - 1) / 2;
  if (__sync_add_and_fetch(&task_counters[parent_task], 1) == 2) {
    // Push a merge task
    int start, middle, end;
    merge_task_t* task;

    task = merge_task_new(parent_task, task->a, task->
  }
}

static void *
thread_func(void * buffer)
{
	while (finished == 0)
	{
    task_t *task;

    // Pull task
    if (queue_pop(&task_queue, &task) != 0) {
      if (finished)
        break;

      // Sleep a little while
      usleep(100);
    }

    switch (task->type) {
      case TASK_TYPE_SORT:
        sort_task((sort_task_t*)task);
        break;
      case TASK_TYPE_MERGE:
        merge_task((merge_task_t*)task);
        break;

      default:
        assert(0); // Should not be reached
    }
	}

	return NULL;
}

static int
find_n_sorts(void)
{
	int n_sorts, best_diff, best_n_sorts;

  n_sorts = 2;
  best_diff = INT_MAX;
  for (splits = 2;; splits *= 2) {
    int sort_size = (array->length + n_sorts - 1) / n_sorts;
    int diff = abs(sort_size - SEQUENTIAL_SORT_SIZE);
    if (diff > best)
      break;
    else if (diff < best) {
      best_diff = diff;
      best_n_sorts = n_sorts;
    }
  }

  return best_n_sorts;
}

static int
parallell_sort(struct array* array)
{
  pthread_t thread[NB_THREADS];
  pthread_attr_t thread_attr;
  int i;
  int n_sorts, n_tasks;

  // Initialise thread poll / master thread synchronisation
  pthread_barrier_init(&thread_pool_barrier, NULL, NB_THREADS + 1);

  // Initialize attributes
  pthread_attr_init(&thread_attr);
  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

  // Find the number of splits that takes us closes to our wanted split size
  n_sorts = find_n_sorts();
  n_tasks = n_sorts*2 - 1;
  
  // Allocate task_counters and task_sizes
  task_counters = malloc(sizeof(int) * n_tasks);
  task_sizes = malloc(sizeof(task_size_t) * n_tasks);

  // Push sort tasks
  int sort_size = (array->length + n_sorts - 1) / n_sorts;
  for (i = 0; i < n_sorts; i++) {
    int task_id, start, end;
    sort_task_t* task;
    
    task_id = n_tasks - i - 1;
    start = i*split_size;
    end = MAX(array->length, start + split_size);

    task_sizes[task_id].start = start;
    task_sizes[task_id].end = end;

    task = sort_task_new(task_id);
    queue_push(&task_queue, task);
  }

  // Calculate merge task sizes
  for (i = n_sorts - 2; i >= 0; i--) {
    int child1, child2;

    child1 = i*2+1;
    child2 = i*2+2;

    task_sizes[i].start = task_sizes[child1].start;
    task_sizes[i].middle = task_sizes[child1].end + 1;
    task_sizes[i].end = task_sizes[child2].end;
  }

  // Start the worker threads
  for (i = 0; i < NB_THREADS; i++)
  {
    pthread_create(&thread[i], &thread_attr, &thread_func, NULL);
  }

  // Wait for the threads to finish
  for (i = 0; i < NB_THREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  return 0;
}
#endif

int
sort(struct array * array)
{
  quicksort(array->data, 0, array->length - 1);

  return 0;
}
