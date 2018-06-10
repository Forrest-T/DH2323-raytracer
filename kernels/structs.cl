#ifndef STRUCTS_H_
#define STRUCTS_H_

typedef struct triangle {
    float4 v0;
    float4 v1;
    float4 v2;
    float4 normal;
    float4 color;
} Triangle;

typedef struct ray {
    float4 origin;
    float4 direction;
} Ray;

typedef struct light {
    float4 position;
    float4 color;
    float4 intensity;
} Light;

typedef struct intersection {
    float4 position;
    __global Triangle *triangle;
    float distance;
} Intersection;

#endif
