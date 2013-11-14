/*
 * gl_mandelbrot.c
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

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

#include <time.h>

#include "ppm.h"
#include "mandelbrot.h"

#define MOV_MAX 40
#define REFRESH_FREQ 40
#define PAUSE_TIME 1000

#define OFFSET_X_MIN (-2)
#define OFFSET_X_MAX (-0.6)
#define OFFSET_Y_MIN (-1)
#define OFFSET_Y_MAX (1)
#define SCALE_MIN 0.07
#define SCALE_MAX 0.1

#define RESOLUTION_X 10000
#define RESOLUTION_Y 10000
#define RESOLUTION_S 10000

#define STEEPNESS 1

struct coord
{
	double x, y;
};
typedef struct coord coord_t;

struct position
{
	double min_r, max_r, min_i, max_i;
};
typedef struct position position_t;

struct ppm* ppm;
struct mandelbrot_param draw_param;

int mouse_button_0, mouse_button_1, bounce, print_help;
double scale, speed;
coord_t mouse_grab;

char perf_caption[256], window_caption[256];

coord_t scale_ori;

position_t movement[MOV_MAX + 1], destination[] =
	{
		{ -0.77471, -0.77467, -0.12424, -0.12421 }, // Spiral
		    { -0.66048, -0.66044, -0.44982, -0.44985 }, // Star
		    { -1.26273, -1.26269, -0.40842, -0.40839 }, // Mini-mandelbrot
		    { -0.81847, -0.81826, -0.18433, -0.18417 }, // Black hole
	  };
int read = 0, write = 0, last_op = 0;

void
gl_mandelbrot_init(int argc, char** argv)
{
	glutInit(&argc, argv);
}

static coord_t
to_set(coord_t coord)
{
	coord_t set;

	set.x = coord.x * (draw_param.upper_r - draw_param.lower_r)
	    / draw_param.width;
	set.y = coord.y * (draw_param.upper_i - draw_param.lower_i)
	    / draw_param.height;

	return set;
}

static int
update_position(position_t position)
{
	// Fix the display ratio if the window changed aspect ratio in the middle of animation
	double ratio, extra_r;

	// Get the desired reduction ratio
	ratio = (draw_param.upper_r - draw_param.lower_r) / (draw_param.upper_i
	    - draw_param.lower_i);

	// Reduce current display by x by the previously computed ratio, and compare it to the new position
	extra_r = ratio * (position.max_i - position.min_i) - (position.max_r
	    - position.min_r);
	position.min_r -= extra_r / 2;
	position.max_r += extra_r / 2;

	if (draw_param.lower_r == position.min_r && draw_param.upper_r
	    == position.max_r && draw_param.lower_i == position.min_i
	    && draw_param.upper_i == position.max_i)
		return 0;
	else
	{
		draw_param.lower_r = position.min_r;
		draw_param.upper_r = position.max_r;
		draw_param.lower_i = position.min_i;
		draw_param.upper_i = position.max_i;

		return 1;
	}
}

static void
fifo_init()
{
	read = 0;
	write = 0;
	last_op = 0;
}

static int
fifo_empty()
{
	return read == write && last_op == 0;
}

static int
fifo_full()
{
	return read == write && last_op == 1;
}

static int
push(position_t pos)
{
	if (!fifo_full())
	{
		movement[write] = pos;
		write = (write + 1) % (MOV_MAX + 1);
		last_op = 1;

		return 1;
	}
	else
	{
		return 0;
	}
}

static int
pop(position_t *pos)
{
	if (!fifo_empty())
	{
		*pos = movement[read];
		read = (read + 1) % (MOV_MAX + 1);
		last_op = 0;

		return 1;
	}
	else
	{
		return 0;
	}
}

static position_t
build_position(double min_r, double max_r, double min_i, double max_i)
{
	position_t new;

	new.min_r = min_r;
	new.max_r = max_r;
	new.min_i = min_i;
	new.max_i = max_i;

	return new;
}

double bell_surface;
static double
bell(double x)
{
	return (1 / (pow((x * (1 << (STEEPNESS)) - (1 << (STEEPNESS - 1))), 2) + 1));
}

static void
play_movement(int value)
{
	double offset_x, offset_y, offset_z;
	position_t step, dest;
	//step = build_position(offsetX, offsetY, scale);

	if (pop(&step))
	{
		// There is a position to process, proceed
		if (update_position(step))
			// Need to refresh display
			glutPostRedisplay();

		//printf_position(build_position(offsetX, offsetY, scale));

		// Restart
		if (bounce)
		{
			glutTimerFunc(REFRESH_FREQ, play_movement, 0);
		}
		else
			fifo_init();
	}
	else
	{
		// No more position, add a new batch;
		double new_x, new_y, new_scale, bell_inc, dist_2d, dist_v, dist_3d_sq,
		    dist_3d, new_z, diam;
		int i;

		// Pick a random destination among predefined ones
		dest = destination[random() % (sizeof(destination) / sizeof(position_t))];
		new_x = (dest.max_r + dest.min_r) / 2;
		new_y = (dest.max_i + dest.min_i) / 2;
		new_scale = (dest.max_r - dest.min_r) / scale_ori.x;

		// Convert current position to a 3d point
		offset_x = (draw_param.upper_r + draw_param.lower_r) / 2;
		offset_y = (draw_param.upper_i + draw_param.lower_i) / 2;
		offset_z = (draw_param.upper_r - draw_param.lower_r) / scale_ori.x;

		// calculate 2d and 3d distances between current and target point
		// 2D distance squared (no square root'd, as the square is used later)
		dist_2d = powf(new_x - offset_x, 2) + powf(new_y - offset_y, 2);
		// Vertical distance
		dist_v = new_scale - offset_z;
		// 3D distance
		dist_3d_sq = dist_2d + pow(dist_v, 2);
		dist_3d = sqrt(dist_3d_sq);
		// Fix 2D distance with square root
		dist_2d = sqrt(dist_2d);
		diam = dist_3d * dist_3d / dist_2d;

		bell_inc = 0;
		for (i = 0; i < MOV_MAX; i++)
		{
			// Progress in distance: faster and faster then less and less fast (bell curve)
			bell_inc += bell((double) i / MOV_MAX) / bell_surface;
			if (new_scale > offset_z)
			{
				new_z = sin(bell_inc * dist_2d / diam * M_PI);// * dist_2d;
			}
			else
			{
				//printf("%f\n", (bell_inc * dist_2d + (diam - dist_2d)) / diam);
				new_z = sin((bell_inc * dist_2d + (diam - dist_2d)) / diam * M_PI);// * dist_2d;
			}

			step = build_position(offset_x + ((new_x - offset_x) * bell_inc) - new_z
			    * scale_ori.x / 2, offset_x + ((new_x - offset_x) * bell_inc) + new_z
			    * scale_ori.x / 2, offset_y + ((new_y - offset_y) * bell_inc) - new_z
			    * scale_ori.y / 2, offset_y + ((new_y - offset_y) * bell_inc) + new_z
			    * scale_ori.y / 2);

			if (!push(step))
			{
				fprintf(stderr, "Error while queueing jumping points\n");
				exit(1);
			}
		}
		if (!push(dest))
		{
			fprintf(stderr, "Error while queueing the final jumping points\n");
			exit(1);
		}

		// Play this move after pause
		glutTimerFunc(PAUSE_TIME, play_movement, time(NULL));
	}
}

static void
print_str(void *font, const char *string)
{
	int len, i;

	len = (int) strlen(string);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, string[i]);
}

static double
WallClockTime()
{
	struct timeval t;
	gettimeofday(&t, NULL);

	return t.tv_sec + t.tv_usec / 1000000.0;
}

static void
draw()
{
	// Exits if window or scale parameters are incorrect
	if(isnan(draw_param.lower_r) || isnan(draw_param.upper_r) || isnan(draw_param.lower_i) || isnan(draw_param.upper_i))
	{
		fprintf(stderr, "Incorrect window parameters\n");
		exit(-1);
	}

	const double startTime = WallClockTime();
	compute_mandelbrot(draw_param);

	const double elapsedTime = WallClockTime() - startTime;
	const double sampleSec = draw_param.height * draw_param.width / elapsedTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glDrawPixels(ppm->width, ppm->height, GL_RGB, GL_UNSIGNED_BYTE, ppm->data);

	glColor3f(1.f, 1.f, 1.f);
	glRasterPos2i(4, 10);
	sprintf(perf_caption,
	    "Rendering time: %.3f secs (sample/sec: %.1fK; max iterations: %d)",
	    elapsedTime, sampleSec / 1000.f, draw_param.maxiter);
	print_str(GLUT_BITMAP_HELVETICA_18, perf_caption);

	glRasterPos2i(4, draw_param.height - 20);
	sprintf(window_caption,
	    "Cre in [%.5f; %.5f]; Cim in [%.5f; %.5f] (scale: %f), %s",
	    draw_param.lower_r, draw_param.upper_r, draw_param.lower_i,
	    draw_param.upper_i, (draw_param.upper_r - draw_param.lower_r)
	        / scale_ori.x, bounce ? "bouncing" : "static");
	print_str(GLUT_BITMAP_HELVETICA_18, window_caption);

	if (print_help)
	{
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-0.5, 639.5, -0.5, 479.5, -1.0, 1.0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.f, 0.f, 0.5f, 0.5f);
		glRecti(40, 40, 600, 440);

		glColor3f(1.f, 1.f, 1.f);
		glRasterPos2i(300, 420);
		print_str(GLUT_BITMAP_HELVETICA_18, "Help");

		glRasterPos2i(60, 390);
		print_str(GLUT_BITMAP_HELVETICA_18, "h - Toggle Help");
		glRasterPos2i(60, 300);
		print_str(GLUT_BITMAP_HELVETICA_18, "Left click + drag - move picture");
		glRasterPos2i(60, 270);
		print_str(GLUT_BITMAP_HELVETICA_18,
		    "Right click + drag up/down - unzoom/zoom");
		glRasterPos2i(60, 240);
		print_str(GLUT_BITMAP_HELVETICA_18, "+ - Increase max. iterations by 32");
		glRasterPos2i(60, 210);
		print_str(GLUT_BITMAP_HELVETICA_18, "- - Decrease max. iterations by 32");
		glRasterPos2i(60, 180);
		print_str(GLUT_BITMAP_HELVETICA_18, "b - Enable/disable bouncing");

		glDisable(GL_BLEND);

		glPopMatrix();
	}

	glutSwapBuffers();
}

static void
reshape_window(int width, int height)
{
	double extra_i, extra_r;

	extra_i = height * (double) (draw_param.upper_i - draw_param.lower_i)
	    / draw_param.height + draw_param.lower_i - draw_param.upper_i;
	extra_r = width * (double) (draw_param.upper_r - draw_param.lower_r)
	    / draw_param.width + draw_param.lower_r - draw_param.upper_r;

	draw_param.upper_i += extra_i / 2;
	draw_param.lower_i -= extra_i / 2;

	draw_param.upper_r += extra_r / 2;
	draw_param.lower_r -= extra_r / 2;

	draw_param.height = height;
	draw_param.width = width;

	init_ppm(&draw_param);
	ppm = draw_param.picture;

	glViewport(0, 0, width, height);
	glLoadIdentity();
	glOrtho(-0.5f, width - 0.5f, -0.5f, height - 0.5f, -1.f, 1.f);

	glutPostRedisplay();
}

static void
mouse_button(int button, int state, int x, int y)
{
	if (button == 0)
	{
		if (state == GLUT_DOWN)
		{
			// Record start position
			mouse_grab.x = x;
			mouse_grab.y = y;
			mouse_button_0 = 1;
		}
		else if (state == GLUT_UP)
		{
			mouse_button_0 = 0;
		}
	}
	else if (button == 2)
	{
		if (state == GLUT_DOWN)
		{
			// Record start position
			mouse_grab.y = y;
			mouse_button_1 = 1;
		}
		else if (state == GLUT_UP)
		{
			mouse_button_1 = 0;
		}
	}
}

static void
mouse_motion(int x, int y)
{
	int need_refresh = 1;
	coord_t dist_win, dist_set;
	double medium_r, medium_i;

	if (mouse_button_0)
	{
		dist_win.x = x - mouse_grab.x;
		dist_win.y = y - mouse_grab.y;

		dist_set = to_set(dist_win);

		draw_param.lower_r -= dist_set.x * speed;
		draw_param.upper_r -= dist_set.x * speed;

		draw_param.lower_i += dist_set.y * speed;
		draw_param.upper_i += dist_set.y * speed;

		mouse_grab.x = x;
		mouse_grab.y = y;
	}
	else
	{
		if (mouse_button_1)
		{
			dist_win.y = y - mouse_grab.y;

			//printf("dis_2d: %f\n", dist_2d_set);

			scale = 1.0f - (2.f * dist_win.y / draw_param.height);

			// Do not zoom more than that
			medium_r = (draw_param.upper_r - draw_param.lower_r) / 2
			    + draw_param.lower_r;
			medium_i = (draw_param.upper_i - draw_param.lower_i) / 2
			    + draw_param.lower_i;

			if (!((((draw_param.upper_r - medium_r) <= 0.00002 && (draw_param.upper_i
			    - medium_i) <= 0.00002) && scale < 1) || (((draw_param.upper_r
			    - medium_r) >= 2 && (draw_param.upper_i - medium_i) >= 1.5) && scale
			    > 1)))
			{

				draw_param.upper_r = medium_r + (draw_param.upper_r - medium_r) * scale;
				draw_param.lower_r = medium_r + (draw_param.lower_r - medium_r) * scale;

				draw_param.upper_i = medium_i + (draw_param.upper_i - medium_i) * scale;
				draw_param.lower_i = medium_i + (draw_param.lower_i - medium_i) * scale;
			}
			else
			{
				need_refresh = 0;
			}

			mouse_grab.y = y;
		}
		else
		{
			need_refresh = 0;
		}
	}

	if (need_refresh)
	{
		glutPostRedisplay();
	}
}

static void
keyboard_key(unsigned char key, int x, int y)
{
	int need_refresh = 1;

	switch (key)
	{
	case 's':
		ppm_save(draw_param.picture, "mandelbrot.ppm");
		break;
	case 'b':
		bounce = !bounce;
		if (bounce)
			glutTimerFunc(0, play_movement, 0);
		break;
		break;
	case 27: /* Escape key */
	case 'q':
	case 'Q':
		exit(0);
		break;
	case '+':
		draw_param.maxiter += draw_param.maxiter < 1024 - 32 ? 32 : 0;
		update_colors(&draw_param);
		break;
	case '-':
		draw_param.maxiter -= draw_param.maxiter > 0 + 32 ? 32 : 0;
		update_colors(&draw_param);
		break;
	case ' ': /* Refresh display */
		break;
	case 'h':
		print_help = !print_help;
		break;
	default:
		need_refresh = 0;
		break;
	}

	if (need_refresh)
	{
		glutPostRedisplay();
	}
}

void
gl_mandelbrot_start(struct mandelbrot_param* parameters)
{
	int i;

	draw_param = *parameters;
	scale_ori.x = draw_param.upper_r - draw_param.lower_r;
	scale_ori.y = draw_param.upper_i - draw_param.lower_i;
	bounce = 0;
	print_help = 0;
	bounce = 1;

	ppm = draw_param.picture;
	scale = 1.0f;
	speed = 1.0f;

	fifo_init();
	bell_surface = 0;
	for (i = 0; i < MOV_MAX + 1; i++)
	{
		bell_surface += bell((double) i / MOV_MAX);
	}

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(draw_param.picture->width, draw_param.picture->height);
	glutCreateWindow("OpenGL Mandelbrot set");

	glutDisplayFunc(draw);
	glutMouseFunc(mouse_button);
	glutMotionFunc(mouse_motion);
	glutReshapeFunc(reshape_window);
	glutKeyboardFunc(keyboard_key);

	if (bounce)
	{
		glutTimerFunc(REFRESH_FREQ, play_movement, 0);
	}

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glutMainLoop();
}
