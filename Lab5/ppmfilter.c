// Flat 5x5 filter, C version

#include <stdio.h>
#include "readppm.h"
#ifdef __APPLE__
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
#else
	#include <GL/glut.h>
#endif

void Draw()
{
	unsigned char *image, *out;
	int n, m, i, j, k, l, sumx, sumy, sumz;
	
	image = readppm("maskros512.ppm", &n, &m);
	out = malloc(n*m*3);
	
// Skip edges (cheat edge tests)
	for(i=2;i<m-2;i++)
		for(j=2;j<n-2;j++)
		{
			// Filter kernel
			sumx=0;sumy=0;sumz=0;
			for(k=-2;k<3;k++)
				for(l=-2;l<3;l++)
				{
					sumx += image[((i+k)*n+(j+l))*3+0];
					sumy += image[((i+k)*n+(j+l))*3+1];
					sumz += image[((i+k)*n+(j+l))*3+2];
				}
			out[(i*n+j)*3+0] = sumx/25;
			out[(i*n+j)*3+1] = sumy/25;
			out[(i*n+j)*3+2] = sumz/25;
		}
	
// Dump the whole picture onto the screen.	
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glRasterPos2f(-1, -1);
	glDrawPixels( n, m, GL_RGB, GL_UNSIGNED_BYTE, image );
	glRasterPos2i(0, -1);
	glDrawPixels( n, m, GL_RGB, GL_UNSIGNED_BYTE, out );
	glFlush();
}

// Main program, inits
int main( int argc, char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
	glutInitWindowSize( 1024, 512 );
	glutCreateWindow("Image filter on CPU");
	glutDisplayFunc(Draw);
	
	glutMainLoop();
}
