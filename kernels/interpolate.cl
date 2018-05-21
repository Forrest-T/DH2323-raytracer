__kernel void interpolate(float4 a, float4 b, float4 c, float4 d,
                        int width, int height, global float4 *out) {
    int index = get_global_id(0);
    float x = (index%width)/(float)(width-1);
    float y = (index/width)/(float)(height-1);
    out[index] = (1-y)*((1-x)*a+x*b)+y*((1-x)*c+x*d);
}
