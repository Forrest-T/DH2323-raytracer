#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <glm/glm.hpp>
#include <SDL.h>
#include <vector>
#include "structs.hpp"
using glm::vec3;
using glm::mat3;
using std::vector;

vec3 indirectLight = 0.5f*vec3( 1, 1, 1 );
vec3 lightPos( 0, -0.5, -0.7 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
mat3 R;
float yaw;
vec3 rightx( R[0][0], R[0][1], R[0][2] );
vec3 downx( R[1][0], R[1][1], R[1][2] );
vec3 forwardx( R[2][0], R[2][1], R[2][2] );
SDL_Surface* screen;
int t;
vec3 camerapos (0,0,-3);
vector<Triangle> triangles;


#endif
