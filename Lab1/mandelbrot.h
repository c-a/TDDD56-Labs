/*
 * mandelbrot.h
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

#include "ppm.h"

#ifndef MANDELBROT_H_
#define MANDELBROT_H_

struct mandelbrot_param
{
  int height, width, maxiter;
  color_t mandelbrot_color;
  int begin_h, end_h, begin_w, end_w;
  float lower_r, upper_r, lower_i, upper_i;
  struct ppm * picture;
};

#ifdef MEASURE
struct mandelbrot_timing
{
  // Monitors general algorithm start and stop time
  struct timespec start, stop;
};

struct mandelbrot_timing**
compute_mandelbrot(struct mandelbrot_param);
#else
void compute_mandelbrot(struct mandelbrot_param);
#endif

void init_mandelbrot(struct mandelbrot_param*);
void destroy_mandelbrot();

void init_ppm(struct mandelbrot_param*);
void update_colors(struct mandelbrot_param*);

#endif /* MANDELBROT_H_ */
