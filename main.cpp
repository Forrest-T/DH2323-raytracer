#include "OpenCL.hpp"

#include "stdlib.h"
#include "stdio.h"

#include "cl_manager.hpp"
#include "scene_manager.hpp"
#include "labs/SDLauxiliary.h"
#include "main.hpp"

using namespace Raytracer;
using glm::vec3;

int main() {
    screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, false);

    Scene_Manager scene;
    scene.log_level = VERBOSE;
    scene.loadBox();
    //scene.initialize("models/dragon_vrip_res4.ply");

    CL_Manager manager;
    manager.log_level = VERBOSE;
    manager.device_type = GPU;
    manager.initialize();

    // TODO raytracing kernel
    cl_program program = manager.createProgram({"kernels/interpolate.cl"});
    cl_kernel kernel = manager.createKernel(program, "interpolate");

    t = SDL_GetTicks();
    while (NoQuitMessageSDL()) {
        update();
        draw(manager, scene, kernel);
    }

    SDL_SaveBMP(screen, "screenshot.bmp");
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    return EXIT_SUCCESS;
}

void update() {
    int t2 = SDL_GetTicks();
    float dt = float(t2-t);
    t = t2;
    printf("Render time: %f ms\n",dt);
    Uint8 *keystate = SDL_GetKeyState(0);
    if (keystate[SDLK_UP])
        camera.s[3] += camera_speed;
    // TODO: handle key states
    // TODO: calculate rotation matrix R
    R = {{1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f}};
}

void draw(CL_Manager &manager, Scene_Manager &scene, cl_kernel &kernel) {
    scene.log_level = VERBOSE;
    cl_int error = CL_SUCCESS;
    cl_float4 a = (cl_float4){{1,0,0}};
    cl_float4 b = (cl_float4){{0,0,1}};
    cl_float4 c = (cl_float4){{0,1,0}};
    cl_float4 d = (cl_float4){{1,1,0}};
    cl_int width = 100;
    cl_int height = 100;
    cl_float4 pixels[SCREEN_WIDTH*SCREEN_HEIGHT];
    cl_mem buf = clCreateBuffer(manager.context, CL_MEM_WRITE_ONLY, sizeof(cl_float4)*width*height, NULL, NULL);
    error  = clSetKernelArg(kernel, 0, sizeof(cl_float4), &a);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_float4), &b);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_float4), &c);
    error |= clSetKernelArg(kernel, 3, sizeof(cl_float4), &d);
    error |= clSetKernelArg(kernel, 4, sizeof(cl_int), &width);
    error |= clSetKernelArg(kernel, 5, sizeof(cl_int), &height);
    error |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &buf);
    manager.checkError(error, "failed to set kernel arguments\n");
    size_t global_size = 100*100;

    manager.checkError(clEnqueueNDRangeKernel(manager.queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL),"failed to enqueue kernel");
    manager.checkError(clEnqueueReadBuffer(manager.queue, buf, CL_TRUE, 0, sizeof(cl_float4)*width*height, pixels, 0, NULL, NULL), "failed to read buffer\n");

    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
    for (int x = 0; x < SCREEN_WIDTH; x++)
        for (int y = 0; y < SCREEN_HEIGHT; y++)
            PutPixelSDL(screen, x, y, float4_to_vec3(pixels[x+y*SCREEN_WIDTH]));
    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

}

inline glm::vec3 float4_to_vec3(cl_float4 &f) {
    return vec3(f.s[0], f.s[1], f.s[2]);
}
