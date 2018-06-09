#include "OpenCL.hpp"

#include "stdlib.h"
#include "stdio.h"

#include "cl_manager.hpp"

int main() {
    cl_int error = CL_SUCCESS;
    // initialize OpenCL
    CL_Manager manager;
    manager.log_level = VERBOSE;
    manager.device_type = GPU;
    manager.initialize();

    // compile the OpenCL program
    cl_program program = manager.createProgram({"testKernel.cl"});
    cl_kernel kernel = manager.createKernel(program, "multiply");

    // create input and output
    cl_int a = 6;
    cl_int b = 7;
    cl_int out[1];

    // create a buffer for the kernel to store output
    cl_mem buf = clCreateBuffer(manager.context, CL_MEM_WRITE_ONLY,
                                sizeof(cl_int), NULL, NULL);

    // set the kernel arguments
    error  = clSetKernelArg(kernel, 0, sizeof(cl_int), &a);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_int), &b);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &buf);
    manager.checkError(error, "failed to set kernel arguments\n");

    // enqueue the kernel, telling it to handle only one item
    size_t global_size = 1;
    manager.checkError(
            clEnqueueNDRangeKernel(manager.queue, kernel, 1, NULL,
                                   &global_size, NULL, 0, NULL, NULL),
            "failed to enqueue kernel");

    // read the result from the GPU into the output buffer
    manager.checkError(
            clEnqueueReadBuffer(manager.queue, buf, CL_TRUE, 0,
                                sizeof(cl_int), out, 0, NULL, NULL),
            "failed to read buffer\n");

    clReleaseKernel(kernel);
    clReleaseProgram(program);

    printf("The answer is: %d\n", *out);
    return EXIT_SUCCESS;
}
