#include <vector>
#include "../include/Kdtree.hpp"
#include "../include/KdArray.hpp"
#include "../include/structs.hpp"

namespace Raytracer {
    void array_ify(KD_Tree *tree,
                   vector<struct kdnode> &tree_flat,
                   vector<struct triangle> &triangles) {
        int index = 0;
        array_ify(index, tree->root, tree_flat, triangles);
    }
    void array_ify(int &index, KD_Node *node,
                   vector<struct kdnode> &tree_flat,
                   vector<struct triangle> &triangles) {
        if (node->leaf) {
            struct kdnode leaf;
            leaf.box = { vec3_to_float4(node->bounding_box.min),
                         vec3_to_float4(node->bounding_box.max)};
            leaf.left_index = 0;
            leaf.right_index = 0;
            leaf.num_triangles = node->triangles.size();
            leaf.triangle_begin = triangles.size();
            for (const auto &t : node->triangles)
                triangles.push_back({vec3_to_float4(t->v0),
                                     vec3_to_float4(t->v1),
                                     vec3_to_float4(t->v2),
                                     vec3_to_float4(t->normal),
                                     vec3_to_float4(t->color),
                                     {vec3_to_float4(t->boundingBox.min),
                                      vec3_to_float4(t->boundingBox.max)}
                                     });
            index++;
            tree_flat.push_back(leaf);
        } else {
            // recursive case
            struct kdnode inner;
            inner.box = {vec3_to_float4(node->bounding_box.min),
                         vec3_to_float4(node->bounding_box.max)};
            inner.num_triangles = 0;
            inner.triangle_begin = 0;
            index++;
            inner.left_index = index;
            array_ify(index, node->leftchild, tree_flat, triangles);
            inner.right_index = index;
            array_ify(index, node->rightchild, tree_flat, triangles);
            tree_flat.push_back(inner);
        }
    }
}
