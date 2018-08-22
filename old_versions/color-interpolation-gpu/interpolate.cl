void __kernel mixColors(float4 tl, float4 tr, float4 bl, float4 br, __global float4 *out, int width, int height) {
    int index = get_global_id(0);
    int x = index % width;
    int y = index / width;
    float4 left  = mix(tl, bl, ((float)(y))/(height-1));
    float4 right = mix(tr, br, ((float)(y))/(height-1));
    out[index] = mix(left, right, ((float)(x))/(width-1));
}
