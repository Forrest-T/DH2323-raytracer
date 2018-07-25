#include <glm/glm.hpp>
#include <vector>
#include "../include/Triangle.hpp"
using std::min;
using std::max;

namespace Raytracer {
Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 color)
        : v0(v0), v1(v1), v2(v2), color(color) {
    computeNormal();
    computeMidpoint();
    computeBBox();
}

void Triangle::computeNormal() {
    vec3 e1(v1-v0);
    vec3 e2(v2-v0);
    normal = glm::normalize(glm::cross(e2, e1));
}

void Triangle::computeBBox() {
    vec3 minb(v0);
    vec3 maxb(v0);
    
    for (const auto &v : {v1, v2}) {
        minb.x = min(minb.x, v.x);
        minb.y = min(minb.y, v.y);
        minb.z = min(minb.z, v.z);

        maxb.x = max(maxb.x, v.x);
        maxb.y = max(maxb.y, v.y);
        maxb.z = max(maxb.z, v.z);
    }

    boundingBox = BBox(minb, maxb);
}
}
