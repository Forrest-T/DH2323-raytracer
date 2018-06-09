#include "scene_manager.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "labs/TestModel.h"

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
        if (!strncmp(line, "ply", 3)) {
            if (log_level == VERBOSE)
                cout << "detected Dragon file" << endl;
            readDragon(file);
        } else {
            if (log_level != SILENT)
                cerr << "scene input file format not recognized" << endl;
            exit(EXIT_FAILURE);
        }
    }

    void Scene_Manager::readDragon(ifstream &file) {
        int num_vert;
        int num_face;
        char line[256];
        char *tok;
        do file.getline(line, sizeof(line)); while (strncmp("element",line,7));
        tok = strtok(line," ");
        tok = strtok(NULL," ");
        tok = strtok(NULL," ");
        sscanf(tok,"%d",&num_vert);
        do file.getline(line, sizeof(line)); while (strncmp("element",line,7));
        tok = strtok(line," ");
        tok = strtok(NULL," ");
        tok = strtok(NULL," ");
        sscanf(tok,"%d",&num_face);
        cout << "vertices: " << num_vert << endl;
        cout << "faces: " << num_face << endl;
        do file.getline(line, sizeof(line)); while (strncmp("end_header",line,10));
        vector<cl_float4> vertices(num_vert);
        float a, b, c, d=0;
        for (int i = 0; i < num_vert; i++) {
            file.getline(line,sizeof(line));
            sscanf(strtok(line," "),"%f",&a);
            sscanf(strtok(NULL," "),"%f",&b);
            sscanf(strtok(NULL," "),"%f",&c);
            vertices[i] = (cl_float4){{a, b, c, d}};
        }
        int x, y, z;
        triangles.reserve(num_face);
        for (int i = 0; i < num_face; i++) {
            file.getline(line,sizeof(line));
            strtok(line," ");
            sscanf(strtok(NULL," "),"%d",&x);
            sscanf(strtok(NULL," "),"%d",&y);
            sscanf(strtok(NULL," "),"%d",&z);
            triangles.push_back({vertices[x], vertices[y], vertices[z],
                                 normal(vertices[x], vertices[y], vertices[z]),
                                 {{1,1,1,0}}});
        }
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
        vector<lab_Triangle> box;
        for (const auto &t : box)
            triangles.push_back({{{t.v0.x,t.v0.y,t.v0.z,0}},
                                 {{t.v1.x,t.v1.y,t.v1.z,0}},
                                 {{t.v2.x,t.v2.y,t.v2.z,0}},
                                 {{t.normal.x,t.normal.y,t.normal.z,0}},
                                 {{t.color.x,t.color.y,t.color.z,0}}});
    }

    Scene_Manager::~Scene_Manager() {
    }
}
