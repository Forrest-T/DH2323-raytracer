#include "stdlib.h"
#include "stdio.h"

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200

#include <CL/cl2.hpp>

#include "io.hpp"
#include "cl_manager.hpp"

#define FAIL_ON_ERROR(x) \
    if(error != CL_SUCCESS) {\
        printf("%s\n",x);\
        exit(EXIT_FAILURE);\
    }

using namespace Raytracer;

int main() {
    cl_int error;
    Manager manager;
    manager.log_level = VERBOSE;
    manager.initialize();
    return EXIT_SUCCESS;

    const char *source = reinterpret_cast<const char*>(readKernel("kernels/interpolate.cl"));
    manager.program = clCreateProgramWithSource(manager.context, 1, &source, NULL, &error);
    FAIL_ON_ERROR("failed creating program");

    error = clBuildProgram(manager.program, 1, &manager.device, "", NULL, NULL);
    FAIL_ON_ERROR("failed building program");
    if (error != CL_SUCCESS) {
        char log[2048];
        error = clGetProgramBuildInfo(manager.program, manager.device, CL_PROGRAM_BUILD_LOG, 2048, &log, NULL);
        printf("%s\n",log);
    }

    manager.kernel = clCreateKernel(manager.program, "interpolate", &error);
    FAIL_ON_ERROR("failed creating kernel\n");
    cl_float4 a = (cl_float4){{1,0,0}};
    cl_float4 b = (cl_float4){{0,0,1}};
    cl_float4 c = (cl_float4){{0,1,0}};
    cl_float4 d = (cl_float4){{1,1,0}};
    cl_int width = 100;
    cl_int height = 100;
    cl_float4 out[100*100];
    cl_mem buf = clCreateBuffer(manager.context, CL_MEM_WRITE_ONLY, sizeof(cl_float4)*width*height, NULL, NULL);
    error  = clSetKernelArg(manager.kernel, 0, sizeof(cl_float4), &a);
    error |= clSetKernelArg(manager.kernel, 1, sizeof(cl_float4), &b);
    error |= clSetKernelArg(manager.kernel, 2, sizeof(cl_float4), &c);
    error |= clSetKernelArg(manager.kernel, 3, sizeof(cl_float4), &d);
    error |= clSetKernelArg(manager.kernel, 4, sizeof(cl_int), &width);
    error |= clSetKernelArg(manager.kernel, 5, sizeof(cl_int), &height);
    error |= clSetKernelArg(manager.kernel, 6, sizeof(cl_mem), &buf);
    FAIL_ON_ERROR("failed to set kernel arguments\n");
    size_t global_size = 100*100;

    error = clEnqueueNDRangeKernel(manager.queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    FAIL_ON_ERROR("failed to enqueue kernel");
    error = clEnqueueReadBuffer(manager.queue, buf, CL_TRUE, 0, sizeof(cl_float4)*width*height, out, 0, NULL, NULL);
    FAIL_ON_ERROR("failed to read buffer\n");

    clFlush(manager.queue);
    clFinish(manager.queue);
    free((void*)source);

    for (int i = 0; i < 5; i++)
        printf("(%f, %f, %f)\n",out[i].s[0], out[i].s[1], out[i].s[2]);

    return EXIT_SUCCESS;
}
