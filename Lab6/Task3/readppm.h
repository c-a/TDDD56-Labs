#ifndef READ_PPM
#define READ_PPM

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

int writeppm(char *filename, int width, int height, unsigned char *data);
unsigned char *readppm(char *filename, int *width, int *height);
GLint readppmtexture(char *filename, char dofilter, char dorepeat);

#endif
