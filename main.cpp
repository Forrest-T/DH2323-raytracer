#include "stdlib.h"

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200

#include <CL/cl2.hpp>

#include "io.hpp"
#include "getInfo.hpp"

int main() {
    cl_int error = 0;
    cl_uint num_platforms = 0;
    cl_platform_id platform[2];
    cl_uint num_devices = 0;
    cl_device_id device = NULL;
    cl_context context = NULL;
    cl_command_queue queue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;

    clGetPlatformIDs(2, platform, &num_platforms);
    if (!num_platforms) {
        fprintf(stderr, "Unable to find any platforms\n");
        exit(EXIT_FAILURE);
    }

    clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
    if (!num_devices) {
        fprintf(stderr, "Unable to find any GPU devices\n");
        exit(EXIT_FAILURE);
    }

    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    // todo: Does the queue need to be executed in order???
    //cl_queue_properties queue_props = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
    queue = clCreateCommandQueueWithProperties(context, device, NULL, NULL);

    const char *source = readKernel("kernels/interpolate.cl");
    program = clCreateProgramWithSource(context, 1, &source, NULL, &error);
    if (error != CL_SUCCESS)
        printf("failed creating program\n");


    error = clBuildProgram(program, 1, &device, "", NULL, NULL);
    if (error != CL_SUCCESS) {
        char log[9999];
        error = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 9999, &log, NULL);
        printf("%s\n",log);
        printf("%d\n",CL_BUILD_NONE);
        printf("%d\n",CL_BUILD_ERROR);
        printf("%d\n",CL_BUILD_SUCCESS);
        printf("%d\n",CL_BUILD_IN_PROGRESS);
    printf("%s\n",source);




//        printf("failed building program\n");
//        if (error == CL_BUILD_PROGRAM_FAILURE)
//            printf("build program failed\n");
    }
    kernel = clCreateKernel(program, "interpolate", &error);
    if (error != CL_SUCCESS)
        printf("failed creating kernel\n");
    cl_float4 a = (cl_float4){{1,0,0}};
    cl_float4 b = (cl_float4){{0,0,1}};
    cl_float4 c = (cl_float4){{0,1,0}};
    cl_float4 d = (cl_float4){{1,1,0}};
    cl_int width = 100;
    cl_int height = 100;
    cl_float4 out[100*100];
    cl_mem buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float4)*width*height, NULL, NULL);
    error  = clSetKernelArg(kernel, 0, sizeof(cl_float4), &a);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_float4), &b);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_float4), &c);
    error |= clSetKernelArg(kernel, 3, sizeof(cl_float4), &d);
    error |= clSetKernelArg(kernel, 4, sizeof(cl_int), &width);
    error |= clSetKernelArg(kernel, 5, sizeof(cl_int), &height);
    error |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &buf);
    if (error != CL_SUCCESS)
        printf("failed to set kernel arguments\n");
    size_t global_size = 100*100;

    error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    if (error != CL_SUCCESS)
        printf("failed to enqueue kernel");
    clEnqueueReadBuffer(queue, buf, CL_TRUE, 0, sizeof(cl_float4)*width*height, out, 0, NULL, NULL);
    if (error != CL_SUCCESS)
        printf("failed to read buffer\n");

    clFlush(queue);
    clFinish(queue);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free((void*)source);

    for (int i = 0; i < 5; i++)
        printf("(%f, %f, %f)\n",out[i].s[0], out[i].s[1], out[i].s[2]);

    return EXIT_SUCCESS;
}
