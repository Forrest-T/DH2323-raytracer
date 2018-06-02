#include "stdlib.h"
#include "stdio.h"

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200

#include <CL/cl2.hpp>

#include "cl_manager.hpp"

#define FAIL_ON_ERROR(x) \
    if(error != CL_SUCCESS) {\
        printf("%s\n",x);\
        exit(EXIT_FAILURE);\
    }

using namespace Raytracer;

int main() {
    cl_int error;
    CL_Manager manager;
    cl_program program;
    cl_kernel kernel;
    manager.log_level = ERROR;
    manager.device_type = GPU;
    manager.initialize();
    program = manager.createProgram({"kernels/interpolate.cl"});
    kernel = manager.createKernel(program, "interpolate");

    cl_float4 a = (cl_float4){{1,0,0}};
    cl_float4 b = (cl_float4){{0,0,1}};
    cl_float4 c = (cl_float4){{0,1,0}};
    cl_float4 d = (cl_float4){{1,1,0}};
    cl_int width = 100;
    cl_int height = 100;
    cl_float4 out[100*100];
    cl_mem buf = clCreateBuffer(manager.context, CL_MEM_WRITE_ONLY, sizeof(cl_float4)*width*height, NULL, NULL);
    error  = clSetKernelArg(kernel, 0, sizeof(cl_float4), &a);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_float4), &b);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_float4), &c);
    error |= clSetKernelArg(kernel, 3, sizeof(cl_float4), &d);
    error |= clSetKernelArg(kernel, 4, sizeof(cl_int), &width);
    error |= clSetKernelArg(kernel, 5, sizeof(cl_int), &height);
    error |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &buf);
    FAIL_ON_ERROR("failed to set kernel arguments\n");
    size_t global_size = 100*100;

    error = clEnqueueNDRangeKernel(manager.queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    FAIL_ON_ERROR("failed to enqueue kernel");
    error = clEnqueueReadBuffer(manager.queue, buf, CL_TRUE, 0, sizeof(cl_float4)*width*height, out, 0, NULL, NULL);
    FAIL_ON_ERROR("failed to read buffer\n");

    clFlush(manager.queue);
    clFinish(manager.queue);

    for (int i = 0; i < 5; i++)
        printf("(%f, %f, %f)\n",out[i].s[0], out[i].s[1], out[i].s[2]);

    if (kernel  != nullptr) clReleaseKernel(kernel);
    if (program != nullptr) clReleaseProgram(program);
    return EXIT_SUCCESS;
}
