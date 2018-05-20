#include "stdlib.h"

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200

#include <CL/cl2.hpp>

#include "io.hpp"
#include "getInfo.hpp"

int main() {
    getInfo();
    return 0;
}
