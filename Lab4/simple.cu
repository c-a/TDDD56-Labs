// Simple CUDA example by Ingemar Ragnemalm 2009. Simplest possible?
// Assigns every element in an array with its index.

// nvcc simple.cu -L /usr/local/cuda/lib -lcudart -o simple

#include <stdio.h>

const int N = 16; 
const int blocksize = 16; 

__global__ 
void simple(float *c) 
{
	c[threadIdx.x] = sqrt(c[threadIdx.x]);
}

int main()
{
	float *c = new float[N];	
	float *cd;
	const int size = N*sizeof(float);
  int i;
	
	cudaMalloc( (void**)&cd, size );

  /* Fill c with data */
  for (i = 0; i < N; i++)
    c[i] = i*i;

  /* Upload c to cd */
  cudaMemcpy(cd, c, size, cudaMemcpyHostToDevice);
 
	dim3 dimBlock( blocksize, 1 );
	dim3 dimGrid( 1, 1 );
	simple<<<dimGrid, dimBlock>>>(cd);
	cudaThreadSynchronize();
	cudaMemcpy( c, cd, size, cudaMemcpyDeviceToHost ); 
	cudaFree( cd );
	
	for (int i = 0; i < N; i++)
		printf("%f:%f ", c[i], sqrtf(i*i));
	printf("\n");
	delete[] c;
	printf("done\n");
	return EXIT_SUCCESS;
}
