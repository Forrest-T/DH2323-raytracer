#ifndef KD_Node_hpp
#define KD_Node_hpp

#include <glm/glm.hpp>
using glm::vec3;

#include <vector>
using std::vector;

#include "model.hpp"
#include "structs.hpp"

typedef enum dimension {X, Y, Z} Dimension;

class KD_Node {
public:
    KD_Node* leftchild;
    KD_Node* rightchild;
    BBox bounding_box;
    vector<Triangle> triangles;
    Dimension dimension;
    bool leaf;
};

class KD_Tree {
public:
    KD_Node *root;
    // TODO: minimum splitting split cutoff

    KD_Tree(const vector<Triangle> &trg) { root = build(trg); }
    ~KD_Tree() { delete root; }

private:
    static KD_Node *build(vector<Triangle> trg){
        if (trg.empty())
            return nullptr;
        vector<Triangle> leftTriangles;
        vector<Triangle> rightTriangles;
        vec3 midpoint(0);
        vec3 minp(std::numeric_limits<float>::max());
        vec3 maxp(std::numeric_limits<float>::min());

        // GET OVERALL BOUNDING BOX
        for (const auto &t : trg) {
            vec3 localmin(t.boundingBox.min);
            vec3 localmax(t.boundingBox.max);
            
            minp.x = std::min(minp.x, localmin.x);
            minp.y = std::min(minp.y, localmin.y);
            minp.z = std::min(minp.z, localmin.z);
            
            maxp.x = std::max(maxp.x, localmax.x);
            maxp.y = std::max(maxp.y, localmax.y);
            maxp.z = std::max(maxp.z, localmax.z);
        }
        BBox bb(minp,maxp);
        for (const auto &t : trg)
            midpoint += t.midpoint;
        midpoint /= static_cast<float>(trg.size());
        int longest = bb.longestaxis();
        switch(longest){
            case 0:
                for (const auto &t : trg) {
                    if (t.boundingBox.max.x >= midpoint.x)
                        rightTriangles.push_back(t);
                    if (t.boundingBox.min.x <= midpoint.x)
                        leftTriangles.push_back(t);
                }
                break;
            case 1:
                for (const auto &t : trg) {
                    if (t.boundingBox.max.y >= midpoint.y)
                        rightTriangles.push_back(t);
                    if (t.boundingBox.min.y <= midpoint.y)
                        leftTriangles.push_back(t);
                }
                break;
            case 2:
                for (const auto &t : trg) {
                    if (t.boundingBox.max.z >= midpoint.z)
                        rightTriangles.push_back(t);
                    if (t.boundingBox.min.z <= midpoint.z)
                        leftTriangles.push_back(t);
                }
                break;
        }
        KD_Node* node = new KD_Node();
        node->bounding_box = bb;

        if (leftTriangles.size() == trg.size()
                || rightTriangles.size() == trg.size()
                || leftTriangles.empty()
                || rightTriangles.empty()) {
            node->leftchild = nullptr;
            node->rightchild = nullptr;
            node->triangles = trg;
            node->leaf = true;
            return node;
        }

        node->leaf = false;
        node->leftchild = build(leftTriangles);
        node->rightchild = build(rightTriangles);
        return node;
    }
};

#endif
