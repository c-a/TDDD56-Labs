#ifndef PPM_H
#define PPM_H

#define PPM_ALIGNMENT 4

typedef unsigned char gray;

struct color
{
	gray red;
	gray green;
	gray blue;
};
typedef struct color color_t;

struct ppm
{
	int height;
	int width;
	color_t * data;
};

void ppm_save(struct ppm *, char *);
struct ppm * ppm_alloc(int, int);
void ppm_free(struct ppm *);
void ppm_write(struct ppm *, int x, int y, color_t);
color_t ppm_read(struct ppm *, int x, int y);
void ppm_printf(struct ppm *);
int ppm_align(int, int);

#endif
