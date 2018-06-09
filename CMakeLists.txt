cmake_minimum_required (VERSION 2.6)
project (raytracer)

add_executable(main
               main.cpp
               cl_manager.cpp
               scene_manager.cpp
               labs/SDLauxiliary.h
               labs/TestModel.h)

find_package (SDL)
if (NOT SDL_FOUND)
    message (FATAL_ERROR "SDL not found!")
endif (NOT SDL_FOUND)
if (SDL_FOUND)
    include_directories(${SDL_INCLUDE_DIR})
    target_link_libraries(main ${SDL_LIBRARY})
endif (SDL_FOUND)

find_package (OpenCL)
if (NOT OpenCL_FOUND)
    message (FATAL_ERROR "OpenCL not found!")
endif (NOT OpenCL_FOUND)
if (SDL_FOUND)
    include_directories(${OpenCL_INCLUDE_DIR})
    target_link_libraries(main ${OpenCL_LIBRARY})
endif (SDL_FOUND)
