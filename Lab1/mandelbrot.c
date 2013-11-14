#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>

#include "mandelbrot.h"
#include "ppm.h"

#ifdef MEASURE
#include <time.h>
// If we measure, we don't debug as assert() and printf() seriously affect performance
#undef DEBUG
#endif

// Disable assertion code if DEBUG is not defined
#ifndef DEBUG
#define NDEBUG
#endif

#if NB_THREADS > 0
#include <pthread.h>
#endif

color_t *color = NULL;

#if NB_THREADS > 0
// Compiled only when several threads are used
struct mandelbrot_thread
{
	int id;
#ifdef MEASURE
struct mandelbrot_timing timing;
#endif
};

int thread_stop;
pthread_barrier_t thread_pool_barrier;

pthread_t thread[NB_THREADS];
struct mandelbrot_thread thread_data[NB_THREADS];
#else
#ifdef MEASURE
struct mandelbrot_timing sequential;
#endif
#endif

#ifdef MEASURE
struct mandelbrot_timing **timing;
#endif

struct mandelbrot_param mandelbrot_param;

static int num_colors(struct mandelbrot_param* param)
{
	return param->maxiter + 1;
}

/**
 * Calculates if the complex number (Cre, Cim)
 * belongs to the Mandelbrot set
 *
 * @param Cre: Real part
 *
 * @param Cim: Imaginary part
 *
 * @return : MAXITER if (Cre, Cim) belong to the
 * mandelbrot set, else the number of iterations
 */
static int
is_in_Mandelbrot(float Cre, float Cim, int maxiter)
{
	int iter;
	float x = 0.0, y = 0.0, xto2 = 0.0, yto2 = 0.0, dist2;

	for (iter = 0; dist2 < 4 && iter <= maxiter; iter++)
	{
		y = x * y;
		y = y + y + Cim;
		x = xto2 - yto2 + Cre;
		xto2 = x * x;
		yto2 = y * y;

		dist2 = xto2 + yto2;
	}
	return iter;
}

static void
compute_chunk(struct mandelbrot_param *args)
{
	int i, j, val;
	float Cim, Cre;
	color_t pixel;

	// Iterate hrough lines
	for (i = args->begin_h; i < args->end_h; i++)
	{
		// Iterate through pixels in a line
		for (j = args->begin_w; j < args->end_w; j++)
		{
			// Convert the coordinate of the pixel to be calculated to both
			// real and imaginary parts of the complex number to be checked
			Cim = (float) i / args->height * (args->upper_i - args->lower_i)
			    + args->lower_i;
			Cre = (float) j / args->width * (args->upper_r - args->lower_r)
			    + args->lower_r;

			// Gets the value returned by is_in_mandelbrot() and scale it
			// from 0 to 255, or -1 if (Cre, Cim) is in the mandelbrot set.
			val = is_in_Mandelbrot(Cre, Cim, args->maxiter);

			// Change a negative value to 0 in val to make mandelbrot
			// elements to appear black in the final picture.
			pixel = val > args->maxiter ? args->mandelbrot_color : color[val
			    % num_colors(args)];

			ppm_write(args->picture, j, i, pixel);
		}
	}
}

/***** You may modify this portion *****/
#if NB_THREADS > 0
void
init_round(struct mandelbrot_thread *args)
{
	// Initialize or reinitialize here variables before any thread starts or restarts computation
}

/*
 * Each thread starts individually this function, where args->id give the thread's id from 0 to NB_THREADS
 */
void
parallel_mandelbrot(struct mandelbrot_thread *args, struct mandelbrot_param *parameters)
{
#if LOADBALANCE == 0
	// naive *parallel* implementation. Compiled only if LOADBALANCE = 0
#endif
#if LOADBALANCE == 1
	// Your load-balanced smarter solution. Compiled only if LOADBALANCE = 1
#endif
#if LOADBALANCE == 2
	// A second *optional* load-balancing solution. Compiled only if LOADBALANCE = 2
#endif
}
/***** end *****/
#else
void
sequential_mandelbrot(struct mandelbrot_param *parameters)
{
	// Define the region compute_chunk() has to compute
	// Entire height: from 0 to picture's height
	parameters->begin_h = 0;
	parameters->end_h = parameters->height;
	// Entire width: from 0 to picture's width
	parameters->begin_w = 0;
	parameters->end_w = parameters->width;

	// Go
	compute_chunk(parameters);
}
#endif

