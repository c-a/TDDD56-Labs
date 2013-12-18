// Laboration in OpenCL. By Jens Ogniewski and Ingemar Ragnemalm 2010-2011.
// Based on the matrix multiplication example by NVIDIA.

// standard utilities and system includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#ifdef __APPLE__
  #include <OpenCL/opencl.h>
  #include <GLUT/glut.h>
  #include <OpenGL/gl.h>
#else
  #include <CL/cl.h>
  #include <GL/glut.h>
#endif
#include "CLutilities.h"


// Size of data!
#define dataWidth 64
#define dataHeight 64


// global variables
static cl_context cxGPUContext;
static cl_command_queue commandQueue;
static cl_program cpSort;
static cl_kernel gpgpuSort;
static size_t noWG;

// Timing globals
struct timeval t_s_cpu, t_e_cpu,t_s_gpu, t_e_gpu;


unsigned int *generateRandomField(unsigned int seed, unsigned int length)
{
  unsigned int *field, i, rnd, initd[length];

  field = (unsigned int *)malloc(length*4);

  if (!field)
  {
    printf("\nerror allocating data.\n\n");
    return NULL;
  }

  srandom(seed);

  for (i=0; i<length; i++)
    initd[i]=0;

  for (i=0; i<length; i++)
  {
    rnd = (unsigned int)(random()%(length));
    while(initd[rnd]!=0)
    {
      if (rnd<(length-1)) rnd++;
      else rnd=0;
    }
    field[rnd]=i*256;
    initd[rnd]=1;
  } 
  return field;
}

// Rank sorting on CPU
void cpu_Sort(unsigned int *data, unsigned int length)
{
  unsigned int i, j, pos;
  unsigned int *indata;
  
  indata = malloc(length * sizeof(unsigned int));
  memcpy(indata, data, length * sizeof(unsigned int));
  
  /* sort by count */
  for (i = 0; i < length; i++) // For all elements
  {
    pos = 0;
    for (j = 0; j < length; j++) // For all other elements
    {
      if (indata[i] > indata[j])
        pos += 1;
    }
    data[pos] = indata[i];
  }
  free(indata);
}

int init_OpenCL()
{
  cl_int ciErrNum = CL_SUCCESS;
  size_t kernelLength;
  char *source;
  cl_device_id device;
  cl_platform_id platform;
  unsigned int no_plat;

  ciErrNum =  clGetPlatformIDs(1,&platform,&no_plat);
  printCLError(ciErrNum,0);

  //get the device
  ciErrNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
  printCLError(ciErrNum,1);
  
  // create the OpenCL context on the device
  cxGPUContext = clCreateContext(0, 1, &device, NULL, NULL, &ciErrNum);
  printCLError(ciErrNum,2);

  ciErrNum = clGetDeviceInfo(device,CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),&noWG,NULL);
  printCLError(ciErrNum,3);
  printf("maximum number of workgroups: %d\n", (int)noWG);
  
  // create command queue
  commandQueue = clCreateCommandQueue(cxGPUContext, device, 0, &ciErrNum);
  printCLError(ciErrNum,4);
  
  source = readFile("sort.cl");
  kernelLength = strlen(source);
  
  // create the program
  cpSort = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&source, 
                                                    &kernelLength, &ciErrNum);
  printCLError(ciErrNum,5);
    
  // build the program
  ciErrNum = clBuildProgram(cpSort, 0, NULL, NULL, NULL, NULL);
  if (ciErrNum != CL_SUCCESS)
  {
    // write out the build log, then exit
    char cBuildLog[10240];
    clGetProgramBuildInfo(cpSort, device, CL_PROGRAM_BUILD_LOG, 
                          sizeof(cBuildLog), cBuildLog, NULL );
    printf("\nBuild Log:\n%s\n\n", (char *)&cBuildLog);
    return -1;
  }
  
  gpgpuSort = clCreateKernel(cpSort, "sort", &ciErrNum);
  printCLError(ciErrNum,6);
  
  //Discard temp storage
  free(source);
  
  return 0;
}

