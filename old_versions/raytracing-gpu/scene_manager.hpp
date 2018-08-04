#ifndef SCENE_MANAGER_HPP_
#define SCENE_MANAGER_HPP_

#include "OpenCL.hpp"
#include "structs.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Raytracer {
    class Scene_Manager {
    public:
        enum log log_level = ERROR;
        std::vector<Triangle> triangles;

        void loadBox();
        void loadModel(std::string);
        ~Scene_Manager();

    private:
        cl_float4 normal(cl_float4 a, cl_float4 b, cl_float4 c);
        void normalize(cl_float4&);

        glm::vec3 computeNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
            glm::vec3 e1(v1-v0);
            glm::vec3 e2(v2-v0);
            return glm::normalize(glm::cross(e2, e1));
        }
        inline cl_float4 vec3_to_float4(glm::vec3 v) { return {{v.x, v.y, v.z, 0.f}}; }
    };

}
#endif
