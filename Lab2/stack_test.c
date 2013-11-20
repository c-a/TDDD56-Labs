/*
 * stack_test.c
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

#ifndef DEBUG
#define NDEBUG
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stddef.h>

#include <semaphore.h>

#include "stack.h"
#include "non_blocking.h"

#define test_run(test)\
  printf("[%s:%s:%i] Running test '%s'... ", __FILE__, __FUNCTION__, __LINE__, #test);\
  test_setup();\
  if(test())\
  {\
    printf("passed\n");\
  }\
  else\
  {\
    printf("failed\n");\
  }\
  test_teardown();

typedef int data_t;
#define DATA_SIZE sizeof(data_t)
#define DATA_VALUE 5

static stack_t *stack;
static data_t data;

static stack_node_t *stack_nodes[MAX_PUSH_POP];
static volatile int next_node;

void
test_init()
{
  // Initialize your test batch
}

void
test_setup()
{
  int i;

  // Allocate and initialize your test stack before each test
  data = DATA_VALUE;
  stack = stack_alloc();

  // Allocate MAX_PUSH_POP stack_nodes
  for (i = 0; i < MAX_PUSH_POP; i++)
  {
    stack_nodes[i] = stack_node_alloc();
  }
  next_node = 0;

#if MEASURE == 2
  // Fill the stack with MAX_PUSH_POP elements
  for (i = 0; i < MAX_PUSH_POP; i++)
  {
    stack_node_t *node = stack_nodes[i];
    node->data = &data;
    stack_push(stack, node);
  }
#endif
}

void
test_teardown()
{
  // Do not forget to free your stacks after each test
  // to avoid memory leaks as now
  stack_free(stack);
}

void
test_finalize()
{
  // Destroy properly your test batch
}

static int
run_test_function(void* (*func)(void* arg))
{
  pthread_attr_t attr;
  pthread_t thread[NB_THREADS];

  int i;
  int result = 0;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

  // Run NB_THREADS that run func
  for (i = 0; i < NB_THREADS; i++)
    {
      pthread_create(&thread[i], &attr, func, NULL);
    }

  // Wait for threads to finish
  for (i = 0; i < NB_THREADS; i++)
    {
      void *ret;
      pthread_join(thread[i], &ret);
      if (ret != 0)
        result = -1;
    }

    return result;
}

static void*
thread_test_push(void* arg)
{
  int i;

  for (i = 0; i < MAX_PUSH_POP; i++)
    {
      stack_node_t *node = stack_node_alloc();
      node->data = &data;
      if (stack_push(stack, node) != 0)
        return (void*)-1;
    }

  return (void*)0; 
}

int
test_push_safe()
{
  size_t counter;
  stack_node_t *node;

  // Make sure your stack remains in a good state with expected content when
  // several threads push concurrently to it

  run_test_function(&thread_test_push);

  counter = 0;
  while (stack_pop(stack, &node) == 0)
  {
    if (node->data != &data)
      return 0;
    free(node);
    counter++;
  }

  return counter == (size_t)(NB_THREADS * MAX_PUSH_POP);
}

static void*
thread_test_pop(void* arg)
{
  int i;

  for (i = 0; i < MAX_PUSH_POP; i++)
    {
      stack_node_t *node;
      if (stack_pop(stack, &node) != 0)
        return (void*)-1;
    }

  return (void*)0; 
}

int
test_pop_safe()
{
  int i;

  // Same as the test above for parallel pop operation

  for (i = 0; i < NB_THREADS * MAX_PUSH_POP; i++)
    {
      stack_node_t *node = stack_node_alloc();
      node->data = &data;
      stack_push(stack, node);
    }

  run_test_function(&thread_test_pop);

  return stack_pop(stack, NULL) == -1;
}

pthread_barrier_t aba_barrier;
sem_t aba_read_a_sem, aba_reinsert_a_sem; 

stack_node_t *A, *B, *C;

static void*
aba_thread1(void* arg)
{
  stack_node_t* node;

  // Try to pop the first element
  printf("\nThread 1: Popping first element\n");
  if (stack_pop_aba(stack, &node, &aba_read_a_sem, &aba_reinsert_a_sem) != 0)
    return (void*)-1;
  printf("Thread 1: Finished popping first element\n");

  if (node != A)
    return (void*)-1;

  return (void*)0;
}

static void*
aba_thread2(void* arg)
{
  stack_node_t* node;

  printf("Thread 2: Waiting for Thread 1 to be in the middle of popping A\n");
  // Wait for thread 1 to be in the middle of popping A
  sem_wait(&aba_read_a_sem);

  // Pop A
  printf("Thread 2: Popping A\n");
  if (stack_pop(stack, &node) != 0 || node != A)
    goto error;
  printf("Thread 2: Finished popping A\n");

  // Pop B
  printf("Thread 2: Popping B\n");
  if (stack_pop(stack, &node) != 0 || node != B)
    goto error;
  printf("Thread 2: Finished popping B\n");

  // Reinsert A
  printf("Thread 2: Reinsert A\n");
  if (stack_push(stack, A) != 0)
    goto error;
  printf("Thread 2: Finished reinserting A\n");

  // Signal thread 1 that we have reinserted A
  sem_post(&aba_reinsert_a_sem);

  return (void*)0;

error:
  return (void*)-1;
}

int
test_aba()
{
  pthread_attr_t attr;
  pthread_t thread1, thread2;
  void *ret;
  stack_node_t *node;

  // Write here a test for the ABA problem

  // Push C then B then A
  C = stack_node_alloc(); C->data = &data;
  stack_push(stack, C);

  B = stack_node_alloc(); B->data = &data;
  stack_push(stack, B);

  A = stack_node_alloc(); A->data = &data;
  stack_push(stack, A);


  // Initialize semaphores to zero
  sem_init(&aba_read_a_sem, 0, 0);
  sem_init(&aba_reinsert_a_sem, 0, 0);

  // Create the two threads needed to exercise the ABA problem
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

  pthread_create(&thread1, &attr, aba_thread1, NULL);
  pthread_create(&thread2, &attr, aba_thread2, NULL);

  // Wait for threads to finish
  pthread_join(thread1, &ret);
  if (ret != 0)
    return 0;
  pthread_join(thread2, &ret);
  if (ret != 0)
    return 0;

  // ABA is detected if the stack head is B
  if (stack_pop(stack, &node) != 0)
    return 0;

  if (node == B) {
    printf("The head of the stack is B although it should be C after the sequence: \n"
           " pop -> pop -> push A -> pop.\n");
  }

  return node == B;
}

// We test here the CAS function
struct thread_test_cas_args
{
  int id;
  size_t* counter;
  pthread_mutex_t *lock;
};
typedef struct thread_test_cas_args thread_test_cas_args_t;

void*
thread_test_cas(void* arg)
{
  thread_test_cas_args_t *args = (thread_test_cas_args_t*) arg;
  int i;
  size_t old, local;

  for (i = 0; i < MAX_PUSH_POP; i++)
    {
      do {
        old = *args->counter;
        local = old + 1;
      } while (cas(args->counter, old, local) != old);
    }

  return NULL;
}

int
test_cas()
{
#if 1
  pthread_attr_t attr;
  pthread_t thread[NB_THREADS];
  thread_test_cas_args_t args[NB_THREADS];
  pthread_mutexattr_t mutex_attr;
  pthread_mutex_t lock;

  size_t counter;

  int i, success;

  counter = 0;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 
  pthread_mutexattr_init(&mutex_attr);
  pthread_mutex_init(&lock, &mutex_attr);

  for (i = 0; i < NB_THREADS; i++)
    {
      args[i].id = i;
      args[i].counter = &counter;
      args[i].lock = &lock;
      pthread_create(&thread[i], &attr, &thread_test_cas, (void*) &args[i]);
    }

  for (i = 0; i < NB_THREADS; i++)
    {
      pthread_join(thread[i], NULL);
    }

  success = counter == (size_t)(NB_THREADS * MAX_PUSH_POP);

  if (!success)
    {
      printf("Got %ti, expected %i\n", counter, NB_THREADS * MAX_PUSH_POP);
    }

  assert(success);

  return success;
#else
  int a, b, c, *a_p, res;
  a = 1;
  b = 2;
  c = 3;

  a_p = &a;

  printf("&a=%X, a=%d, &b=%X, b=%d, &c=%X, c=%d, a_p=%X, *a_p=%d; cas returned %d\n", (unsigned int)&a, a, (unsigned int)&b, b, (unsigned int)&c, c, (unsigned int)a_p, *a_p, (unsigned int) res);

  res = cas((void**)&a_p, (void*)&c, (void*)&b);

  printf("&a=%X, a=%d, &b=%X, b=%d, &c=%X, c=%d, a_p=%X, *a_p=%d; cas returned %X\n", (unsigned int)&a, a, (unsigned int)&b, b, (unsigned int)&c, c, (unsigned int)a_p, *a_p, (unsigned int)res);

  return 0;
#endif
}

#if MEASURE == 1
static void*
thread_test_performance_push(void* arg)
{
  int i;

  for (i = 0; i < MAX_PUSH_POP/NB_THREADS; i++)
    {
      int node_index = __sync_fetch_and_add(&next_node, 1);
      stack_node_t *node = stack_nodes[node_index];
      node->data = &data;
      if (stack_push(stack, node) != 0)
        return (void*)-1;
    }

  return (void*)0; 
}
#elif MEASURE == 2
static void*
thread_test_performance_pop(void* arg)
{
  int i;

  for (i = 0; i < MAX_PUSH_POP/NB_THREADS; i++)
    {
      stack_node_t *node;
      if (stack_pop(stack, &node) != 0)
        return (void*)-1;
    }

  return (void*)0; 
}
#endif

// Stack performance test
#if MEASURE != 0
struct stack_measure_arg
{
  int id;
};
typedef struct stack_measure_arg stack_measure_arg_t;

struct timespec t_start[NB_THREADS], t_stop[NB_THREADS], start, stop;
#endif

int
main(int argc, char **argv)
{
setbuf(stdout, NULL);
// MEASURE == 0 -> run unit tests
#if MEASURE == 0
  test_init();

  test_run(test_cas);

  test_run(test_push_safe);
  test_run(test_pop_safe);
  test_run(test_aba);

  test_finalize();
#else
  // Run performance tests
  int i;
  stack_measure_arg_t arg[NB_THREADS];  

  test_setup();

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (i = 0; i < NB_THREADS; i++)
    {
      arg[i].id = i;
      (void)arg[i].id; // Makes the compiler to shut up about unused variable arg
      // Run push-based performance test based on MEASURE token
#if MEASURE == 1
      clock_gettime(CLOCK_MONOTONIC, &t_start[i]);
      // Push MAX_PUSH_POP times in parallel
      run_test_function(&thread_test_performance_push);
      clock_gettime(CLOCK_MONOTONIC, &t_stop[i]);
#else
      // Run pop-based performance test based on MEASURE token

      clock_gettime(CLOCK_MONOTONIC, &t_start[i]);
      // Pop MAX_PUSH_POP times in parallel
      run_test_function(&thread_test_performance_pop);
      clock_gettime(CLOCK_MONOTONIC, &t_stop[i]);
#endif
    }

  // Wait for all threads to finish
  clock_gettime(CLOCK_MONOTONIC, &stop);

  // Print out results
  for (i = 0; i < NB_THREADS; i++)
    {
      printf("%i %i %li %i %li %i %li %i %li\n", i, (int) start.tv_sec,
          start.tv_nsec, (int) stop.tv_sec, stop.tv_nsec,
          (int) t_start[i].tv_sec, t_start[i].tv_nsec, (int) t_stop[i].tv_sec,
          t_stop[i].tv_nsec);
    }
#endif

  return 0;
}
