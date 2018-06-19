#include "Kdtree.hpp"

bool KD_Tree::intersect(bool(*fptr)(const vec3&,const vec3&,const vector<Triangle>&,Intersection&,int),
                   vec3 start, vec3 dir, Intersection &i, int exclude) {
    return intersectHelper(fptr, root, start, dir, i, exclude);
}

bool KD_Tree::intersectHelper(bool(*fptr)(const vec3&,const vec3&,const vector<Triangle>&,Intersection&,int),
                   KD_Node *node, vec3 start, vec3 dir,
                   Intersection &i, int exclude) {
    Intersection temp;
    if (!node->bounding_box.BoxIntersection(start, dir, temp))
        return false;
    temp.distance = FMAX;

    if (node->leaf) {
        if ((*fptr)(start, dir, node->triangles, temp, exclude)) {
            if (temp.distance < i.distance) {
                i = temp;
                return true;
            }
        }
        return false;
    }

    Intersection left = i, right = i;
    bool leftFlag = false, rightFlag = false;
    leftFlag = intersectHelper(fptr, node->leftchild, start, dir, left, exclude);
    rightFlag = intersectHelper(fptr, node->rightchild, start, dir, right, exclude);
    if (!leftFlag && !rightFlag)
        return false;
    else if (!leftFlag)
        i = right;
    else if (!rightFlag)
        i = left;
    else
        i = left.distance < right.distance ? left : right;
    return true;
}
