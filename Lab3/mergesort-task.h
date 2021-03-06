
#include <stdlib.h>

#ifndef TASK_H
#define TASK_H

typedef enum
{
  TASK_TYPE_SORT,
  TASK_TYPE_MERGE
} task_type_t;

typedef struct
{
  task_type_t type;
  int id;

  union {
    struct 
    {
    } sort;
    struct 
    {
      unsigned int from;
      unsigned int to;
    } merge;
  };
  
} task_t;

void sort_task_init(task_t* task, int id);
void merge_task_init(task_t* task, int id,
                     unsigned int from, unsigned int to);

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
