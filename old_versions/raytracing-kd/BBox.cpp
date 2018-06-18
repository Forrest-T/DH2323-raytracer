#include "BBox.hpp"

int BBox::quantize(float ind) {
    if(ind < EPSILON && ind > -EPSILON)
        return 0;
    if(ind >= EPSILON)
        return 1;
    return -1;
}
