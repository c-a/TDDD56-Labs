// Matrix addition, CPU version
// gcc matrix_cpu.c -o matrix_cpu -std=c99

#include <stdio.h>
#include <stdlib.h>

#include "milli.h"

void add_matrix(float *a, float *b, float *c, int N)
{
	int index;
	
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			index = i + j*N;
			c[index] = a[index] + b[index];
		}
}

int main()
{
	const int N = 2048;

	float* a = malloc(sizeof(float)*N*N);
	float* b = malloc(sizeof(float)*N*N);
	float* c = malloc(sizeof(float)*N*N);

  int time;

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			a[i+j*N] = 10 + i;
			b[i+j*N] = (float)j / N;
		}
	
  ResetMilli();
	add_matrix(a, b, c, N);
  time = GetMilliseconds();
	
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

  printf("Time: %d milliseconds\n", time);
}
