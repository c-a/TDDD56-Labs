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

struct stack_node
{
  void *data;
  struct stack_node *prev;
};
typedef struct stack_node stack_node;

struct stack
{
  stack_node *head;
#if NON_BLOCKING == 0
#warning Stacks are synchronized through locks
  pthread_mutex_t mutex;
#else
#if NON_BLOCKING == 1 
#warning Stacks are synchronized through lock-based CAS
  pthread_mutex_t lock;
#else
#warning Stacks are synchronized through hardware CAS
#endif
#endif
};

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
  // Implement a software CAS-based stack
  if (pthread_mutex_init(&stack->lock, NULL) != 0)
    return -1;
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
  // Implement a software CAS-based stack
  if (pthread_mutex_destroy(&stack->lock) == 0)
    return -1;
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
  // Implement a software CAS-based stack
  stack_node *head;
  do {
    head = stack->head;
    new->prev = head;
  } while (software_cas((size_t*)&stack->head, (size_t)head, (size_t)new, &stack->lock) != (size_t)head);
#else
  // Implement a harware CAS-based stack
  stack_node *head;
  do {
    head = stack->head;
    new->prev = head;
  } while (cas((size_t*)&stack->head, (size_t)head, (size_t)new) != (size_t)head);
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
  // Implement a software CAS-based stack
  stack_node *new_head;
  do {
    popped = stack->head;
    new_head = popped != NULL ? popped->prev : NULL;
  } while (software_cas((size_t*)&stack->head, (size_t)popped, (size_t)new_head, &stack->lock) != (size_t)popped);
#else
  // Implement a hardware CAS-based stack
  stack_node *new_head;
  do {
    popped = stack->head;
    new_head = popped != NULL ? popped->prev : NULL;
  } while (cas((size_t*)&stack->head, (size_t)popped, (size_t)new_head) != (size_t)popped);
#endif

  if (popped == NULL)
    return -1;

  if (data != NULL)
    *data = popped->data;
  free(popped);

  return 0;
}

