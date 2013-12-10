
#include <stdbool.h>
#include <stdlib.h>

#ifndef TASK_H
#define TASK_H

#define BLOCK_SIZE 2048

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef enum
{
  TASK_TYPE_PARTITION,
  TASK_TYPE_SORT
} task_type_t;

typedef enum
{
  PARTITION_STATE_INITIAL,
  PARTITION_STATE_OWNED,
  PARTITION_STATE_PIVOT_CHOSEN,
  PARTITION_STATE_PARALLEL_FINISHED,
  PARTITION_STATE_FINISHED
} partition_state_t;

typedef struct
{
  int index;
  unsigned int start;
  unsigned int end;
} block_t;

typedef struct
{
  task_type_t type;
  union {
    struct 
    {
      volatile partition_state_t state;
      volatile unsigned int busy_threads;

      unsigned int start;
      unsigned int end;

      unsigned int blocks;
      size_t next_blocks;

      int pivot;
      int remaining_blocks[NB_THREADS];
      volatile unsigned int left_neutralized;
      volatile unsigned int right_neutralized;
    } partition;
    struct 
    {
      unsigned int start;
      unsigned int end;
    } sort;
  };
  
} task_t;

void partition_task_init(task_t* task,
                         unsigned int start, unsigned int end);
block_t partition_task_get_left_block(task_t* task);
block_t partition_task_get_right_block(task_t* task);

block_t partition_task_get_block(task_t* task, int index);

void sort_task_init(task_t* task,
                    unsigned int start, unsigned int end);

typedef struct task_stack_node task_stack_node_t;

struct task_stack
{
  unsigned int size;
  task_stack_node_t *nodes;
  volatile unsigned int next_node;

  task_stack_node_t *head;
};
typedef struct task_stack task_stack_t;

int       task_stack_init(task_stack_t* stack, unsigned int size);
void      task_stack_destroy(task_stack_t* stack);

task_t*   task_stack_reserve(task_stack_t* stack);

void      task_stack_push(task_stack_t* stack, task_t* task);
int       task_stack_pop(task_stack_t* stack, task_t** task);


#endif /* TASK_H */
