#include "stdlib.h"

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200

#include <CL/cl2.hpp>

#include "io.hpp"
#include "getInfo.hpp"

int main() {
    cl_int error = 0;
    cl_uint num_platforms = 0;
    cl_platform_id platform = NULL;
    cl_uint num_devices = 0;
    cl_device_id device = NULL;
    cl_context context = NULL;
    cl_command_queue queue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;

    clGetPlatformIDs(1, &platform, &num_platforms);
    if (!num_platforms) {
        fprintf(stderr, "Unable to find any platforms\n");
        exit(EXIT_FAILURE);
    }

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
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
    if (error != CL_SUCCESS)
        printf("failed building program\n");
    kernel = clCreateKernel(program, "interpolate", &error);
    if (error != CL_SUCCESS)
        printf("failed creating kernel\n");
    cl_float3 a = (cl_float3){0,0,0};
    cl_float3 b = (cl_float3){1,1,1};
    cl_int width = 100;
    cl_float3 out[100];
    cl_mem buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float3)*width, NULL, NULL);
    error = clSetKernelArg(kernel, 0, sizeof(cl_float3), &a);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_float3), &b);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_int), &width);
    error |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &buf);
    if (error != CL_SUCCESS)
        printf("failed to set kernel arguments\n");
    size_t global_size = 100;

    error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    if (error != CL_SUCCESS)
        printf("failed to enqueue kernel");
    clEnqueueReadBuffer(queue, buf, CL_TRUE, 0, sizeof(cl_float3)*width, out, 0, NULL, NULL);
    if (error != CL_SUCCESS)
        printf("failed to read buffer");

    /********************************/

    /********************************/

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
