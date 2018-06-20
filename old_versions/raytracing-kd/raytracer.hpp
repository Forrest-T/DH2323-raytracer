#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <glm/glm.hpp>
#include <SDL.h>
#include <cmath>
#include <vector>
#include "structs.hpp"
#include "Triangle.hpp"
#include "Kdtree.hpp"

using std::sin;
using std::cos;

/* macros for inline calculations */
#define DOT(a,b) ((a).x*(b).x+(a).y*(b).y+(a).z*(b).z)
#define CROSS(a,b) (vec3((a).y*(b).z-(a).z*(b).y,(a).z*(b).x-(a).x*(b).z,(a).x*(b).y-(a).y*(b).z))
#define MAX(a,b) ((a)>(b)?(a):(b))

/* Display-related constants */
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;

/* Global camera variables */
glm::vec3 camera(0,0,-2.5);
glm::mat3 R({cos(1),0,sin(1)}, {0,1,0}, {-sin(1),0,cos(1)});
const int focal = SCREEN_HEIGHT/2*(-1-camera.z);
const float camera_speed = 0.05;
float yaw = 0.f;
float yaw_speed = 0.15;

/* Color and lighting variables */
glm::vec3 light_pos(0,-.5,-.7);
const glm::vec3 light_dir(.5f,.5f,.5f);
const glm::vec3 light_col(14.f,14.f,14.f);
const glm::vec3 black(0,0,0);
const float light_speed = 0.05;

/* List of triangles in the scene */
KD_Tree *tree = nullptr;
std::vector<Triangle> triangles;

/* Calculates the closest triangle intersection
 *   returns true if an intersection was found, false otherwise
 */
bool ClosestIntersection(const glm::vec3 &start,
                         const glm::vec3 &dir,
                         const std::vector<Triangle*>&,
                         Intersection&, void *exclude = nullptr);

glm::vec3 DirectLight(const intersection&);

/* Tick counter */
int t;

void update();
void draw();

#endif
