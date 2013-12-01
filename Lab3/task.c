
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "task.h"
#include "non_blocking.h"

void
sort_task_init(task_t* task, int id)
{
  task->type = TASK_TYPE_SORT;
  task->id = id;
}

void
merge_task_init(task_t* task, int id,
                unsigned int from, unsigned int to)
{
  task->type = TASK_TYPE_MERGE;
  task->id = id;

  task->merge.from = from;
  task->merge.to = to;
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
  } while (cas((size_t*)&stack->head, (size_t)head, (size_t)node) != (size_t)head);
}

int
task_stack_pop(task_stack_t* stack, task_t** task)
{
  task_stack_node_t* popped = NULL;
  task_stack_node_t* new_head;

  assert(stack != NULL);

  do {
    popped = stack->head;
    new_head = popped != NULL ? popped->prev : NULL;
  } while (cas((size_t*)&stack->head, (size_t)popped, (size_t)new_head) != (size_t)popped);

  if (popped == NULL)
    return -1;

  if (task != NULL)
    *task = &popped->task;

  return 0;
}
