#include "../include/BBox.hpp"
#include "../include/structs.hpp"

int BBox::quantize(float ind) {
    if(ind < EPSILON && ind > -EPSILON)
        return 0;
    if(ind >= EPSILON)
        return 1;
    return -1;
}

void swap(float &a, float &b) {
    float temp = a;
    a = b;
    b = temp;
}

bool BBox::BoxIntersection(vec3 start, vec3 dir, Intersection& boxIntersection){
    float hit1, hit2;
    float txmax, txmin, tymax, tymin, tzmax, tzmin;
    float invx = 1/dir.x, invy = 1/dir.y, invz = 1/dir.z;

    txmin = (min.x - start.x)*invx;
    txmax = (max.x - start.x)*invx;
    if (txmin > txmax)
        swap(txmin, txmax);

    tymin = (min.y - start.y)*invy;
    tymax = (max.y - start.y)*invy;
    if (tymin > tymax)
        swap(tymin, tymax);
    
    if (txmax < 0 || tymax < 0)
        return false;

    if (txmin > tymax || tymin > txmax)
        return false; // ray misses, no need to check z
    
    hit1 = std::fmax(txmin, tymin);
    hit2 = std::fmin(txmax, tymax);
    
    // tminz and tzmax calculation
    tzmin = (min.z - start.z)*invz;
    tzmax = (max.z - start.z)*invz;
    if (tzmin > tzmax)
        swap(tzmin, tzmax);
    
    if (tzmax < hit1-EPSILON || tzmin > hit2+EPSILON)
        return false; //ray misses cube in z dimension

    hit1 = std::fmax(hit1, tzmin);
    hit2 = std::fmin(hit2, tzmax);

    if (hit2 > 0.f)
        hit1 = std::fmax(hit1,0.f);

    boxIntersection.position = start + dir * hit1;
    boxIntersection.distance = hit1;

    return (boxIntersection.distance >= 0.f);
}

int BBox::longestaxis(){
    float x_axis = max.x - min.x;
    float y_axis = max.y - min.y;
    float z_axis = max.z - min.z;
    float max = std::fmax(x_axis, std::fmax(y_axis,z_axis));
    if (max == x_axis){
        return 0;
    }
    if (max == y_axis){
        return 1;
    }
    if (max == z_axis){
        return 2;
    }
    return -1;
}
