#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <glm/glm.hpp>
#include <SDL.h>
#include <cmath>
#include <vector>

#include "scene_manager.hpp"
#include "cl_manager.hpp"

using namespace Raytracer;
using std::sin;
using std::cos;

namespace Raytracer {
    /* Display-related constants */
    const int SCREEN_WIDTH = 500;
    const int SCREEN_HEIGHT = 500;
    SDL_Surface* screen;

    /* Global camera variables */
    cl_float4 camera {{0,0,-2.5,0}};
    cl_float16 R;
    const int focal = 375;//SCREEN_HEIGHT/2*(-1-camera.s[3]);
    const float camera_speed = 0.002;
    float yaw = 0.f;
    float yaw_speed = 0.15;

    /* Color and lighting variables */
    cl_float4 light_pos {{0,-.5,-.7,0}};
    cl_float4 light_dir {{.5f,.5f,.5f,0}};
    cl_float4 light_col {{14.f,14.f,14.f}};
    cl_float4 black     {{0,0,0}};
    const float light_speed = 0.05;

    Scene_Manager scene;

    /* Tick counter */
    int t;

    void update();
    void draw(CL_Manager&, Scene_Manager&, cl_kernel&, cl_mem&, cl_mem&);

    inline glm::vec3 float4_to_vec3(cl_float4 &f) {
        return glm::vec3(f.s[0], f.s[1], f.s[2]);
    }
}
#endif
