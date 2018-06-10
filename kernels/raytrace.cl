float4 calculateColor(Intersection *i, Light *l) {
    if (i->triangle == NULL) return (float4)(0,0,0,0);
    float4 norm = i->triangle->normal;
    float4 light = l->position - i->position;
    float dist = dot(light,light);
    return l->intensity * (i->triangle->color * l->color * max(dot(norm,light),0.f)/(1*M_PI_F*dist));
}

void calculateIntersection(Intersection *i, Ray *r, __global Triangle *t) {
    float time, u, v, f, distance;
    float4 e1 = t->v1 - t->v0;
    float4 e2 = t->v2 - t->v0;
    if (fabs(dot(r->direction, cross(e1, e2))) < FLT_EPSILON) return;
    float4 s = r->origin - t->v0;
    float4 c = cross(r->direction, e2);
    f = 1.0/dot(c,e1);
    u = f * dot(c,s);
    if (u < 0 || u > 1) return;
    c = cross(s, e1);
    v = f * dot(c,r->direction);
    if (v < 0 || v+u > 1) return;
    time = f*dot(c,e2);
    if (time <= 0) return;
    distance = time*length(r->direction);
    if (distance >= i->distance) return;
    i->position = r->origin + time*r->direction;
    i->triangle = t;
    i->distance = distance;
}

__kernel void tracePixel(__global Triangle *triangles,
                         int num_triangles,
                         Light light,
                         float4 camera,
                         int focal,
                         mat4 R,
                         int width,
                         global float4 *out) {
    int id = get_global_id(0);
    int x = id % width;
    int y = id / width;
    Intersection i = { .triangle = NULL, .distance = FLT_MAX };
    Ray r = {camera, (float4)(x-width/2,y-width/2,focal,0)};
    r.direction = normalize(r.direction);
    //r.direction = mat4VMult(R,r.direction);
    //r.direction = mat4VMult2(r.direction,R);
    for (int j = 0; j < num_triangles; j++)
        calculateIntersection(&i, &r, &triangles[j]);
    calculateIntersection(&i, &r, &triangles[0]);
    out[x+y*width] = calculateColor(&i, &light);
}
