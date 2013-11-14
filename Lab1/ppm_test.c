#include <stdio.h>

#include "ppm.h"

#define WIDTH 5
#define HEIGHT 5

int
main(int argc, char ** argv)
{
	struct ppm * picture;
	color_t pixel;
	int i, j;

	picture = ppm_init(WIDTH, HEIGHT);

	for(i = 0; i < WIDTH; i++)
	{
		for(j = 0; j < HEIGHT; j++)
		{
			pixel.red = (float)i * j / (WIDTH * HEIGHT) * 255;
			//i == 2 && j == 2 ? 255 : 0;
			pixel.green = 0;
			pixel.blue = pixel.green;

			ppm_write(picture, i, j, pixel);
		}
	}

	ppm_save(picture, "picture.ppm");

	return 0;
}

