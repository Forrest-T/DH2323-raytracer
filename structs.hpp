#ifndef STRUCTS_HPP_
#define STRUCTS_HPP_

#include "OpenCL.hpp"

namespace Raytracer {
    enum log {SILENT, ERROR, VERBOSE};
    typedef struct triangle {
        cl_float4 v0;
        cl_float4 v1;
        cl_float4 v2;
        cl_float4 normal;
        cl_float4 color;
    } Triangle;
}

#endif
