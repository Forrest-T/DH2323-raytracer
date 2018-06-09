__kernel void multiply(int a, int b, global int *out) {
    int index = get_global_id(0);
    out[index] = a*b;
}
