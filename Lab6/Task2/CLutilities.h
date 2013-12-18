#ifndef __INGEMARS_CL_UTILITIES_
#define __INGEMARS_CL_UTILITIES_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

char* readFile(const char * filename);
void printCLError(cl_int ciErrNum, int location);

#endif

