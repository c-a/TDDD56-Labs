
#include <stdio.h>
#include "readppm.c"
#ifdef __APPLE__
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
#else
	#include <GL/glut.h>
#endif

static const int nBlocks = 32;
static const int outputPerBlock = 512 / nBlocks;
static const int threadsPerBlock = outputPerBlock + 4;

__constant__ int kernel[5*5];

__global__ void filter(unsigned char *image, unsigned char *out, int n, int m)
{
    __shared__ unsigned char S[threadsPerBlock*threadsPerBlock*3];
	int i = blockIdx.y * outputPerBlock + (threadIdx.y - 2);
	int j = blockIdx.x * outputPerBlock + (threadIdx.x - 2);

// printf is OK under --device-emulation
//  printf("%d %d %d %d\n", i, j, n, m);

  if (i < 0 || j < 0 || i >= n || j >= m)
    return;

  //printf("%d %d\n", i, j);
  S[(threadIdx.y*threadsPerBlock + threadIdx.x)*3+0] = image[(i*n+j)*3+0];
  S[(threadIdx.y*threadsPerBlock + threadIdx.x)*3+1] = image[(i*n+j)*3+1];
  S[(threadIdx.y*threadsPerBlock + threadIdx.x)*3+2] = image[(i*n+j)*3+2];

  __syncthreads();

  if ((threadIdx.x >= 2 && threadIdx.x < (threadsPerBlock - 2)) &&
      (threadIdx.y >= 2 && threadIdx.y < (threadsPerBlock - 2)))
  {
      int y = threadIdx.y;
      int x = threadIdx.x;

      if (i <= 1 || i >= (n-2) || j <= 1 || j >= (m-2)) {
	      out[(i*n+j)*3+0] = S[(y*threadsPerBlock+x)*3+0];
	      out[(i*n+j)*3+1] = S[(y*threadsPerBlock+x)*3+1];
	      out[(i*n+j)*3+2] = S[(y*threadsPerBlock+x)*3+2];
      }
      else {
      	int sumx, sumy, sumz, k, l;
   			// Filter kernel
		    sumx=0;sumy=0;sumz=0;
		    for(k=-2;k<3;k++)
			    for(l=-2;l<3;l++)
			    {
            int kernelIndex = (k+2)*5 + (l+2);
            sumx += kernel[kernelIndex]*S[((y+k)*threadsPerBlock+(x+l))*3+0];
            sumy += kernel[kernelIndex]*S[((y+k)*threadsPerBlock+(x+l))*3+1];
            sumz += kernel[kernelIndex]*S[((y+k)*threadsPerBlock+(x+l))*3+2];
			    }

		    out[(i*n+j)*3+0] = sumx/256;
		    out[(i*n+j)*3+1] = sumy/256;
		    out[(i*n+j)*3+2] = sumz/256;
     }
  }
}


// Compute CUDA kernel and display image
void Draw()
{
	unsigned char *image, *out;
	int n, m;
	unsigned char *dev_image, *dev_out;
  cudaEvent_t startEvent, endEvent;
  float time;
	
	image = readppm("maskros512.ppm", &n, &m);
	out = (unsigned char*) malloc(n*m*3);
	
	cudaMalloc( (void**)&dev_image, n*m*3);
	cudaMalloc( (void**)&dev_out, n*m*3);
	cudaMemcpy( dev_image, image, n*m*3, cudaMemcpyHostToDevice);
	
  int kernel_[] = {
    1,  4,  6,  4, 1,
    4, 16, 24, 16, 4,
    6, 24, 36, 24, 6,
    4, 16, 24, 16, 4,
    1,  4,  6,  4, 1
  };

  cudaMemcpyToSymbol(kernel, kernel_, 5*5*sizeof(int)); 

	dim3 dimBlock( threadsPerBlock, threadsPerBlock );
	dim3 dimGrid( nBlocks, nBlocks );
	
  // Insert event before kernel has run
  cudaEventCreate(&startEvent);
  cudaEventRecord(startEvent);

	filter<<<dimGrid, dimBlock>>>(dev_image, dev_out, n, m);

  // Insert event after kernel has run
  cudaEventCreate(&endEvent);
  cudaEventRecord(endEvent);

  // Wait for event to finish
  cudaEventSynchronize(endEvent);

	cudaThreadSynchronize();
	
	cudaMemcpy( out, dev_out, n*m*3, cudaMemcpyDeviceToHost );
	cudaFree(dev_image);
	cudaFree(dev_out);
	
// Dump the whole picture onto the screen.	
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glRasterPos2f(-1, -1);
	glDrawPixels( n, m, GL_RGB, GL_UNSIGNED_BYTE, image );
	glRasterPos2i(0, -1);
	glDrawPixels( n, m, GL_RGB, GL_UNSIGNED_BYTE, out );
	glFlush();

  cudaEventElapsedTime(&time, startEvent, endEvent);
  printf("Time: %f milliseconds\n", time);
}

// Main program, inits
int main( int argc, char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
	glutInitWindowSize( 1024, 512 );
	glutCreateWindow("CUDA on live GL");
	glutDisplayFunc(Draw);
	
	glutMainLoop();
}
