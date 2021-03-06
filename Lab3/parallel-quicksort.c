#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

#include "quicksort-task.h"
#include "array.h"
#include "quicksort.h"

#define SEQUENTIAL_SORT_SIZE 20000

int* array;
unsigned volatile int task_counter;
static task_stack_t task_stack;

static inline void
swap(int* a, int* b) {
  int c;

  c = *a;
  *a = *b;
  *b = c;
}

static inline int
median_of_three(int* a, int first, int last) {

  int mid = first + (last - first) / 2;

  /* Puts smallest value first, biggest value last and the middle in the middle*/
  if (a[last] < a[first])
    swap(&a[last], &a[first]);
  if (a[mid] < a[first])
    swap(&a[mid], &a[first]);
  if (a[last] < a[mid])
    swap(&a[last], &a[mid]);

  return mid;
}

typedef enum
{
  LEFT,
  RIGHT,
  BOTH
} side_t;

static side_t
neutralize(block_t* left_block, block_t* right_block,
           int pivot)
{
  int *al, *ar, *bl, *br;

  al = array + left_block->start;
  ar = array + left_block->end;
  bl = array + right_block->start;
  br = array + right_block->end;

  do {
    while (al <= ar && *al <= pivot) al++;
    while (bl <= br && *bl >= pivot) bl++;

    if (al > ar || bl > br)
      break;

    swap(al, bl);
  } while (true);

  left_block->start = (al - array);
  right_block->start = (bl - array);

  if (al > ar && bl > br)
    return BOTH;
  if (al > ar)
    return LEFT;
  return RIGHT;
}

#if 1
static void
check_partitions(task_t* task, unsigned int split) {
  unsigned int i;

  for (i = task->partition.start; i <= split; i++)
    if (array[i] > task->partition.pivot)
      assert(false);

  for (i = split + 1; i <= task->partition.end; i++)
    if (array[i] < task->partition.pivot)
      assert(false);
}
#endif

static unsigned int
sequential_partition(task_t* task) {
  unsigned int left, right, i, start, end;
  block_t left_block, right_block;

  if (task->partition.left_neutralized == task->partition.blocks)
    return task->partition.end;
  else if (task->partition.right_neutralized == task->partition.blocks)
    return task->partition.start;
  
  // Sequential partitioning
  int* blocks = task->partition.remaining_blocks;
  quicksort(blocks, 0, NB_THREADS - 1);

  // Try to neutralize the remaining blocks
  left = 0;
  while (left < NB_THREADS && blocks[left] < 0) left++;
  right = NB_THREADS - 1;

  left_block = partition_task_get_block(task, blocks[left]);
  right_block = partition_task_get_block(task, blocks[right]);
  while (left < right) {
    side_t side;

    side = neutralize(&left_block, &right_block, task->partition.pivot);
    if (side == LEFT || side == BOTH) {
      if (blocks[left] < task->partition.left_neutralized) {
        task->partition.left_neutralized++;
        blocks[left] = -1;
      }
      left_block = partition_task_get_block(task, blocks[++left]);
    }
    if (side == RIGHT || side == BOTH) {
      if (blocks[right] >= (task->partition.blocks - task->partition.right_neutralized)) {
        task->partition.right_neutralized++;
        blocks[right] = -1;
      }
      right_block = partition_task_get_block(task, blocks[--right]);
    }
  }

  if (task->partition.left_neutralized == task->partition.blocks)
    return task->partition.end;
  else if (task->partition.right_neutralized == task->partition.blocks)
    return task->partition.start;

  // Swap non-neutralized blocks into position.
  left = task->partition.left_neutralized;
  right = task->partition.blocks - task->partition.right_neutralized - 1;
  for (i = 0; i < NB_THREADS; i++) {
    if (blocks[i] < 0)
      continue;

    block_t block_a, block_b;
    block_a = partition_task_get_block(task, blocks[i]);
    if (blocks[i] < left)
      block_b = partition_task_get_block(task, left++);
    else if (blocks[i] > right)
      block_b = partition_task_get_block(task, right--);
    else
      continue;

    int *a = &array[block_a.start], *ae =  &array[block_a.end], *b = &array[block_b.start];
    for (; a <= ae; a++, b++)
      swap(a, b);
  }

  // Sequentially partition the remaining non neutralized area
  left_block = partition_task_get_block(task, task->partition.left_neutralized);
  right_block = partition_task_get_block(task, task->partition.blocks - task->partition.right_neutralized - 1);
  start  = left_block.start;
  end = right_block.end;
  int *s = array + start, *e = array + end;
  int *l = array + start - 1, *r = array + end + 1;
  int pivot = task->partition.pivot;
  while (true) {
    while (*(++l) < pivot) if (l == e) break;
    while (*(--r) > pivot) if (r == s) break;

    if (l >= r)
      break;

    swap(l, r);
  }

  return (r - array);
}

