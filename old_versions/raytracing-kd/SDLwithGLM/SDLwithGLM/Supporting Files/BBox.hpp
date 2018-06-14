//
//  BBox.hpp
//  SDLwithGLM
//
//

#ifndef BBox_hpp
#define BBox_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#endif /* BBox_hpp */
const double EPSILON = 1e-20;
const int MAX =  +std::numeric_limits<int>::max();
const int MIN =  -std::numeric_limits<int>::max();
struct Intersection
{
    glm::vec3 position;
    float distance;
    int index;
};
class BBox{
public:
   
    glm::vec3 min;
    glm::vec3 max;
    BBox(){}
    BBox (glm::vec3 &vmin, glm::vec3 &vmax){
        
        min = vmin;
        max = vmax;
        
    }
    int quantize(float ind) {
        if(ind < EPSILON && ind > -EPSILON)
            return 0;
        if(ind >= EPSILON)
            return 1;
        return -1;
    }
    glm::vec3 minmax(int x) const{
        if(x == 0) return min;
        return max;
    }
    bool insider(glm::vec3 ray) {
        if (quantize(ray.x - min.x) < 0 || quantize(ray.x - max.x) > 0)
            return false;
        if (quantize(ray.y - min.y) < 0 || quantize(ray.y - max.y) > 0)
            return false;
        if (quantize(ray.z - min.z) < 0 || quantize(ray.z - max.z) > 0)
            return false;
        return true;
    }
    
    bool BoxIntersection(glm::vec3 start, glm::vec3 dir, Intersection& boxIntersection){
        //finding txmin and tymin
        float hit1, hit2;
        //std::cout<< "min.y " << min.y <<std::endl;
       // std::cout<< "start.y " << start.y <<std::endl;
       // std::cout<< "dir.y " << dir.y <<std::endl;
        glm::vec3 invdir = 1.f/ dir;
        float tmin,tmax, txmax, txmin, tymax, tymin, tzmax, tzmin;
        /*
       
        float t00,t01,t10,t11,t20,t21,t0min,t0max,t1min,t1max,t2min,t2max,tmin,tmax, txmax, txmin, tymax, tymin, tzmax, tzmin;
        tmin = MAX;
        tmax = MIN;
        if (quantize(dir.y) != 0) {
            
            t00 = (min.y - start.y) / dir.y;
            t01 = (max.y - start.y) / dir.y;
            t0min = std::min(t00, t01);
            t0max = std::max(t00, t01);
            tmin = t0min;
            tmax = t0max;
        
            
        }
        if (quantize(dir.x) != 0) {
            
            t10 = (min.x - start.x) / dir.x;
            t11 = (max.x - start.x) / dir.x;
            t1min = std::min(t10, t11);
            t1max = std::max(t10, t11);
            tmin = std::max(tmin, t1min);
            tmax = std::min(tmax, t1max);
        }
        if (quantize(dir.z) != 0) {
            
            t20 = (min.z - start.z) / dir.z;
            t21 = (max.z - start.z) / dir.z;
            t2min = std::min(t20, t21);
            t2max = std::max(t20, t21);
            tmin = std::max(tmin, t2min);
            tmax = std::min(tmax, t2max);
        }
        if (quantize(tmin - tmax) < 0 && quantize(tmax - 0) > 0)
            return true;
        return false;*/
        
            
           // if ( quantize(dir.y) != 0){
            tymin = (min.y - start.y)/dir.y;
            tymax = (max.y - start.y)/dir.y;
                
            //}
           // finding txmax and tymax
            //if ( quantize(dir.x) != 0){
            txmin = (min.x - start.x)/dir.x;
            txmax = (max.x - start.x)/dir.x;
            //}
            
            if (txmin > txmax){
                std::swap(txmin, txmax);
            }
            if (tymin > tymax){
                std::swap(tymin, tymax);
            }
            
            if ( txmin > tymax || tymin > txmax){
                return false; //ray misses non need to check z
            }
            
            if (txmin < tymin){ //getting the position of the first time ray enters the cube
                
                hit1 = tymin;
                
            }else{
                hit1 = txmin;
            }
            if (tymax < txmax){ //getting the second hit
                
                hit2 = tymax;
                
            }else{
                hit2 = txmax;
            }
            
            // tminz and tzmax calculation
        
            tzmax = (max.z - start.z)/dir.z;
            tzmin = (min.z - start.z)/dir.z;
        
        
            if (tzmin > tzmax){
                std::swap(tzmin, tzmax);
            }
            
            if ( tzmax < hit1 || tzmin > hit2  ){
                return false; //ray misses cube in z dimension
            }
            if (hit1 < tzmin){
                hit1 = tzmin;
            }
            if (hit2 > tzmax){
                hit2 = tzmax;
            }
            if (hit1 > hit2){
                hit1 = hit2;
            }
            if ( hit1 < 0){
                hit1= hit2; //secondary ray
            }
            
            boxIntersection.position = start + dir * hit1;
            boxIntersection.distance = hit1;
        
            return (boxIntersection.distance > 0.f);
         
            
    }
    
    int longestaxis(){
        float x_axis = max.x - min.x;
        float y_axis = max.y - min.y;
        float z_axis = max.z - min.z;
        float max = std::max(x_axis, std::max(y_axis,z_axis));
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
    
    
};



