
//constant float4 black = (float4)(0,0,0,0);
//
//void calculateIntersection(Intersection *i,
//                            Ray *r,
//                            __global Triangle *t,
//                            __global Triangle  *skip) {
//    if (t == skip) return;
//    /* Muller-Trömbore algorithm */
//    float u, v, time;
//    float4 e1 = t->v1 - t->v0;
//    float4 e2 = t->v2 - t->v0;
//    float4 pvec = cross(r->direction, e2);
//    float det = dot(pvec, e1);
//    if (fabs(det) < FLT_EPSILON) return;
//    float invdet = 1.0f/det;
//    float4 tvec = r->origin - t->v0;
//    u = dot(tvec, pvec) * invdet;
//    if (u < 0 || u > 1) return;
//    float4 qvec = cross(tvec, e1);
//    v = dot(r->direction, qvec) * invdet;
//    if (v < 0 || u+v > 1) return;
//    time = dot(e2, qvec) * invdet;
//    if (time <= 0 || time >= i->distance) return;
//    i->position = r->origin + time*r->direction;
//    i->triangle = t;
//    i->distance = time;
//}
//
//float4 calculateColor(Intersection *i,
//                      Light *l,
//                      float4 cam,
//                      __global Triangle *triangles,
//                      int num_triangles) {
//    if (i->triangle == NULL) return black;
//    cam -= i->position;
//    float4 norm = i->triangle->normal;
//    float4 light = l->position - i->position;
//    if (dot(norm,light) < 0) norm *= -1;
//    float dist = length(light);
//    Intersection i2 = { .triangle = NULL, .distance = dist};
//    Ray r = {i->position + FLT_EPSILON * light, normalize(light)};
//    for (int j = 0; j < num_triangles; j++)
//        calculateIntersection(&i2, &r, &triangles[j], i->triangle);
//    if (dot(cam,norm) < 0 || i2.distance < dist)
//        light = black;
//    else
//        light = (l->color * max(dot(norm,light),0.f)/(4*M_PI_F*dist*dist));
//    return i->triangle->color * (l->glob + light);
//}

__kernel void tracePixel(__global struct kdnode* flat_tree,
                         __global Triangle *triangles,
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
}

//__kernel void tracePixel(__global Triangle *triangles,
//                         int num_triangles,
//                         Light light,
//                         float4 camera,
//                         int focal,
//                         mat4 R,
//                         int width,
//                         global float4 *out) {
//    int id = get_global_id(0);
//    int x = id % width;
//    int y = id / width;
//    Intersection i = { .triangle = NULL, .distance = FLT_MAX };
//    Ray r = {camera, (float4)(x-width/2,y-width/2,focal,0)};
//    r.direction = normalize(r.direction);
//    //r.direction = mat4VMult(R,r.direction);
//    //r.direction = mat4VMult2(r.direction,R);
//    for (int j = 0; j < num_triangles; j++)
//        calculateIntersection(&i, &r, &triangles[j], NULL);
//    out[x+y*width] = calculateColor(&i, &light, camera, triangles, num_triangles);
//}
