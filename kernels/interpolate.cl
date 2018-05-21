kernel void interpolate(float3 a, float3 b,
                        int width, global float3 *out) {
    int index = get_global_id(0);
    float c = (index%width)/(float)width;
    out[index] = (1-c)*a+c*b;
}
