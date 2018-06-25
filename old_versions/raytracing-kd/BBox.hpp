#ifndef BBox_hpp
#define BBox_hpp

#include <glm/glm.hpp>
#include "structs.hpp"

using glm::vec3;

const float EPSILON = 0.00001f;

class BBox{
public:
    vec3 min;
    vec3 max;

    BBox(){}
    BBox (const vec3 &min, const vec3 &max): min(min), max(max) {}
    BBox (const BBox &o): min(o.min), max(o.max) {}

    int quantize(float ind);

    bool BoxIntersection(vec3, vec3, Intersection&);
    int longestaxis();
};
#endif /* BBox_hpp */
