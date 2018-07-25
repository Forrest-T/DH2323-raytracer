#ifndef KD_ARRAY_H_
#define KD_ARRAY_H_

#include <vector>
#include "Kdtree.hpp"
#include "structs.hpp"
using std::vector;

namespace Raytracer {
    /* Traverses the kd tree and encodes it into a vector of kd-nodes for
     *  use on the GPU (where recursion doesn't exist)
     */
    void array_ify(KD_Tree *, vector<struct kdnode> &, vector<struct triangle> &);
    void array_ify(int &, KD_Node *, vector<struct kdnode> &, vector<struct triangle> &);

    inline cl_float4 vec3_to_float4(vec3 v) { return {{v.x, v.y, v.z, 0.f}}; }
}
#endif
