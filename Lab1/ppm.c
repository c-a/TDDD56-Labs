#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>

#include "ppm.h"

#define MAX_LINE_LENGTH 70
#define MAX_PIXEL_LENGTH 12

/**
 * Returns a width multiple of alignment, useful to align row for performance reason or comply with OpenGL
 */
int
ppm_align(int value, int alignment)
{
	if (value % alignment != 0)
	{
		return (value / alignment + 1) * alignment;
	}
	else
	{
		return value;
	}
}

/**
 * Converts an xy coordinate to the index of an unidimensional
 * array according to the size of the picture given
 */
color_t*
coord_to_ptr(struct ppm * picture, int x, int y)
{
	return (color_t*)(((char*)picture->data) + ppm_align(picture->width * sizeof(color_t), PPM_ALIGNMENT) * y + sizeof(color_t) * x);
}

/**
 * Saves a picture to the specified filename using the ASCII ppm format
 */
void
ppm_save(struct ppm * picture, char * filename)
{
	FILE * ppm_file; // File to write ppm picture content to.
	int i, j; // iterators
	int length; // counts the number of characters written on a line
	color_t color;

	ppm_file = fopen(filename, "w");

	// Write header
	length = fprintf(ppm_file, "P3 %u %u 255 ", picture->width, picture->height);

	// Iterate through the pixels of the picture
	for (i = 0; i < picture->height; i++)
	{
		for (j = 0; j < picture->width; j++)
		{
			// write to the file all RGB components of the color to be written
			color = *coord_to_ptr(picture, j, i);
			length += fprintf(ppm_file, "%d %d %d ", color.red,
			    color.green,
			    color.blue);

			// Makes sure no line in the PPM file exceeds 70 characters
			if (length > MAX_LINE_LENGTH - MAX_PIXEL_LENGTH)
			{
				length = 0;
				fprintf(ppm_file, "\n");
			}
		}
	}

	// Close the file
	fclose(ppm_file);
}

struct ppm *
ppm_alloc(int width, int height)
{
	struct ppm * picture;
	picture = malloc(sizeof(struct ppm));
	if (picture != NULL)
	{
		picture->height = height;
		picture->width = width;
		picture->data = malloc(ppm_align(sizeof(color_t) * width, PPM_ALIGNMENT) * height);
	}

	return picture;
}

void
ppm_free(struct ppm * picture)
{
	free(picture->data);
	picture->data = NULL;
	free(picture);
}

void
ppm_write(struct ppm * picture, int x, int y, color_t color)
{
	*coord_to_ptr(picture, x, y) = color;
}

color_t
ppm_read(struct ppm * picture, int x, int y)
{
	color_t color;

	color = *coord_to_ptr(picture, x, y);

	return color;
}

void
ppm_printf(struct ppm * ppm)
{
	int i, j;
	color_t color;

	for(i = 0; i < ppm->height; i++)
	{
		for(j = 0; j < ppm->width; j++)
			{
				color = ppm_read(ppm, j, i);
				printf("(%'3d, %'3d) %'3d %'3d %'3d;", j, i, color.red, color.green, color.blue);
			}
		printf("\n");
	}
}
