#ifndef SCENE_MANAGER_HPP_
#define SCENE_MANAGER_HPP_

#include "OpenCL.hpp"
#include "structs.hpp"
#include "Triangle.hpp"
#include <vector>

namespace Raytracer {
    class Scene_Manager {
    public:
        enum log log_level = ERROR;
        std::vector<Triangle> triangles;

        void initialize(std::string);
        void loadBox();
        ~Scene_Manager();
    private:
        cl_float4 normal(cl_float4 a, cl_float4 b, cl_float4 c);
        void normalize(cl_float4&);
    };
}
#endif
