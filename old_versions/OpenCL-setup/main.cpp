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
    cl_int a[8] = {1, 2, 3, 4, 5, 6, 7, 6};
    cl_int b[8] = {1, 1, 1, 1, 1, 1, 1, 7};
    cl_int out[8];

    // create buffers for the kernel to read the input
    cl_mem a_buf = clCreateBuffer(manager.context, CL_MEM_READ_ONLY,
                                  sizeof(cl_int)*8, NULL, NULL);
    cl_mem b_buf = clCreateBuffer(manager.context, CL_MEM_READ_ONLY,
                                  sizeof(cl_int)*8, NULL, NULL);

    // copy input into buffers for kernel
    manager.checkError(
            clEnqueueWriteBuffer(manager.queue, a_buf, CL_TRUE, 0,
                                 sizeof(cl_int)*8, a, 0, NULL, NULL),
            "failed to write to buffer");
    manager.checkError(
            clEnqueueWriteBuffer(manager.queue, b_buf, CL_TRUE, 0,
                                 sizeof(cl_int)*8, b, 0, NULL, NULL),
            "failed to write to buffer");

    // create a buffer for the kernel to store output
    cl_mem o_buf = clCreateBuffer(manager.context, CL_MEM_WRITE_ONLY,
                                  sizeof(cl_int)*8, NULL, NULL);

    // set the kernel arguments
    error  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a_buf);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &b_buf);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &o_buf);
    manager.checkError(error, "failed to set kernel arguments\n");

    // enqueue the kernel, telling it to handle only one item
    size_t global_size = 8;
    manager.checkError(
            clEnqueueNDRangeKernel(manager.queue, kernel, 1, NULL,
                                   &global_size, NULL, 0, NULL, NULL),
            "failed to enqueue kernel");

    // read the result from the GPU into the output buffer
    manager.checkError(
            clEnqueueReadBuffer(manager.queue, o_buf, CL_TRUE, 0,
                                sizeof(cl_int)*8, out, 0, NULL, NULL),
            "failed to read buffer\n");

    clReleaseKernel(kernel);
    clReleaseProgram(program);

    printf("Multiplying a = {");
    for (int i = 0; i < 7; i++)
        printf("%d, ", a[i]);
    printf("%d}\n", a[7]);
    
    printf("by          b = {");
    for (int i = 0; i < 7; i++)
        printf("%d, ", b[i]);
    printf("%d}\n", b[7]);

    printf("The answer is:  {");
    for (int i = 0; i < 7; i++)
        printf("%d, ", out[i]);
    printf("%d}\n", out[7]);
    return EXIT_SUCCESS;
}
