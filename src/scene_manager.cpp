#include <iostream>
#include <fstream>
#include <string>
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

    void Scene_Manager::loadModel(std::string filepath) {
        FILE *stream = fopen(filepath.c_str(),"r");
        char *line = NULL;
        size_t len = 0;
        ssize_t nread;

        vec3 v0, v1, v2, col;
        while ((nread = getline(&line, &len, stream)) != -1) {
            sscanf(line, "%f %f %f %f %f %f %f %f %f %f %f %f",
                   &v0.x, &v0.y, &v0.z, &v1.x, &v1.y, &v1.z,
                   &v2.x, &v2.y, &v2.z, &col.x, &col.y, &col.z);
            triangles.push_back(Triangle(v0, v1, v2, col));
        }
        float miny = triangles[0].v0.y, maxy = miny;
        for (const auto &t : triangles) {
            if (t.v0.y < miny) miny = t.v0.y;
            if (t.v1.y < miny) miny = t.v1.y;
            if (t.v2.y < miny) miny = t.v2.y;
            if (t.v0.y > maxy) maxy = t.v0.y;
            if (t.v1.y > maxy) maxy = t.v1.y;
            if (t.v2.y > maxy) maxy = t.v2.y;
        }
        printf("max y: %f\n",maxy);
        printf("min y: %f\n",miny);
        free(line);
        fclose(stream);

    }

    Scene_Manager::~Scene_Manager() { }
}
