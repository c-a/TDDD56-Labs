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
#else
#if NON_BLOCKING == 1 
#warning Stacks are synchronized through lock-based CAS
#else
#warning Stacks are synchronized through hardware CAS
#endif
#endif

stack_t *
stack_alloc()
{
  // Example of a task allocation with correctness control
  // Feel free to change it
  stack_t *res;

  res = malloc(sizeof(stack_t));
  assert(res != NULL);

  if (res == NULL)
    return NULL;

// You may allocate a lock-based or CAS based stack in
// different manners if you need so
#if NON_BLOCKING == 0
  // Implement a lock_based stack
#elif NON_BLOCKING == 1
  /*** Optional ***/
  // Implement a harware CAS-based stack
#else
  // Implement a harware CAS-based stack
#endif

  return res;
}

int
stack_init(stack_t *stack, size_t size)
{
  assert(stack != NULL);
  assert(size > 0);

#if NON_BLOCKING == 0
  // Implement a lock_based stack
#elif NON_BLOCKING == 1
  /*** Optional ***/
  // Implement a harware CAS-based stack
#else
  // Implement a harware CAS-based stack
#endif

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
stack_push(stack_t *stack, void* buffer)
{
#if NON_BLOCKING == 0
  // Implement a lock_based stack
#elif NON_BLOCKING == 1
  /*** Optional ***/
  // Implement a harware CAS-based stack
#else
  // Implement a harware CAS-based stack
#endif

  return 0;
}

int
stack_pop(stack_t *stack, void* buffer)
{
#if NON_BLOCKING == 0
  // Implement a lock_based stack
#elif NON_BLOCKING == 1
  /*** Optional ***/
  // Implement a harware CAS-based stack
#else
  // Implement a harware CAS-based stack
#endif

  return 0;
}

