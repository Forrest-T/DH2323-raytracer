void __kernel mixColors(float4 tl, float4 tr, float4 bl, float4 br, __global float4 *out, int width, int height) {
    int index = get_global_id(0);
    int x = index % width;
    int y = index / width;
    float4 left  = mix(tl, bl, ((float)(y))/(height-1));
    float4 right = mix(tr, br, ((float)(y))/(height-1));
    out[index] = mix(left, right, ((float)(x))/(width-1));
}

constant float4 green = (float4)(0,1,0,0);
constant float4 blue  = (float4)(0,0,1,0);

// This produces a smooth gradient, as it should
void __kernel mixColors2(float4 tl, float4 tr, float4 bl, float4 br, __global float4 *out, int width, int height) {
    int index = get_global_id(0);
    int y = index / width;
    float f = (float)y/(height-1);
    out[index] = mix(green,blue,f);
}

// Why the hell does this produce black???
void __kernel mixColors3(float4 tl, float4 tr, float4 bl, float4 br, __global float4 *out, int width, int height) {
    int index = get_global_id(0);
    float f = 0.5;
    out[index] = mix(green,blue,f);
}
