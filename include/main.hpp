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
    float yaw_speed = 0.003;

    /* Color and lighting variables */
    cl_float4 light_pos {{0,0,-.7,0}};
    cl_float4 light_col {{5,5,5,0}};
    cl_float4 light_glb {{.1,.1,.1,0}};
    const float light_speed = 0.0005;

    Scene_Manager scene;

    /* Tick counter */
    int t;

    void update();
    void draw(CL_Manager&, Scene_Manager&, cl_kernel&, cl_mem&);

    inline glm::vec3 float4_to_vec3(cl_float4 &f) {
        return glm::vec3(f.s[0], f.s[1], f.s[2]);
    }
}
#endif
