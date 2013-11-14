/*
 * gl_mandelbrot.h
 *
 *  Created on: 3 Sep 2012
 *  Copyright 2012 Nicolas Melot
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

#ifndef GL_MANDELBROT_H_
#define GL_MANDELBROT_H_

int gl_mandelbrot_start(struct mandelbrot_param*);
int gl_mandelbrot_init(int argc, char** argv);

#endif /* GL_MANDELBROT_H_ */
