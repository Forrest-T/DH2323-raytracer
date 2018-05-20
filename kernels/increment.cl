__kernel void increment(int in, __global int* out) {
    out[0] = in + 1;
}
