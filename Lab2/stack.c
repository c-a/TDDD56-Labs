/*
 * stack.c
 *
 *  Created on: 18 Oct 2011
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
 *     but WITHOUT ANY WARRANTY without even the implied warranty of
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

#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "stack.h"
#include "non_blocking.h"

#if NON_BLOCKING == 0
#warning Stacks are synchronized through locks

struct stack_node
{
  void *data;
  struct stack_node *prev;
};
typedef struct stack_node stack_node;

struct stack
{
  stack_node *head;
  pthread_mutex_t mutex;  
};

#else
#if NON_BLOCKING == 1 
#warning Stacks are synchronized through lock-based CAS
#else
#warning Stacks are synchronized through hardware CAS
#endif
#endif

static int stack_init(stack_t *stack);

stack_t *
stack_alloc(void)
{
  stack_t *stack;

  stack = malloc(sizeof(struct stack));
  if (stack == NULL)
    return NULL;

  if (stack_init(stack) != 0)
  {
    free(stack);
    return NULL;
  }

  return stack;
}

static int
stack_init(stack_t *stack)
{
  assert(stack != NULL);

  stack->head = NULL;

#if NON_BLOCKING == 0
  // Implement a lock_based stack
  if (pthread_mutex_init(&stack->mutex, NULL) != 0)
    return -1;
#elif NON_BLOCKING == 1
  /*** Optional ***/
  // Implement a harware CAS-based stack
#else
  // Implement a harware CAS-based stack
#endif

  return 0;
}

int
stack_free(stack_t *stack)
{
  stack_node *node;

  assert(stack != NULL);

  // Free all nodes
  node = stack->head;
  while (node != NULL)
  {
    stack_node *tmp = node->prev;
    free(node);
    node = tmp;
  }
  
#if NON_BLOCKING == 0
  if (pthread_mutex_destroy(&stack->mutex) == 0)
    return -1;
#elif NON_BLOCKING == 1
  /*** Optional ***/
  // Implement a harware CAS-based stack
#else
  // Implement a harware CAS-based stack
#endif

  free(stack);

  return 0;
}

int
stack_check(stack_t *stack)
{
  /*** Optional ***/
  // Use code and assertions to make sure your stack is
  // in a consistent state and is safe to use.
  //
  // For now, just makes just the pointer is not NULL
  //
  // Debugging use only

  assert(stack != NULL);

  return 0;
}

int
stack_push(stack_t *stack, void* data)
{
  stack_node *new;

  assert(stack != NULL);

  new = calloc(sizeof(stack_t), 1);
  if (new == NULL)
    return -1;

  new->data = data;

#if NON_BLOCKING == 0
  // Implement a lock_based stack
  pthread_mutex_lock(&stack->mutex);
  new->prev = stack->head;
  stack->head = new;
  pthread_mutex_unlock(&stack->mutex);

#elif NON_BLOCKING == 1
  /*** Optional ***/
  // Implement a harware CAS-based stack
#else
  // Implement a harware CAS-based stack
#endif

  return 0;
}

int
stack_pop(stack_t *stack, void** data)
{
  stack_node *popped = NULL;

  assert(stack != NULL);

#if NON_BLOCKING == 0
  // Implement a lock_based stack
  pthread_mutex_lock(&stack->mutex);
  if (stack->head != NULL) {
    popped = stack->head;
    stack->head = popped->prev;
  }
  pthread_mutex_unlock(&stack->mutex);

#elif NON_BLOCKING == 1
  /*** Optional ***/
  // Implement a harware CAS-based stack
#else
  // Implement a harware CAS-based stack
#endif

  if (popped == NULL)
    return -1;

  if (data != NULL)
    *data = popped->data;
  free(popped);

  return 0;
}

