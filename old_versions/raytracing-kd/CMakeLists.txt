cmake_minimum_required (VERSION 2.6)
project (raytracer)

add_executable(main main.cpp
               BBox.cpp
               Triangle.cpp)

find_package (SDL)
if (NOT SDL_FOUND)
    message (FATAL_ERROR "SDL not found!")
endif (NOT SDL_FOUND)
if (SDL_FOUND)
    include_directories(${SDL_INCLUDE_DIR})
    target_link_libraries(main ${SDL_LIBRARY})
endif (SDL_FOUND)