static void
parallel_partition(task_t* task, int tid) {
  block_t left_block, right_block;
  unsigned int left_counter, right_counter;

  // Parallel partitioning
  left_block = partition_task_get_left_block(task);
  if (left_block.index < 0)
    return;
  right_block = partition_task_get_right_block(task);

  left_counter = right_counter = 0;
  while (left_block.index >= 0 && right_block.index >= 0) {
    side_t side;

    side = neutralize(&left_block, &right_block, task->partition.pivot);
    if (side == LEFT || side == BOTH) {
      left_block = partition_task_get_left_block(task);
      left_counter++;
    }
    if (side == RIGHT || side == BOTH) {
      right_block = partition_task_get_right_block(task);
      right_counter++;
    }
  }

  if (left_block.index >= 0)
    task->partition.remaining_blocks[tid] = left_block.index;
  else
    task->partition.remaining_blocks[tid] = right_block.index;

  __sync_fetch_and_add(&task->partition.left_neutralized, left_counter);
  __sync_fetch_and_add(&task->partition.right_neutralized, right_counter);
}

static void
partition_task(task_t* task, int tid)
{
  volatile partition_state_t* state;
  bool owner;

  state = &task->partition.state;
  if (*state >= PARTITION_STATE_PARALLEL_FINISHED)
    return;

  owner = __sync_val_compare_and_swap(state, PARTITION_STATE_INITIAL, PARTITION_STATE_OWNED) == PARTITION_STATE_INITIAL;
  if (owner) {
    // Choose pivot
#if 1
    int samples[20], i;
    int data_step_size = (task->partition.end - task->partition.start + 1) / 20;
    for (i = 0; i < 20; i++)
      samples[i] = array[task->partition.start + i*data_step_size];
    quicksort(samples, 0, 20 - 1);
    task->partition.pivot = samples[10];
#else
    int m = median_of_three(array, task->partition.start, task->partition.end);
    task->partition.pivot = array[m];
#endif
    *state = PARTITION_STATE_PIVOT_CHOSEN;
  __sync_synchronize();
  }
  else {
    // Increase the number of busy threads for this task.
    __sync_fetch_and_add(&task->partition.busy_threads, 1);

    // Busy wait till pivot has been chosen.
    while (*state < PARTITION_STATE_PIVOT_CHOSEN);
  }

  parallel_partition(task, tid);

  __sync_val_compare_and_swap(state, PARTITION_STATE_PIVOT_CHOSEN, PARTITION_STATE_PARALLEL_FINISHED);
  if (!owner) {
    __sync_fetch_and_add(&task->partition.busy_threads, -1);
    return;
  }

  // Wait for all other threads to finish
  __sync_synchronize();
  while (task->partition.busy_threads != 0);
  __sync_synchronize();

  unsigned int split = sequential_partition(task);

#if 1
  check_partitions(task, split);
#endif

  // We create two new tasks and finish one >= +1 tasks
  __sync_fetch_and_add(&task_counter, 1);

  task_t* new_task = task_stack_reserve(&task_stack);
  if (task->partition.end - split <= SEQUENTIAL_SORT_SIZE)
    sort_task_init(new_task, split + 1, task->partition.end);
  else
    partition_task_init(new_task, split + 1, task->partition.end);
  task_stack_push(&task_stack, new_task);

  new_task = task_stack_reserve(&task_stack);
  if (split - task->partition.start + 1 <= SEQUENTIAL_SORT_SIZE)
    sort_task_init(new_task, task->partition.start, split);
  else
    partition_task_init(new_task, task->partition.start, split);
  task_stack_push(&task_stack, new_task);

  task->partition.state = PARTITION_STATE_FINISHED;
}

static void
sort_task(task_t* task)
{
  // Sort
  quicksort(array, task->sort.start, task->sort.end);

  // Finished one task
  __sync_fetch_and_add(&task_counter, -1);
}

static void*
thread_func(void* data)
{
  int tid = (int)(long)data;

  while (task_counter > 0)
  {
    task_t *task;

    /* Pull task */
    if (task_stack_pop(&task_stack, &task) != 0) {
      usleep(50);
      continue;
    }

    switch (task->type) {
      case TASK_TYPE_PARTITION:
        partition_task(task, tid);
        break;
      case TASK_TYPE_SORT:
        sort_task(task);
        break;

      default:
        assert(0); // Should not be reached
    }
  }

  return NULL;
}

int find_n_tasks(int size) {
  int n_tasks = 2;

  while (size > SEQUENTIAL_SORT_SIZE) {
    size = (size + 1) >> 1;
    n_tasks <<= 1;
  }

  return n_tasks - 1;
}

int
parallel_quicksort(struct array* in_array)
{
  pthread_t thread[NB_THREADS];
  pthread_attr_t thread_attr;
  int n_tasks;
  int i;
  task_t* task;

  /* Just do a single quicksort if array size is small */
  if (in_array->length <= SEQUENTIAL_SORT_SIZE) {
    quicksort(in_array->data, 0, in_array->length - 1);
    return 0;
  }

  /* Initialize attributes */
  pthread_attr_init(&thread_attr);
  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

  /* Init task stack */
  n_tasks = find_n_tasks(in_array->length);
  task_stack_init(&task_stack, n_tasks*10);

  /* Push inital partition task. */
  array = in_array->data;
  task_counter = 1;
  task = task_stack_reserve(&task_stack);
  partition_task_init(task, 0, in_array->length - 1);
  task_stack_push(&task_stack, task);

  /* Start the worker threads */
  for (i = 0; i < NB_THREADS; i++)
  {
    pthread_create(&thread[i], &thread_attr, &thread_func, (void*)(long)i);
  }

  /* Wait for the threads to finish */
  for (i = 0; i < NB_THREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  
#if 0
  /* Destroy task stack */
  task_stack_destroy(&task_stack);
#endif

  return 0;
}
