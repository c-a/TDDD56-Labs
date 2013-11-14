/*
 * mandelbrot_main.c
 *
 *  Created on: 5 Sep 2011
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

// Defined when calling the compiler
/*
#define MAXITER 256

#define WIDTH 500
#define HEIGHT 375

#define LOWER_R (-2)
#define UPPER_R (0.6)
#define LOWER_I (-1)
#define UPPER_I (1)
#define MANDELBROT_COLOR 0

#define GLUT 1
*/
#define NUMBER_COLORS (MAXITER / 2)

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "mandelbrot.h"
#include "gl_mandelbrot.h"

int
main(int argc, char ** argv)
{
  struct mandelbrot_param param;

  param.height = HEIGHT;
  param.width = WIDTH;
  param.lower_r = LOWER_R;
  param.upper_r = UPPER_R;
  param.lower_i = LOWER_I;
  param.upper_i = UPPER_I;
  param.maxiter = MAXITER;
  param.mandelbrot_color.red = (MANDELBROT_COLOR >> 16) & 255;
  param.mandelbrot_color.green = (MANDELBROT_COLOR >> 8) & 255;
  param.mandelbrot_color.blue = MANDELBROT_COLOR & 255;

  // Initializes the mandelbrot computation framework. Among other, spawns the threads in thread pool
  init_mandelbrot(&param);

#ifdef MEASURE
  struct mandelbrot_timing ** thread, global;
  clock_gettime(CLOCK_MONOTONIC, &global.start);

  thread = compute_mandelbrot(param);

  clock_gettime(CLOCK_MONOTONIC, &global.stop);
#elif GLUT == 1
  srand(time(NULL));
  gl_mandelbrot_init(argc, argv);
  gl_mandelbrot_start(&param);
#else
  compute_mandelbrot(param);
  ppm_save(param.picture, "mandelbrot.ppm");
#endif

#ifdef MEASURE
#if NB_THREADS > 0
  int i;

  for (i = 0; i < NB_THREADS; i++)
    {
      printf("%i %li %li %li %li %li %li %li %li\n", i + 1, thread[i]->start.tv_sec, thread[i]->start.tv_nsec, thread[i]->stop.tv_sec, thread[i]->stop.tv_nsec, global.start.tv_sec, global.start.tv_nsec, global.stop.tv_sec, global.stop.tv_nsec);
    }
#else
  printf("0 %li %li %li %li %li %li %li %li\n", thread[0]->start.tv_sec, thread[0]->start.tv_nsec, thread[0]->stop.tv_sec, thread[0]->stop.tv_nsec, global.start.tv_sec, global.start.tv_nsec, global.stop.tv_sec, global.stop.tv_nsec);
#endif
#endif

  // Final: deallocate structures
  destroy_mandelbrot(param);

  return EXIT_SUCCESS;
}
