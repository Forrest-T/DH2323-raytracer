cmake_minimum_required (VERSION 2.6)
project (opencl)

add_executable(main main.cpp cl_manager.cpp)

find_package (OpenCL)
if (NOT OpenCL_FOUND)
    message (FATAL_ERROR "OpenCL not found!")
endif (NOT OpenCL_FOUND)
if (OPENCL_FOUND)
    include_directories(${OpenCL_INCLUDE_DIR})
    target_link_libraries(main ${OpenCL_LIBRARY})
endif (OPENCL_FOUND)
