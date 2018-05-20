#include "stdlib.h"

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200

#include <CL/cl2.hpp>

#include "io.hpp"
#include "getInfo.hpp"

int main() {
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
    cl_queue_properties queue_props = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
    queue = clCreateCommandQueueWithProperties(context, device, &queue_props, NULL);

    const char *source = readKernel("kernels/increment.cl");
    program = clCreateProgramWithSource(context, 1, &source, NULL, NULL);
    clBuildProgram(program, 1, &device, "", NULL, NULL);
    kernel = clCreateKernel(program, "increment", NULL);

    /********************************/

    /********************************/

    clFlush(queue);
    clFinish(queue);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free((void*)source);

    return EXIT_SUCCESS;
}