int gpu_Sort(unsigned int *data, unsigned int length)
{
  cl_int ciErrNum = CL_SUCCESS;
  size_t localWorkSize, globalWorkSize;
  cl_mem input, output;
  printf("GPU sorting.\n");
  
  input = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, length * sizeof(unsigned int), data, &ciErrNum);
  printCLError(ciErrNum,7);
  output = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, length * sizeof(unsigned int), NULL, &ciErrNum);
  printCLError(ciErrNum,7);

    if (length<512) localWorkSize  = length;
    else            localWorkSize  = 512;
    globalWorkSize = length;

    // set the args values
    ciErrNum  = clSetKernelArg(gpgpuSort, 0, sizeof(cl_mem),  (void *) &input);
    ciErrNum |= clSetKernelArg(gpgpuSort, 1, sizeof(cl_mem),  (void *) &output);
    ciErrNum |= clSetKernelArg(gpgpuSort, 2, sizeof(cl_uint), (void *) &length);
    printCLError(ciErrNum,8);

    gettimeofday(&t_s_gpu, NULL);
    
    cl_event event;
    ciErrNum = clEnqueueNDRangeKernel(commandQueue, gpgpuSort, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, &event);
    printCLError(ciErrNum,9);
    
    clWaitForEvents(1, &event); // Synch
    gettimeofday(&t_e_gpu, NULL);
    printCLError(ciErrNum,10);

  ciErrNum = clEnqueueReadBuffer(commandQueue, output, CL_TRUE, 0, length * sizeof(unsigned int), data, 0, NULL, &event);
    printCLError(ciErrNum,11);
    clWaitForEvents(1, &event); // Synch
  printCLError(ciErrNum,10);
    
  clReleaseMemObject(input);
  clReleaseMemObject(output);
  return ciErrNum;
}

void close_OpenCL()
{
  if (gpgpuSort) clReleaseKernel(gpgpuSort);
  if (cpSort) clReleaseProgram(cpSort);
  if (commandQueue) clReleaseCommandQueue(commandQueue);
  if (cxGPUContext) clReleaseContext(cxGPUContext);
}

// Computed data
unsigned int *data_cpu, *data_gpu;


GLuint texNum, texNum2;

////////////////////////////////////////////////////////////////////////////////
// main computation function
////////////////////////////////////////////////////////////////////////////////
void computeImages()
{
  const char *outputname_cpu = "task1_out_cpu.rbm";
  const char *outputname_gpu = "task1_out_gpu.rbm";
  unsigned int seed;
  int i, length = dataWidth * dataHeight; // SIZE OF DATA
  unsigned short int header[2];

  gettimeofday(&t_s_cpu, NULL);
  seed = (unsigned int)t_s_cpu.tv_usec;
  printf("\nseed: %u\n",seed);

  if (init_OpenCL()<0)
  {
    close_OpenCL();
    return;
  }

  data_cpu = generateRandomField(seed,length);
  data_gpu = (unsigned int *)malloc (length*sizeof(unsigned int));

  if ((!data_cpu)||(!data_gpu))
  {
    printf("\nError allocating data.\n\n");
    return;
  }
  
  for(i=0;i<length;i++)
    data_gpu[i]=data_cpu[i];
  
  gettimeofday(&t_s_cpu, NULL);
  cpu_Sort(data_cpu,length);
  gettimeofday(&t_e_cpu, NULL);

  gettimeofday(&t_s_gpu, NULL);
  gpu_Sort(data_gpu,length);
  gettimeofday(&t_e_gpu, NULL);

// For small data sets you may print out errors here.
//  for(i=0;i<length;i++)
//    if(data_gpu[i]!=data_cpu[i]) printf("error @ %u\n",i);

  printf("\n time needed: \nCPU: %i us\n",(int)(t_e_cpu.tv_usec-t_s_cpu.tv_usec + (t_e_cpu.tv_sec-t_s_cpu.tv_sec)*1000000));
  printf("\nGPU: %i us\n\n",(int)(t_e_gpu.tv_usec-t_s_gpu.tv_usec + (t_e_gpu.tv_sec-t_s_gpu.tv_sec)*1000000));

  header[0]=dataWidth;
  header[1]=dataHeight;

  close_OpenCL();

  return;
}


// Display images side by side
void Draw()
{
	int m = dataWidth;
	int n = dataHeight;
	
// Dump the whole picture onto the screen.	
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT );

    glTexImage2D(GL_TEXTURE_2D, 0, 4, m, n, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, data_cpu);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);

    // Draw polygon
    glBegin(GL_POLYGON);
    glColor3f(1, 1, 1);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0,-1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f( 0.0,-1.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f( 0.0, 1.0, 0.0);
    glEnd();

    glTexImage2D(GL_TEXTURE_2D, 0, 4, m, n, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, data_gpu);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);

    // Draw polygon
    glBegin(GL_POLYGON);
    glColor3f(1, 1, 1);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(0.0,-1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f( 1.0,-1.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f( 1.0, 1.0, 0.0);
    glEnd();
    
    glFlush();
}

// Main program, inits
int main( int argc, char** argv) 
{
	
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
	glutInitWindowSize( 1024, 512 );
	glutCreateWindow("OpenCL output on GL");
	glutDisplayFunc(Draw);
	
	computeImages();
	
	glutMainLoop();
}