// Thread code, compiled only if we use threads
#if NB_THREADS > 0
static void *
run_thread(void * buffer)
{
	struct mandelbrot_thread *args;
	args = (struct mandelbrot_thread*) buffer;
	struct mandelbrot_param param;

	// Notify the master this thread is spawned
	pthread_barrier_wait(&thread_pool_barrier);

	// Reinitialize vars before any thread restart
	init_round(args);

	// Wait for the first computation order
	pthread_barrier_wait(&thread_pool_barrier);

	// Fetch the latest parameters
	param = mandelbrot_param;

	while (thread_stop == 0)
	{
#ifdef MEASURE
		clock_gettime(CLOCK_MONOTONIC, &args->timing.start);
#endif

		parallel_mandelbrot(args, &param);

#ifdef MEASURE
		clock_gettime(CLOCK_MONOTONIC, &args->timing.stop);
#endif

		// Notify the master thread of completion
		pthread_barrier_wait(&thread_pool_barrier);

		// Reinitialize vars before any thread restart
		init_round(args);

		// Wait for the next work signal
		pthread_barrier_wait(&thread_pool_barrier);
	
		// Fetch the latest parameters
		param = mandelbrot_param;
	}

	// Notify the master thread of work completion
	pthread_barrier_wait(&thread_pool_barrier);

	return NULL;
}
#endif

void
init_ppm(struct mandelbrot_param* param)
{
	if(param->picture->data != NULL)
	{
		free(param->picture->data);
		param->picture->data = NULL;
	}

	param->picture->data = malloc(ppm_align(sizeof(color_t) * param->width, PPM_ALIGNMENT) * param->height);
	param->picture->height = param->height;
	param->picture->width = param->width;
}

void
update_colors(struct mandelbrot_param* param)
{
	// Gradient color
	color_t start, stop;
	// Other control variables
	int i;

	if(color != NULL)
	{
		free(color);
	}
	color = malloc(sizeof(color_t) * num_colors(param));

	// Start color
	start.red = 219;
	start.green = 57;
	start.blue = 0;

	// Stop color
	stop.red = 0;
	stop.green = 0;
	stop.blue = 0;

	// Initialize the color vector
	for (i = 0; i < num_colors(param); i++)
	{
		color[i].green = (stop.green - start.green) * ((double) i / num_colors(param)) + start.green;
		color[i].red = (stop.red - start.red) * ((double) i / num_colors(param)) + start.red;
		color[i].blue = (stop.blue - start.blue) * ((double) i / num_colors(param)) + start.blue;
	}
}

void
init_mandelbrot(struct mandelbrot_param *param)
{
	// Initialize the picture container, but not its buffer
	param->picture = ppm_alloc(0, 0);
	param->picture->height = param->height;
	param->picture->width = param->width;

#if GLUT != 1
	// GLUT will do it when creating or resizing its window
	init_ppm(param);
#endif
	// initialize the color vector
	update_colors(param);

#if NB_THREADS > 0
	// Thread-based variant

	pthread_attr_t thread_attr;
	int i;

	// Initialise thread poll / master thread synchronisation
	pthread_barrier_init(&thread_pool_barrier, NULL, NB_THREADS + 1);

	// Initialize attributes
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

	// Enables thread running
	thread_stop = 0;

#ifdef MEASURE
	// Measuring record structures
	timing = malloc(sizeof(struct timing*) * NB_THREADS);
#endif

	// Create a thread pool
	for (i = 0; i < NB_THREADS; i++)
	{
		thread_data[i].id = i;

#ifdef MEASURE
		timing[i] = &thread_data[i].timing;
#endif

		// Check the good behavior or pthread_create; must be disabled while measuring for performance reasons
#ifdef DEBUG
		assert(pthread_create(&thread[i], &thread_attr, &run_thread, &thread_data[i]) == 0);
#else
		pthread_create(&thread[i], &thread_attr, &run_thread, &thread_data[i]);
#endif
	}

	// Wait for the thread to be fully spawned before returning
	pthread_barrier_wait(&thread_pool_barrier);
#else
#ifdef MEASURE
	// Measuring record structures
	timing = malloc(sizeof(struct timing*));
	timing[0] = &sequential;
#endif
#endif
}

#ifdef MEASURE
struct mandelbrot_timing**
#else
void
#endif
compute_mandelbrot(struct mandelbrot_param param)
{
#if NB_THREADS > 0
	mandelbrot_param = param;

	// Trigger threads' resume
	pthread_barrier_wait(&thread_pool_barrier);

	// Wait for the threads to be done
	pthread_barrier_wait(&thread_pool_barrier);
#else
#ifdef MEASURE
	clock_gettime(CLOCK_MONOTONIC, &sequential.start);
#endif

	sequential_mandelbrot(&param);

#ifdef MEASURE
	clock_gettime(CLOCK_MONOTONIC, &sequential.stop);
#endif
#endif

#ifdef MEASURE
	return timing;
#endif
}

void
destroy_mandelbrot(struct mandelbrot_param param)
{
#if NB_THREADS > 0
	int i;

	// Initiate a stop order and resume threads in the thread pool
	thread_stop = 1;
	compute_mandelbrot(param);

	// Wait for the threads to finish
	for (i = 0; i < NB_THREADS; i++)
	{
#ifdef DEBUG
		assert(pthread_join(thread[i], NULL) == 0);
#else
		pthread_join(thread[i], NULL);
#endif
	}

	pthread_barrier_destroy(&thread_pool_barrier);

#else
	// Sequential version, nothing to destroy
#endif

#ifdef MEASURE
	free(timing);
#endif

	free(color);
	ppm_free(param.picture);
}
