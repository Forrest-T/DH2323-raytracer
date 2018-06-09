#ifndef MAIN_H_
#define MAIN_H_

#include "OpenCL.hpp"
#include "cl_manager.hpp"
#include <glm/glm.hpp>
#include <SDL.h>

/* Display-related constants */
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;

inline glm::vec3 float4_to_vec3(cl_float4 &);
void draw(CL_Manager &manager, cl_kernel &kernel);
#endif
