#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "scene_manager.hpp"
#include "model.hpp"

using std::ifstream;
using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

namespace Raytracer {
    cl_float4 Scene_Manager::normal(cl_float4 a, cl_float4 b, cl_float4 c) {
        cl_float4 e1 = {{b.s[0]-a.s[0],b.s[1]-a.s[1],b.s[2]-a.s[2],0}};
        cl_float4 e2 = {{c.s[0]-a.s[0],c.s[1]-a.s[1],c.s[2]-a.s[2],0}};
        cl_float4 n = {{e1.s[1]*e2.s[2]-e1.s[2]*e2.s[1],
                        e1.s[2]*e2.s[0]-e1.s[0]*e2.s[2],
                        e1.s[0]*e2.s[1]-e1.s[1]*e2.s[0],0}};
        normalize(n);
        return n;
    }

    void Scene_Manager::normalize(cl_float4 &n) {
        float len = sqrtf(n.s[0]*n.s[0]+n.s[1]*n.s[1]+n.s[2]*n.s[2]);
        if (len == 0) return;
        n.s[0] /= len;
        n.s[1] /= len;
        n.s[2] /= len;
        n.s[3] /= len;
    }

    void Scene_Manager::loadBox() {
        vector<struct glm_triangle> box;
        LoadTestModel(box);
        for (const auto &t : box)
            triangles.push_back({{{t.v0.x,t.v0.y,t.v0.z,0}},
                                 {{t.v1.x,t.v1.y,t.v1.z,0}},
                                 {{t.v2.x,t.v2.y,t.v2.z,0}},
                                 {{t.normal.x,t.normal.y,t.normal.z,0}},
                                 {{t.color.x,t.color.y,t.color.z,0}}});
    }

    void Scene_Manager::loadModel(std::string filepath) {
        FILE *stream = fopen(filepath.c_str(),"r");
        char *line = NULL;
        size_t len = 0;
        ssize_t nread;

        vec3 v0, v1, v2, col, norm;
        while ((nread = getline(&line, &len, stream)) != -1) {
            sscanf(line, "%f %f %f %f %f %f %f %f %f %f %f %f",
                   &v0.x, &v0.y, &v0.z, &v1.x, &v1.y, &v1.z,
                   &v2.x, &v2.y, &v2.z, &col.x, &col.y, &col.z);
            norm = computeNormal(v0, v1, v2);
            triangles.push_back({vec3_to_float4(v0),
                                 vec3_to_float4(v1),
                                 vec3_to_float4(v2),
                                 vec3_to_float4(norm),
                                 vec3_to_float4(col)});
        }
        free(line);
        fclose(stream);

    }

    Scene_Manager::~Scene_Manager() { }
}
