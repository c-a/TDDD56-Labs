#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>

#include "mergesort-task.h"
#include "array.h"
#include "quicksort.h"

#define SEQUENTIAL_SORT_SIZE 2000000

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef struct
{
  unsigned int start, middle, end;
  unsigned int n_tasks;
  int *in, *out;
} task_size_t;

static volatile int finished_counter;
static volatile int* task_counters;
static task_size_t* task_sizes;

static task_stack_t task_stack;

static void
create_merge_tasks(int task_id)
{
  int start, end;
  int n_tasks, merge_size, i;
  task_t* task;

  start = task_sizes[task_id].start;
  end = task_sizes[task_id].end;

  /* Create new merge tasks */
  n_tasks = task_sizes[task_id].n_tasks;
  merge_size = ((end - start + 1) + (n_tasks - 1)) / n_tasks;
  for (i = 0; i < n_tasks; i++) {
    int from, to;

    from = start + i*merge_size;
    to = MIN(end, from + merge_size - 1);

    task = task_stack_reserve(&task_stack);
    assert(task != NULL);

    merge_task_init(task, task_id, from, to);
    task_stack_push(&task_stack, task);
  }
}

static unsigned int
find_insert_pos_in_a_array(int* a, int start, int end, int val)
{

  while (start <= end)
  {
    unsigned int m = start + (end - start) / 2;

    if (a[m] <= val)
      start = m + 1;
    else
      end = m - 1;
  }

  return start;
}

static unsigned int
find_insert_pos_in_b_array(int* b, int start, int end, int val)
{

  while (start <= end)
  {
    unsigned int m = start + (end - start) / 2;

    if (b[m] >= val)
      end = m - 1;
    else
      start = m + 1;
  }

  return start;
}

static void
merge(task_t* task)
{
  int *in, *out;
  unsigned int start, mid, end, from, to;
  unsigned int i;

  in = task_sizes[task->id].in;
  out = task_sizes[task->id].out;

  start = task_sizes[task->id].start;
  mid = task_sizes[task->id].middle;
  end = task_sizes[task->id].end;

  from = task->merge.from;
  to = task->merge.to;

  for (i = from; i <= MIN(mid - 1, to); i++) {
    unsigned int pos_in_b;

    pos_in_b = find_insert_pos_in_b_array(in, mid, end, in[i]);
    out[i + (pos_in_b - mid)] = in[i];
  }

  for (i = MAX(from, mid); i <= MIN(end, to); i++) {
    unsigned int pos_in_a;

    pos_in_a = find_insert_pos_in_a_array(in, start, mid - 1, in[i]);
    out[pos_in_a + (i - mid)] = in[i];
  }
}

static void
merge_task(task_t* task)
{
  int parent_task;

  /* Merge */
  __sync_synchronize();
  merge(task);

  /* Finished */
  if (task->id == 0) {
    __sync_add_and_fetch(&finished_counter, -1);
    return;
  }

  parent_task = (task->id - 1) / 2;

  /* Create new merge tasks if the two merges has finished. */
  if (__sync_add_and_fetch(&task_counters[parent_task], -1) == 0)
    create_merge_tasks(parent_task);
}

static void
sort_task(task_t* task)
{
  int parent_task;
  
  /* Sort */
  quicksort(task_sizes[task->id].out, task_sizes[task->id].start, task_sizes[task->id].end);
  __sync_synchronize();

  parent_task = (task->id - 1) / 2;

  /* Create new merge tasks if the two sorts has finished. */
  if (__sync_add_and_fetch(&task_counters[parent_task], -1) == 0)
    create_merge_tasks(parent_task);
}

