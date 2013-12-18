// Laboration in OpenCL. By Ingemar Ragnemalm 2012.
// Based on the matrix multiplication example by NVIDIA and our part 2.
// Part 3: Framework for wavelet transform. (Initially only inverts the image.)

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
#include "readppm.h"

// Size of data!
int dataWidth, dataHeight;

// global variables
static cl_context cxGPUContext;
static cl_command_queue commandQueue;
static cl_program myClProgram;
static cl_kernel myKernel;
static size_t noWG;

// Timing globals
struct timeval t_s_cpu, t_e_cpu,t_s_gpu, t_e_gpu;


// Process image on CPU
void cpu_WL(unsigned char *image, unsigned char *data, unsigned int length)
{
  unsigned int i;
  
  for (i = 0; i < length; i++) // For all elements
  {
    data[i] = 255 - image[i];
  }
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
  
  source = readFile("3.cl");
  kernelLength = strlen(source);
  
  // create the program
  myClProgram = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&source, 
                                                    &kernelLength, &ciErrNum);
  printCLError(ciErrNum,5);
  
  // build the program
  ciErrNum = clBuildProgram(myClProgram, 0, NULL, NULL, NULL, NULL);
  if (ciErrNum != CL_SUCCESS)
  {
    // write out the build log, then exit
    char cBuildLog[10240];
    clGetProgramBuildInfo(myClProgram, device, CL_PROGRAM_BUILD_LOG, 
                          sizeof(cBuildLog), cBuildLog, NULL );
    printf("\nBuild Log:\n%s\n\n", (char *)&cBuildLog);
    return -1;
  }
  
  myKernel = clCreateKernel(myClProgram, "kernelmain", &ciErrNum);
  printCLError(ciErrNum,6);
  
  //Discard temp storage
  free(source);
  
  return 0;
}

int gpu_WL(unsigned char *image, unsigned char *data, unsigned int length)
{
  cl_int ciErrNum = CL_SUCCESS;
  size_t localWorkSize, globalWorkSize;
  cl_mem in_data, out_data;
  printf("GPU processing.\n");
  
  in_data = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, length * sizeof(unsigned char), image, &ciErrNum);
    printCLError(ciErrNum,7);
  out_data = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, length * sizeof(unsigned char), NULL, &ciErrNum);
    printCLError(ciErrNum,7);

    if (length<512) localWorkSize  = length;
    else            localWorkSize  = 512;
    globalWorkSize = length;

    // set the args values
    ciErrNum  = clSetKernelArg(myKernel, 0, sizeof(cl_mem),  (void *) &in_data);
    ciErrNum |= clSetKernelArg(myKernel, 1, sizeof(cl_mem),  (void *) &out_data);
    ciErrNum |= clSetKernelArg(myKernel, 2, sizeof(cl_uint), (void *) &length);
    printCLError(ciErrNum,8);

    gettimeofday(&t_s_gpu, NULL);
    
    cl_event event;
    ciErrNum = clEnqueueNDRangeKernel(commandQueue, myKernel, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, &event);
    printCLError(ciErrNum,9);
    
    clWaitForEvents(1, &event); // Synch
    gettimeofday(&t_e_gpu, NULL);
    printCLError(ciErrNum,10);

  ciErrNum = clEnqueueReadBuffer(commandQueue, out_data, CL_TRUE, 0, length * sizeof(unsigned char), data, 0, NULL, &event);
    printCLError(ciErrNum,11);
    clWaitForEvents(1, &event); // Synch
  printCLError(ciErrNum,10);
    
  clReleaseMemObject(in_data);
  clReleaseMemObject(out_data);
  return ciErrNum;
}

void close_OpenCL()
{
  if (myKernel) clReleaseKernel(myKernel);
  if (myClProgram) clReleaseProgram(myClProgram);
  if (commandQueue) clReleaseCommandQueue(commandQueue);
  if (cxGPUContext) clReleaseContext(cxGPUContext);
}

// Computed data, plus input image
unsigned char *data_cpu, *data_gpu, *image;


GLuint texNum, texNum2;

////////////////////////////////////////////////////////////////////////////////
// main computation function
////////////////////////////////////////////////////////////////////////////////
void computeImages()
{
  const char *outputname_cpu = "task1_out_cpu.rbm";
  const char *outputname_gpu = "task1_out_gpu.rbm";
  int i, length; // = dataWidth * dataHeight; // SIZE OF DATA
  unsigned short int header[2];
  int n, m;
  
  if (init_OpenCL()<0)
  {
    close_OpenCL();
    return;
  }

  image = readppm("lenna512.ppm", &n, &m);
  dataWidth = n;
  dataHeight = m;
  length = dataWidth * dataHeight * 3;

  data_cpu = (unsigned char *) malloc(length);
  data_gpu = (unsigned char *) malloc(length);
  
  
  if ((!data_cpu)||(!data_gpu)||(!image))
  {
    printf("\nError allocating data.\n\n");
    return;
  }
  
  gettimeofday(&t_s_cpu, NULL);
  cpu_WL(image, data_cpu,length);
  gettimeofday(&t_e_cpu, NULL);

  gettimeofday(&t_s_gpu, NULL);
  gpu_WL(image, data_gpu,length);
  gettimeofday(&t_e_gpu, NULL);

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

    glTexImage2D(GL_TEXTURE_2D, 0, 3, m, n, 0,
             GL_RGB, GL_UNSIGNED_BYTE, data_cpu);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);

    // Draw polygon
    glBegin(GL_POLYGON);
    glColor3f(1, 1, 1);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0,-1.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f( 0.0,-1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f( 0.0, 1.0, 0.0);
    glEnd();

    glTexImage2D(GL_TEXTURE_2D, 0, 3, m, n, 0,
             GL_RGB, GL_UNSIGNED_BYTE, data_gpu);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);

    // Draw polygon
    glBegin(GL_POLYGON);
    glColor3f(1, 1, 1);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.0,-1.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f( 1.0,-1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
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
	glutCreateWindow("CPU+OpenCL output on GL");
	glutDisplayFunc(Draw);
	
	computeImages();
	
	glutMainLoop();
}
