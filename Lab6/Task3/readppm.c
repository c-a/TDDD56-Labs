#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif
#include "readppm.h"

// By Ingemar Ragnemalm
// based on a ppm reader by Edward Angel,
// Added support for binary PPM (2002?)
// 2006: Fixed a bug that caused some files to end prematurely
// and added a PPM writer
// for making screen shots and other output (i.e. GPGPU uses)
// Added writeppm. (Writes in ASCII format only.)
// 2009: Added readppmtexture. (A writer for a texture would also be possible when needed.)
// Portable #include.
// 2010: Fixed a stupid bug that made non-square textures fail.
// Still crashes on non-power-of-two.
// 2010, march: Even more bug fixes, and now it is OK (for power of two).
// Checks for power of two to avoid crashes.
// 2010-03-15 A fix in writeppm

int writeppm(char *filename, int width, int height, unsigned char *data)
{
	FILE *fp;
	int error = 1;
	int i, h, v;

	if (filename != NULL)
	{
		fp = fopen(filename,"w");
		
		if (fp != NULL)
		{
			// Write PPM file
			// Header	
			fprintf(fp, "P3\n");
			fprintf(fp, "# written by Ingemars PPM writer\n");
			fprintf(fp, "%d %d\n", width, height);
			fprintf(fp, "%d\n", 255); // range
			
			// Data
			for (v = height-1; v >=0; v--)
			{
				for (h = 0; h < width; h++)
				{
					i = (width*v + h)*3; // assumes rgb, not rgba
					fprintf(fp, "%d %d %d ", data[i], data[i+1], data[i+2]);
				}
				fprintf(fp, "\n"); // range
			}
			
			if (fwrite("\n",sizeof(char),1,fp) == 1)
				error = 0; // Probable success
			fclose(fp);
		}
	}
	return(error);
}



unsigned char *readppm(char *filename, int *width, int *height)
{
	FILE *fd;
	int  k, nm;
	char c;
	int i,j;
	char b[100];
	float s;
	int red, green, blue;
	long numbytes, howmuch;
	int n;
	int m;
	char *image;
	
	fd = fopen(filename, "rb");
	if (fd == NULL)
	{
		printf("Could not open %s\n", filename);
		return NULL;
	}
	c = getc(fd);
	if (c=='P' || c=='p')
		c = getc(fd);
	
	if (c == '3')
	{
		printf("%s is a PPM file (plain text version)\n", filename);
		
		// NOTE: This is not very good PPM code! Comments are not allowed
		// except immediately after the magic number.
		c = getc(fd);
		if (c == '\n' || c == '\r') // Skip any line break and comments
		{
			c = getc(fd);
			while(c == '#') 
			{
				fscanf(fd, "%[^\n\r] ", b);
				printf("%s\n",b);
				c = getc(fd);
			}
			ungetc(c,fd); 
		}
		fscanf(fd, "%d %d %d", &n, &m, &k);
		
		printf("%d rows  %d columns  max value= %d\n",n,m,k);
		
		numbytes = n * m * 3;
		image = (char *) malloc(numbytes);
		if (image == NULL)
		{
			printf("Memory allocation failed!\n"); 
			return NULL;
		}
		for(i=m-1;i>=0;i--) for(j=0;j<n;j++) // Important bug fix here!
		{ // i = row, j = column
			fscanf(fd,"%d %d %d",&red, &green, &blue );
			image[(i*n+j)*3]=red * 255 / k;
			image[(i*n+j)*3+1]=green * 255 / k;
			image[(i*n+j)*3+2]=blue * 255 / k;
		}
	}
	else
	if (c == '6')
	{
		printf("%s is a PPM file (raw version)!\n", filename); 
		
		c = getc(fd);
		if (c == '\n' || c == '\r') // Skip any line break and comments
		{
			c = getc(fd);
			while(c == '#') 
			{
				fscanf(fd, "%[^\n\r] ", b);
				printf("%s\n",b);
				c = getc(fd);
			}
			ungetc(c,fd); 
		}
		fscanf(fd, "%d %d %d", &n, &m, &k);
		printf("%d rows  %d columns  max value= %d\n",m,n,k);
		c = getc(fd); // Skip the last whitespace
		
		numbytes = n * m * 3;
		image = (char *) malloc(numbytes);
		if (image == NULL)
		{
			printf("Memory allocation failed!\n"); 
			return NULL;
		}
		// Read and re-order as necessary
		for(i=m-1;i>=0;i--) for(j=0;j<n;j++) // Important bug fix here!
		{
			image[(i*n+j)*3+0]=getc(fd);
			image[(i*n+j)*3+1]=getc(fd);
			image[(i*n+j)*3+2]=getc(fd);
		}
	}
	else
	{
		printf("%s is not a PPM file!\n", filename); 
		return NULL;
	}
	
	printf("read image\n");
	
	*height = m;
	*width = n;
	return image;
}

// Convenient high-level call
GLint readppmtexture(char *filename, char dofilter, char dorepeat)
{
	int width, height, w, h;
	char *image;
	GLint tex;
	
	image = readppm(filename, &width, &height);
	
	// Check if power of 2
	w = 1;
	while (w < width) w = w << 1;
	h = 1;
	while (h < height) h = h << 1;
	if ((w > width) || (h > height))
	{
		printf("readppmtexture(): Only power of two texture size allowed.\n");
		printf("Try readppm() with mipmapping instead.\n");
		return 0;
	}
	
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glEnable(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE, image);
	if (dorepeat)
	{
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	}
	if (dofilter)
	{
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	}
	// free(image);
	
	return tex;
}