static void*
thread_func(void* data)
{
  while (finished_counter != 0)
  {
    task_t *task;

    /* Pull task */
    if (task_stack_pop(&task_stack, &task) != 0) {
      if (finished_counter == 0)
        break;

      /* Sleep a little while */
      usleep(10);
      continue;
    }

    switch (task->type) {
      case TASK_TYPE_SORT:
        sort_task(task);
        break;
      case TASK_TYPE_MERGE:
        merge_task(task);
        break;

      default:
        assert(0); /* Should not be reached */
    }
  }

  return NULL;
}

static int
find_n_sorts(int length)
{
  int n_sorts, best_diff, best_n_sorts;

  best_diff = INT_MAX;
  for (n_sorts = 2;; n_sorts <<= 1) {
    int sort_size = (length + n_sorts - 1) / n_sorts;
    int diff = abs(sort_size - SEQUENTIAL_SORT_SIZE);
    if (diff > best_diff)
      break;
    else if (diff < best_diff) {
      best_diff = diff;
      best_n_sorts = n_sorts;
    }
  }

  return best_n_sorts;
}

int
parallel_mergesort(struct array* array)
{
  pthread_t thread[NB_THREADS];
  pthread_attr_t thread_attr;
  int i;
  int n_sorts, n_tasks;
  int* buffer;

  /* Initialize attributes */
  pthread_attr_init(&thread_attr);
  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

  /* Find the number of splits that takes us closes to our wanted split size */
  n_sorts = find_n_sorts(array->length);
  n_tasks = n_sorts*2 - 1;

  /* Allocate task_counters and task_sizes */
  task_counters = malloc(sizeof(int) * n_tasks);
  task_sizes = malloc(sizeof(task_size_t) * n_tasks);

  /* Init task stack */
  task_stack_init(&task_stack, n_tasks*10);

  /* Allocate buffer used during merging */
  buffer = malloc(array->length * sizeof(int));
  assert(buffer != NULL);

  /* Create sort tasks */
  int sort_size = ((array->length + n_sorts - 1) / n_sorts);
  for (i = 0; i < n_sorts; i++) {
    int task_id, start, end;
    task_t* task;

    task_id = n_tasks - i - 1;
    start = i*sort_size;
    end = MIN(array->length - 1, start + sort_size - 1);

    task_sizes[task_id].start = start;
    task_sizes[task_id].end = end;
    task_sizes[task_id].in = buffer;
    task_sizes[task_id].out = array->data;
    task_sizes[task_id].n_tasks = 1;

    task = task_stack_reserve(&task_stack);
    assert(task != NULL);

    sort_task_init(task, task_id);
    task_stack_push(&task_stack, task);
  }

  /* Calculate sizes of merge tasks */
  for (i = n_sorts - 2; i >= 0; i--) {
    int task_id;
    int child1, child2;

    task_id = i;

    /* Calculate merge task size */
    child1 = task_id*2+2;
    child2 = task_id*2+1;

    task_sizes[task_id].start = task_sizes[child1].start;
    task_sizes[task_id].middle = task_sizes[child1].end + 1;
    task_sizes[task_id].end = task_sizes[child2].end;
    task_sizes[task_id].in = task_sizes[child1].out;
    task_sizes[task_id].out = task_sizes[child1].in;
    task_sizes[task_id].n_tasks = 2*task_sizes[child1].n_tasks;

    task_counters[task_id] = task_sizes[task_id].n_tasks;
  }

  finished_counter = task_sizes[0].n_tasks;

  /* Start the worker threads */
  for (i = 0; i < NB_THREADS; i++)
  {
    pthread_create(&thread[i], &thread_attr, &thread_func, NULL);
  }

  /* Wait for the threads to finish */
  for (i = 0; i < NB_THREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  if (task_sizes[0].out != array->data) {
    int* tmp;
    tmp = array->data;
    array->data = buffer;
    buffer = tmp;
  }

#if 0
  /* Destroy task stack */
  task_stack_destroy(&task_stack);

  /* Free task_counters and task_sizes */
  free((void*)task_counters);
  free(task_sizes);
#endif

  return 0;
}
