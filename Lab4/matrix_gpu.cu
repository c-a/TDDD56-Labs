// Simple CUDA example by Ingemar Ragnemalm 2009. Simplest possible?
// Assigns every element in an array with its index.

// nvcc simple.cu -L /usr/local/cuda/lib -lcudart -o simple

#include <stdio.h>

static const int N = 2048;
static const int block_xDim = 256;
static const int block_yDim = 1;

__global__ 
void matrix_add(float* a, float* b, float *c) 
{
  uint i = (blockIdx.y * blockDim.y) + threadIdx.y;
  uint j = (blockIdx.x * blockDim.x) + threadIdx.x;
  
  if (i < N && j < N) {
    uint idx = i * N + j;
  	c[idx] = a[idx] + b[idx];
  }
}

int main()
{
	const int size = N*N*sizeof(float);

  cudaEvent_t startEvent, endEvent;
  float time;

	float* a = new float[N*N];
	float* b = new float[N*N];
  float* c = new float[N*N];
	float *ad, *bd, *cd;

  // Initialize a and b
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			a[i+j*N] = 10 + i;
			b[i+j*N] = (float)j / N;
		}
	
	cudaMalloc( (void**)&ad, size );
  cudaMalloc((void**)&bd, size);
  cudaMalloc((void**)&cd, size);

  /* Upload a to ad and b to bd */
  cudaMemcpy(ad, a, size, cudaMemcpyHostToDevice);
  cudaMemcpy(bd, b, size, cudaMemcpyHostToDevice);

  // Insert event before kernel has run
  cudaEventCreate(&startEvent);
  cudaEventRecord(startEvent);

	dim3 dimBlock( block_xDim, block_yDim);
	dim3 dimGrid( (N + block_xDim - 1)/block_xDim, (N + block_yDim - 1)/block_yDim);
	matrix_add<<<dimGrid, dimBlock>>>(ad, bd, cd);

  // Insert event after kernel has run
  cudaEventCreate(&endEvent);
  cudaEventRecord(endEvent);

  // Wait for event to finish
  cudaEventSynchronize(endEvent);

	cudaThreadSynchronize();
	cudaMemcpy( c, cd, size, cudaMemcpyDeviceToHost ); 

	cudaFree( ad );
	cudaFree( bd );
	cudaFree( cd );
#if 0
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%0.2f ", c[i+j*N]);
		}
		printf("\n");
	}
#endif

  cudaEventElapsedTime(&time, startEvent, endEvent);
  printf("Time: %f milliseconds\n", time);

	return EXIT_SUCCESS;
}
