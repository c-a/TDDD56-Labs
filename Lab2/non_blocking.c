/*
 * non_blocking.c
 *
 *  Created on: 19 Oct 2011
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

#include <stddef.h>

#if NON_BLOCKING == 0 || NON_BLOCKING == 1
#include <pthread.h>
#endif

#if NON_BLOCKING == 1
#include <string.h>

inline size_t
software_cas(size_t* reg, size_t oldval, size_t newval, pthread_mutex_t *lock)
{
  // Implement lock-based cas yourself
}
#endif

#if defined i386 || __i386__ || __i486__ || __i586__ || __i686__ || __i386 || __IA32__ || _M_IX86 || _M_IX86 || __X86__ || _X86_ || __THW_INTEL__ || __I86__ || __INTEL__ || __x86_64 || __x86_64__

inline size_t
cas(size_t* reg, size_t oldval, size_t newval)
{
  /*int out;

  __asm__("cmpxchgl %4, %2 \n\t" // CAS32(reg (%2), oldval(%%eax), newval(%4))
      "pushf \n\t" //
      "popl %%eax \n\t" //
      "andl $64, %%eax" // Gets result from ZF produced by CAS
      :"=a"(out), "=m"(*(int*)reg)
      :"m"(*(int*)reg), "a"(*(int*)oldval), "r"(*(int*)newval)
  );

  return out;*/

#if 0
  //return __sync_val_compare_and_swap(reg, oldval, newval);
#else
  asm volatile( "lock; cmpxchg %2, %1":
                "=a"(oldval):
                "m"(*reg), "r"(newval), "a"(oldval):
                "memory" );

  return oldval;
#endif
}

#else
#ifdef __GNUC__

inline size_t
cas(size_t* reg, size_t oldval, size_t newval)
{
  return __sync_val_compare_and_swap(reg, oldval, newval);
}

#else

#warning Unsupported compiler and architecture; no CAS available
inline size_t
cas(size_t* reg, size_t oldval, size_t newval)
{
  return 0;
}

#endif
#endif

