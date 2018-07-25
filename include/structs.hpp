#ifndef STRUCTS_HPP_
#define STRUCTS_HPP_

#include "OpenCL.hpp"
#include <glm/glm.hpp>

namespace Raytracer {
    enum log {SILENT, ERROR, VERBOSE};
    typedef struct light {
        cl_float4 position;
        cl_float4 color;
        cl_float4 glob;
    } Light;
    typedef struct intersection {
        glm::vec3 position;
        float distance;
        void *triangle;
    } Intersection;
    struct bbox {
        cl_float4 min;
        cl_float4 max;
    };
    struct kdnode {
        struct bbox box;
        cl_int left_index;
        cl_int right_index;
        cl_int num_triangles; /* 0 iff not leaf node */
        cl_int triangle_begin;
    };
    struct triangle {
        cl_float4 v0;
        cl_float4 v1;
        cl_float4 v2;
        cl_float4 normal;
        cl_float4 color;
        struct bbox bbox;
    };
}

#endif
