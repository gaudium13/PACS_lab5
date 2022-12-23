////////////////////////////////////////////////////////////////////
//File: basic_environ.c
//
//Description: base file for environment exercises with openCL
//
// 
////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "CImg/CImg.h"
#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif
  
// check error, in such a case, it exits

void cl_error(cl_int code, const char *string){
	if (code != CL_SUCCESS){
		printf("%d - %s\n", code, string);
	    exit(-1);
	}
}
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

  clock_t start_program = clock();

  int err;                            	// error code returned from api calls
  size_t t_buf = 50;			// size of str_buffer
  char str_buffer[t_buf];		// auxiliary buffer	
  size_t e_buf;				// effective size of str_buffer in use
	    
  size_t global_size;                      	// global domain size for our calculation
  size_t local_size;                       	// local domain size for our calculation

  const cl_uint num_platforms_ids = 10;				// max of allocatable platforms
  cl_platform_id platforms_ids[num_platforms_ids];		// array of platforms
  cl_uint n_platforms;						// effective number of platforms in use
  const cl_uint num_devices_ids = 10;				// max of allocatable devices
  cl_device_id devices_ids[num_platforms_ids][num_devices_ids];	// array of devices
  cl_uint n_devices[num_platforms_ids];				// effective number of devices in use for each platform
	
  cl_device_id device_id;             				// compute device id 
  cl_context context;                 				// compute context
  cl_command_queue command_queue;     				// compute command queue

  cl_program program;
  cl_kernel kernel;


  // read image
  // cimg_library::CImg<unsigned char> img("image.jpg");
  // printf("%d", img.width());
  //  img.display("My first Cimg code");


  // 1. Scan the available platforms:
  err = clGetPlatformIDs (num_platforms_ids, platforms_ids, &n_platforms);
  cl_error(err, "Error: Failed to Scan for Platforms IDs");
  printf("Number of available platforms: %d\n\n", n_platforms);

  for (int i = 0; i < n_platforms; i++ ){
    char str_buffer[50];
    size_t ret;
    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_NAME, sizeof(str_buffer), str_buffer, &ret);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Name: %s\n", i, str_buffer);
  }
  printf("\n");
  // ***Task***: print on the screen the name, host_timer_resolution, vendor, versionm, ...
	
  // 2. Scan for devices in each platform
  for (int i = 0; i < n_platforms; i++ ){
    printf("%d %d",i,n_platforms);
    err = clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, num_devices_ids, devices_ids[i], &(n_devices[i]));
    cl_error(err, "Error: Failed to Scan for Devices IDs");
    printf("\t[%d]-Platform. Number of available devices: %d\n", i, n_devices[i]);

    for(int j = 0; j < n_devices[i]; j++){
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_NAME, sizeof(str_buffer), &str_buffer, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device name");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_NAME: %s\n", i, j,str_buffer);

      cl_uint max_compute_units_available;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units_available), &max_compute_units_available, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device max compute units available");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_COMPUTE_UNITS: %d\n\n", i, j, max_compute_units_available);
    }
  }	
  // ***Task***: print on the screen the cache size, global mem size, local memsize, max work group size, profiling timer resolution and ... of each device


  
  // 3. Create a context, with a device
  printf("%d", 96);
  cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms_ids[0], 0};
  context = clCreateContext(properties, n_devices[0], &devices_ids[0][0], NULL, NULL, &err);
  cl_error(err, "Failed to create a compute context\n");
  // 4. Create a command queue
  cl_command_queue_properties proprt[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
  command_queue = clCreateCommandQueueWithProperties( context, devices_ids[0][0], proprt, &err);
  cl_error(err, "Failed to create a command queue\n");


  
  // Calculate size of the file
  FILE *fileHandler = fopen("kernel_rotate.cl", "r");
  fseek(fileHandler, 0, SEEK_END);
  size_t fileSize = ftell(fileHandler);
  printf("\nFilesizeis: %d\n", fileSize);
  rewind(fileHandler);

  // read kernel source into buffer
  char * sourceCode = (char*) malloc(fileSize + 1);
  sourceCode[fileSize] = '\0';
  fread(sourceCode, sizeof(char), fileSize, fileHandler);
  fclose(fileHandler);
  
  
  // create program from buffer
  // program = clCreateProgramWithSource(context, 1, (const char **) sourceCode, (const size_t*) fileSize, &err);
  program = clCreateProgramWithSource(context, 1, (const char**)&sourceCode, &fileSize, &err);
  //printf("%s\n", sourceCode);
  //printf("%d\n", fileSize);
  
  cl_error(err, "Failed to create program with source\n");
  free(sourceCode);
  
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS){
    size_t len = 2048;
    char buffer[2048];

    printf("Error: Some error at building process. \n");
    clGetProgramBuildInfo(program, devices_ids[0][0], CL_PROGRAM_BUILD_LOG, len, &buffer, NULL);
    printf("%s\n", buffer);
    exit(-1);
  }
  


  cimg_library::CImg<unsigned char> img("image.jpg");
  unsigned int himg_width = img.width();
  unsigned int himg_height = img.height();
  unsigned int num_channels = img.depth();

  unsigned int angle = 30;

  printf("Image width: %d\n Image height: %d\n Image depth: %d\n", himg_width, himg_height, num_channels);
  
  unsigned char img_red[himg_width*himg_height];
  unsigned char img_green[himg_width*himg_height];
  unsigned char img_blue[himg_width*himg_height];

  for (unsigned int row = 0; row < himg_height; row++)
  {
	for (unsigned int column = 0; column < himg_width; column++)
	{
		img_red[row*himg_width+column] = img(column, row, 0, 0);
		img_green[row*himg_width+column] = img(column, row, 0, 1);
		img_blue[row*himg_width+column] = img(column, row, 0, 2);
	}
  }

  cl_ulong time_start, time_end, read_time_start, read_time_end, write_time_start, write_time_end;
  cl_event timing_event, timing_event_read, timing_event_write;

  kernel = clCreateKernel(program, "rotate_img", NULL);
  cl_error(err, "Failed to create kernel from the program.\n");

  printf("Host info declared");
  cl_mem in_red_channel = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char)*himg_width*himg_height, NULL, &err);
  cl_mem in_green_channel = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char)*himg_width*himg_height, NULL, &err);
  cl_mem in_blue_channel = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char)*himg_width*himg_height, NULL, &err);

  cl_mem out_red_channel = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned char)*himg_width*himg_height, NULL, &err);
  cl_mem out_green_channel = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned char)*himg_width*himg_height, NULL, &err);
  cl_mem out_blue_channel = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned char)*himg_width*himg_height, NULL, &err);
  cl_error(err, "Failed to create memory buffer at device\n");
  
  err = clEnqueueWriteBuffer(command_queue, in_red_channel, CL_TRUE, 0, sizeof(unsigned char)*himg_width*himg_height, img_red, 0, NULL, &timing_event_write);
  err = clEnqueueWriteBuffer(command_queue, in_green_channel, CL_TRUE, 0, sizeof(unsigned char)*himg_width*himg_height, img_green, 0, NULL, &timing_event_write);
  err = clEnqueueWriteBuffer(command_queue, in_blue_channel, CL_TRUE, 0, sizeof(unsigned char)*himg_width*himg_height, img_blue, 0, NULL, &timing_event_write);
  cl_error(err, "Failed to enqueue a write command\n");
  
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &in_red_channel);
  cl_error(err, "Failed to set argument 0\n");
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &in_green_channel);
  cl_error(err, "Failed to set argument 1\n");
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &in_blue_channel);
  cl_error(err, "Failed to set argument 2\n");
  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &out_red_channel);
  cl_error(err, "Failed to set argument 3\n");
  err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &out_green_channel);
  cl_error(err, "Failed to set argument 4\n");
  err = clSetKernelArg(kernel, 5, sizeof(cl_mem), &out_blue_channel);
  cl_error(err, "Failed to set argument 5\n");
  err = clSetKernelArg(kernel, 6, sizeof(unsigned int), &himg_width);
  cl_error(err, "Failed to set argument 6\n");
  err = clSetKernelArg(kernel, 7, sizeof(unsigned int), &himg_height);
  cl_error(err, "Failed to set argument 7\n");
  err = clSetKernelArg(kernel, 8, sizeof(unsigned int), &angle);
  cl_error(err, "Failed to set argument 8\n");
  
  local_size = 128;

  global_size = 128;
  
  err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, &timing_event);
  cl_error(err, "Failed to launch kernel to the device\n");

  unsigned char img_red_out[himg_width*himg_height];
  unsigned char img_green_out[himg_width*himg_height];
  unsigned char img_blue_out[himg_width*himg_height];
  
  err = clEnqueueReadBuffer(command_queue, out_red_channel, CL_TRUE, 0, sizeof(unsigned char)*himg_width*himg_height, img_red_out, 0, NULL, &timing_event_read);
  err = clEnqueueReadBuffer(command_queue, out_green_channel, CL_TRUE, 0, sizeof(unsigned char)*himg_width*himg_height, img_green_out, 0, NULL, &timing_event_read);
  err = clEnqueueReadBuffer(command_queue, out_blue_channel, CL_TRUE, 0, sizeof(unsigned char)*himg_width*himg_height, img_blue_out, 0, NULL, &timing_event_read);
  cl_error(err, "Failed to enqueue a read command\n");

  clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
  clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
 
  clGetEventProfilingInfo(timing_event_write, CL_PROFILING_COMMAND_START, sizeof(write_time_start), &write_time_start, NULL);
  clGetEventProfilingInfo(timing_event_write, CL_PROFILING_COMMAND_END, sizeof(write_time_end), &write_time_end, NULL);

  clGetEventProfilingInfo(timing_event_read, CL_PROFILING_COMMAND_START, sizeof(read_time_start), &read_time_start, NULL);
  clGetEventProfilingInfo(timing_event_read, CL_PROFILING_COMMAND_END, sizeof(read_time_end), &read_time_end, NULL);

  cimg_library::CImg<unsigned char> rotated_img(himg_height, himg_width, 1, 3);

  for (unsigned int row = 0; row < himg_height; row++)
  {
	for (unsigned int column = 0; column < himg_width; column++)
	{
		flipped_img(column, row, 0, 0) = img_red[row*himg_width+column];
		flipped_img(column, row, 0, 1) = img_green[row*himg_width+column];
		flipped_img(column, row, 0, 2) = img_blue[row*himg_width+column];
	}
  }
  
  img.display("Original Image");
  flipped_img.display("Flipped Image");

  
  clReleaseMemObject(in_red_channel);
  clReleaseMemObject(in_green_channel);
  clReleaseMemObject(in_blue_channel);
  clReleaseMemObject(out_red_channel);
  clReleaseMemObject(out_green_channel);
  clReleaseMemObject(out_blue_channel);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(command_queue);
  clReleaseContext(context);

  clock_t end_program = clock();	
  double seconds = (double) (end_program- start_program)/CLOCKS_PER_SEC;
  printf("\n Time by program: %lf \n", seconds);
  printf("\n Kernel time: %lf \n\n", (double)(time_end-time_start)/1000000000);
  printf("\n Kernel time read memory: %lf \n\n", (double)(read_time_end-read_time_start)/1000000000);
  printf("\n Kernel time write memory: %lf \n\n", (double)(write_time_end-write_time_start)/1000000000);
  return 0;
  
}

