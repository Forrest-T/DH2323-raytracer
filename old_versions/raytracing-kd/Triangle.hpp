#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include <glm/glm.hpp>
#include "BBox.hpp"
using glm::vec3;

class Triangle {
public:
    vec3 v0;
    vec3 v1;
    vec3 v2;
    vec3 color;
    vec3 normal;
    vec3 midpoint;
    BBox boundingBox;

    Triangle() = default;
    Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 color);
    Triangle(const Triangle &o): v0(o.v0), v1(o.v1), v2(o.v2), color(o.color),
                                 normal(o.normal), midpoint(o.midpoint),
                                 boundingBox(o.boundingBox) {}

private:
    inline void computeNormal();
    inline void computeMidpoint() { midpoint = (v0+v1+v2)/3.f; }
    void computeBBox();
};


#endif
