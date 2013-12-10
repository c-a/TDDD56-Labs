
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "quicksort-task.h"

#define TO_NEXT_BLOCKS(left, right) (((size_t)(left) << 32) | (size_t)((right) & 0xffffffff))

void
partition_task_init(task_t* task,
                    unsigned int start, unsigned int end)
{
  int i;

  assert(end > start);

  task->type = TASK_TYPE_PARTITION;
  task->partition.state = PARTITION_STATE_INITIAL;
  task->partition.busy_threads = 0;
  task->partition.start = start;
  task->partition.end = end;

  task->partition.blocks = ((end - start + 1) + BLOCK_SIZE - 1) / BLOCK_SIZE;
  task->partition.next_blocks = TO_NEXT_BLOCKS(0, task->partition.blocks - 1);

  for (i = 0; i < NB_THREADS; i++)
    task->partition.remaining_blocks[i] = -1;

  task->partition.left_neutralized = 0;
  task->partition.right_neutralized = 0;
}

block_t partition_task_get_block(task_t* task, int index) {
  block_t block = { index };

  if (index < 0)
    return block;

  block.start = task->partition.start + index * BLOCK_SIZE;
  block.end = MIN(block.start + BLOCK_SIZE - 1, task->partition.end);

  return block;
}

block_t
partition_task_get_left_block(task_t* task)
{
  size_t next_blocks, new_next_blocks;
  unsigned int left, right;

  do {
    next_blocks = task->partition.next_blocks;
    left = (next_blocks >> 32) & 0xffffffff;
    right = next_blocks & 0xffffffff;

    if (left > right)
      return partition_task_get_block(task, -1);

    new_next_blocks =  TO_NEXT_BLOCKS(left + 1, right);

  } while (__sync_val_compare_and_swap(&task->partition.next_blocks, next_blocks, new_next_blocks) != next_blocks);

  return partition_task_get_block(task, left);
}

block_t
partition_task_get_right_block(task_t* task)
{
  size_t next_blocks, new_next_blocks;
  unsigned int left, right;

  do {

    next_blocks = task->partition.next_blocks;
    left = (next_blocks >> 32) & 0xffffffff;
    right = next_blocks & 0xffffffff;

    if (left > right)
      return partition_task_get_block(task, -1);

    new_next_blocks =  TO_NEXT_BLOCKS(left, right - 1);

  } while (__sync_val_compare_and_swap(&task->partition.next_blocks, next_blocks, new_next_blocks) != next_blocks);

  return partition_task_get_block(task, right);
}

void
sort_task_init(task_t* task,
               unsigned int start, unsigned int end)
{
  task->type = TASK_TYPE_SORT;
  task->sort.start = start;
  task->sort.end = end;
}

struct task_stack_node
{
  task_t task;
  struct task_stack_node* prev;
};

int
task_stack_init(task_stack_t *stack, unsigned int size)
{
  assert(stack != NULL);

  stack->size = size;
  stack->nodes = malloc(size*sizeof(task_stack_node_t));
  if (stack->nodes == NULL)
    return ENOMEM;
  stack->next_node = 0;

  stack->head = NULL;

  return 0;
}

void
task_stack_destroy(task_stack_t* stack)
{
  assert(stack != NULL);

  free(stack->nodes);
}

task_t*
task_stack_reserve(task_stack_t* stack)
{
  unsigned int node_index;

  assert(stack != NULL);

  if (stack->next_node >= stack->size)
    return NULL;

  node_index = __sync_fetch_and_add(&stack->next_node, 1);
  if (node_index >= stack->next_node)
    return NULL;

  return &stack->nodes[node_index].task;
}

void
task_stack_push(task_stack_t *stack, task_t* task)
{
  task_stack_node_t *node, *head;

  assert(stack != NULL);
  assert(task != NULL);

  /* Task is the first element in the task_node_t struct so we can just cast
   * it to a task_node_t. */
  node = (task_stack_node_t*)task;

  do {
    head = stack->head;
    node->prev = head;
  } while (__sync_val_compare_and_swap(&stack->head, head, node) != head);
}

int
task_stack_pop(task_stack_t* stack, task_t** task)
{
  task_stack_node_t* popped = NULL;
  task_stack_node_t* new_head;

  assert(stack != NULL);

  do {
    task_t* t;

    popped = stack->head;
    if (popped == NULL)
      break;

    // If the task is a partition task we just return it without popping if it hasn't
    // finished doing parallel partitioning yet.
    t = (task_t*)popped;
    if (t->type == TASK_TYPE_PARTITION &&
        t->partition.state < PARTITION_STATE_PARALLEL_FINISHED)
      break;

    new_head = popped->prev;
  } while (__sync_val_compare_and_swap(&stack->head, popped, new_head) != popped);

  if (popped == NULL)
    return -1;

  if (task != NULL)
    *task = &popped->task;

  return 0;
}
