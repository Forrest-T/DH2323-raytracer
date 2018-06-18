#ifndef STRUCTS_HPP_
#define STRUCTS_HPP_

#include <glm/glm.hpp>
using glm::vec3;

typedef struct intersection {
    vec3 position;
    float distance;
    int index;
} Intersection;

typedef struct ray {
    vec3 origin;
    vec3 direction;
} Ray;
#endif
