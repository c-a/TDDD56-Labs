/*
 * non_blocking.h
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

#include <stddef.h>

#ifndef NON_BLOCKING_H_
#define NON_BLOCKING_H_

// Alias for pointer-sized CAS
#define cas(reg, old, new) cas(reg, old, new)

size_t cas(size_t*, size_t, size_t);

#if NON_BLOCKING == 1
#include <pthread.h>

size_t software_cas(size_t *, size_t, size_t, pthread_mutex_t*);
#endif

#endif /* NON_BLOCKING_H_ */
