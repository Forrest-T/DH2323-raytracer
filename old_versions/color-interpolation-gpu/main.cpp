#include "OpenCL.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "SDL_helper.hpp"
#include "cl_manager.hpp"
#include "main.hpp"

using glm::vec3;

int main() {
    screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, false);

    CL_Manager manager;
    manager.log_level = ERROR;
    manager.device_type = GPU;
    manager.initialize();

    cl_program program = manager.createProgram({"interpolate.cl"});
    cl_kernel kernel = manager.createKernel(program, "mixColors");

    draw(manager, kernel);
    while (NoQuitMessageSDL()) usleep(100000);

    SDL_SaveBMP(screen, "screenshot.bmp");
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    return EXIT_SUCCESS;
}

void draw(CL_Manager &manager, cl_kernel &kernel) {
    cl_int error = CL_SUCCESS;
    cl_float4 a = (cl_float4){{1,0,0,0}};
    cl_float4 b = (cl_float4){{0,0,1,0}};
    cl_float4 c = (cl_float4){{0,1,0,0}};
    cl_float4 d = (cl_float4){{1,1,0,0}};
    cl_int width = SCREEN_WIDTH;
    cl_int height = SCREEN_HEIGHT;
    size_t size = width * height;
    cl_float4 pixels[size];
    cl_mem buf = clCreateBuffer(manager.context, CL_MEM_WRITE_ONLY,
                                sizeof(cl_float4)*size, NULL, NULL);
    error  = clSetKernelArg(kernel, 0, sizeof(cl_float4), &a);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_float4), &b);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_float4), &c);
    error |= clSetKernelArg(kernel, 3, sizeof(cl_float4), &d);
    error |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &buf);
    error |= clSetKernelArg(kernel, 5, sizeof(cl_int), &width);
    error |= clSetKernelArg(kernel, 6, sizeof(cl_int), &height);
    manager.checkError(error, "failed to set kernel arguments\n");

    manager.checkError(
            clEnqueueNDRangeKernel(
                manager.queue, kernel, 1, NULL,
                &size, NULL, 0, NULL, NULL),
            "failed to enqueue kernel");

    manager.checkError(
            clEnqueueReadBuffer(
                manager.queue, buf, CL_TRUE, 0,
                sizeof(cl_float4)*size, pixels, 0, NULL, NULL),
            "failed to read buffer\n");

    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            PutPixelSDL(screen, x, y, float4_to_vec3(pixels[x+y*height]));
    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

}

inline glm::vec3 float4_to_vec3(cl_float4 &f) {
    return vec3(f.s[0], f.s[1], f.s[2]);
}
