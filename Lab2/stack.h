/*
 * stack.h
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
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with TDDD56. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef STACK_H
#define STACK_H

struct stack_node
{
  void *data;
  struct stack_node *prev;
};
typedef struct stack_node stack_node_t;

typedef struct stack stack_t;

stack_t * stack_alloc(void);
int       stack_free(stack_t *stack);

stack_node_t * stack_node_alloc(void);

int       stack_push(stack_t *stack, stack_node_t* node);
int       stack_pop(stack_t *stack, stack_node_t** node);

int       stack_pop_aba(stack_t *stack, stack_node_t** node,
                        sem_t* read_a_sem, sem_t* reinsert_a_sem);

void      stack_print_aba(stack_t *stack);

int       stack_check(stack_t *stack);


#endif /* STACK_H */
