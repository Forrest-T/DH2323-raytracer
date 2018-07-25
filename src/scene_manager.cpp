#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "../include/scene_manager.hpp"
#include "../include/model.hpp"

using std::ifstream;
using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

namespace Raytracer {
    void Scene_Manager::initialize(string filepath) {
        ifstream file(filepath);
        if (!file.good()) {
            cerr << "Unable to read file: " << filepath << endl;
            exit(EXIT_FAILURE);
        }
        char line[256];
        file.getline(line, sizeof(line));
        if (log_level != SILENT)
            cerr << "scene input file format not recognized" << endl;
        exit(EXIT_FAILURE);
    }

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
            triangles.push_back(Triangle(t.v0, t.v1, t.v2, t.color));
    }

    Scene_Manager::~Scene_Manager() { }
}
