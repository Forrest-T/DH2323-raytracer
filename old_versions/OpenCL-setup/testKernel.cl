

__kernel void multiply(__global int *a,
                       __global int *b,
                       __global int *out) {
    int index = get_global_id(0);
    out[index] = a[index]*b[index];
}
