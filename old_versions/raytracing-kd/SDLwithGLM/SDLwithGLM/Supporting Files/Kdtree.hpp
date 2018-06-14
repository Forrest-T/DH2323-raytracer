//
//  Kdtree.hpp
//  SDLwithGLM
//

//

#ifndef Kdtree_hpp
#define Kdtree_hpp

#include <stdio.h>

#include "TestModel.h"
#endif /* Kdtree_hpp */
class Kdtree{
    
public:
    
    BBox bounding_box;
    Kdtree* leftchild;
    Kdtree* rightchild;
    std::vector<Triangle> triangles;
    
    
    Kdtree(){}
    
    Kdtree* buildTree(std::vector<Triangle> &trg){
        std::vector<Triangle> leftTriangles;
        std::vector<Triangle> rightTriangles;
        glm::vec3 midpoint;
        glm::vec3 minp(std::numeric_limits<float>::max());
        glm::vec3 maxp(std::numeric_limits<float>::min());
        Kdtree* node = new Kdtree();
        node->bounding_box;
        node->leftchild = NULL;
        node->rightchild = NULL;
        node->triangles = trg;
        
        if (trg.size() == 0){
            return node;
        }
        if (trg.size() == 1){
            trg[0].triangleBBox();
            node->bounding_box = trg[0].boundingBox;
            node->leftchild = new Kdtree();
            node->rightchild = new Kdtree();
            node->leftchild->triangles;
            node->rightchild->triangles;
        }
        //GET OVERALL BOUNDING BOX
        for (int i=0; i < trg.size(); i++){
            trg[i].triangleBBox();
            glm::vec3 localmin = trg[i].boundingBox.min;
            glm::vec3 localmax = trg[i].boundingBox.max;
            
            minp.x = std::min( minp.x, localmin.x);
            minp.y = std::min( minp.y, localmin.y);
            minp.z = std::min( minp.z, localmin.z);
            
            maxp.x = std::max( maxp.x, localmax.x);
            maxp.y = std::max( maxp.y, localmax.y);
            maxp.z = std::max( maxp.z, localmax.z);
            
        }
        node->bounding_box =  BBox(minp,maxp);
        for (int i=0; i< trg.size(); i++){
            //trg[i].ComputeNormal();
            midpoint = midpoint + (trg[i].midpoint /(float)trg.size());
            
        }
        int longest = node->bounding_box.longestaxis();
        for (int i=0; i < trg.size();i++){
            switch(longest){
                case 0:
                    midpoint.x >= trg[i].midpoint.x ? rightTriangles.push_back(trg[i]) : leftTriangles.push_back(trg[i]);
                    break;
                case 1:
                    midpoint.y >= trg[i].midpoint.y ? rightTriangles.push_back(trg[i]) : leftTriangles.push_back(trg[i]);
                    break;
                case 2:
                    midpoint.z >= trg[i].midpoint.z ? rightTriangles.push_back(trg[i]) : leftTriangles.push_back(trg[i]);
                    break;
            }
        }
        
        if (leftTriangles.size()> 0 && rightTriangles.size() != 0) {
            node->leftchild = buildTree(leftTriangles);
        }
        if (leftTriangles.size()!= 0 && rightTriangles.size() > 0) {
            node->rightchild = buildTree(rightTriangles);
        }
       
        
        return node;
    }
    
   
    
    
};
