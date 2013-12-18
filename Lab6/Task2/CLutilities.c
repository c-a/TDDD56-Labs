// CL utilities by Ingemar
// readFile is just grabbed from my shader utilities.

#include "CLutilities.h"


char* readFile(const char * filename)
{
	char * data;
	FILE *theFile;
	char c;
	long howMuch;
	
	// Get file length
	theFile = fopen(filename, "rb");
	if (theFile == NULL)
	{
		printf("%s not found\n", filename);
		return NULL;
	}
	howMuch = 0;
	c = 0;
	while (c != EOF)
	{
		c = getc(theFile);
		howMuch++;
	}
	fclose(theFile);

	printf("%ld bytes\n", howMuch);
	
	// Read it again
	data = (char *)malloc(howMuch);
	theFile = fopen(filename, "rb");
	fread(data, howMuch-1,1,theFile);
	fclose(theFile);
	data[howMuch-1] = 0;

//	printf("%s\n-----\n", data);
	printf("%s loaded from disk\n", filename);

	return data;
}

void printCLError(cl_int ciErrNum, int location)
{	
    if (ciErrNum != CL_SUCCESS)
    {
      switch (location)
      {
        case 0:
          printf("Error @ clGetPlatformIDs: ");
          break;
        case 1:
          printf("Error @ clGetDeviceIDs: ");
          break;
        case 2:
          printf("Error @ clCreateContext: ");
          break;
        case 3:
          printf("Error @ clGetDeviceInfo: ");
          break;
        case 4:
          printf("Error @ clCreateCommandQueue: ");
          break;
        case 5:
          printf("Error @ clCreateProgramWithSource: ");
          break;
        case 6:
          printf("Error @ clCreateKernel: ");
          break;
        case 7:
          printf("Error @ clCreateBuffer: ");
          break;
        case 8:
          printf("Error @ clSetKernelArg: ");
          break;
        case 9:
          printf("Error @ clEnqueueNDRangeKernel: ");
          break;
        case 10:
          printf("Error @ clWaitForEvents: ");
          break;
        case 11:
          printf("Error @ clEnqueueReadBuffer: ");
          break;
        case 12:
          printf("Error @ clBuildProgram: ");
          break;
        default:
          printf("Error @ unknown location: ");
          break;
      }
      switch (ciErrNum)
      {
        case CL_INVALID_PROGRAM_EXECUTABLE:
          printf("CL_INVALID_PROGRAM_EXECUTABLE\n");
          break;
        case CL_INVALID_COMMAND_QUEUE:
          printf("CL_INVALID_COMMAND_QUEUE\n");
          break;
        case CL_INVALID_KERNEL:
          printf("CL_INVALID_KERNEL\n");
          break;
        case CL_INVALID_CONTEXT:
          printf("CL_INVALID_CONTEXT\n");
          break;
        case CL_INVALID_KERNEL_ARGS:
          printf("CL_INVALID_KERNEL_ARGS\n");
          break;
        case CL_INVALID_WORK_DIMENSION:
          printf("CL_INVALID_WORK_DIMENSION\n");
          break;
        case CL_INVALID_WORK_GROUP_SIZE:
          printf("CL_INVALID_WORK_GROUP_SIZE\n");
          break;
        case CL_INVALID_WORK_ITEM_SIZE:
          printf("CL_INVALID_WORK_ITEM_SIZE\n");
          break;
        case CL_INVALID_GLOBAL_OFFSET:
          printf("CL_INVALID_GLOBAL_OFFSET\n");
          break;
        case CL_OUT_OF_RESOURCES:
          printf("CL_OUT_OF_RESOURCES\n");
          break;
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
          printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
          break;
        case CL_INVALID_EVENT_WAIT_LIST:
          printf("CL_INVALID_EVENT_WAIT_LIST\n");
          break;
        case CL_OUT_OF_HOST_MEMORY:
          printf("CL_OUT_OF_HOST_MEMORY\n");
          break;
        case CL_INVALID_MEM_OBJECT:
          printf("CL_INVALID_MEM_OBJECT\n");
          break;
        case CL_INVALID_VALUE:
          printf("CL_INVALID_VALUE\n");
          break;
        case CL_INVALID_PROGRAM:
          printf("CL_INVALID_PROGRAM\n");
          break;
        case CL_INVALID_KERNEL_DEFINITION:
          printf("CL_INVALID_KERNEL_DEFINITION\n");
          break;
        case CL_INVALID_PLATFORM:
          printf("CL_INVALID_PLATFORM\n");
          break;
        case CL_INVALID_DEVICE_TYPE:
          printf("CL_INVALID_DEVICE_TYPE\n");
          break;
        case CL_DEVICE_NOT_FOUND:
          printf("CL_DEVICE_NOT_FOUND\n");
          break;
        
        default:
          printf("Error: Unknown error\n");
          break;
      }
      exit;
    }
}

