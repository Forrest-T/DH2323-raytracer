constant float4 black = (float4)(0,0,0,0);

void swap(__private float *, __private float *);
bool bboxIntersection(__private Ray *, __private BBox *, __private Intersection *);
bool calculateIntersection(__private Intersection *, __private Ray *, __global Triangle *, __global Triangle  *);
void treeIntersection(__global struct kdnode *, __private Intersection *, __private Ray *, __global Triangle *, __global Triangle *);
float4 calculateColor(__global struct kdnode *, __private Intersection *, __private Light *, float4, __global Triangle *);

void swap(__private float *a, __private float *b) {
    float c = *a;
    *a = *b;
    *b = c;
}

bool bboxIntersection(__private Ray *r, __private BBox *b, __private Intersection *i) {
    float hit1, hit2;
    float txmax, txmin, tymax, tymin, tzmax, tzmin;
    float invx = 1/(r->direction.x),
          invy = 1/(r->direction.y),
          invz = 1/(r->direction.z);

    txmin = (b->min.x - r->origin.x)*invx;
    txmax = (b->max.x - r->origin.x)*invx;
    if (txmin > txmax)
        swap(&txmin, &txmax);

    tymin = (b->min.y - r->origin.y)*invy;
    tymax = (b->max.y - r->origin.y)*invy;
    if (tymin > tymax)
        swap(&tymin, &tymax);
    
    if (txmax < 0 || tymax < 0)
        return false;

    if (txmin > tymax || tymin > txmax)
        return false; // ray misses, no need to check z
    
    hit1 = fmax(txmin, tymin);
    hit2 = fmin(txmax, tymax);
    
    // tminz and tzmax calculation
    tzmin = (b->min.z - r->origin.z)*invz;
    tzmax = (b->max.z - r->origin.z)*invz;
    if (tzmin > tzmax)
        swap(&tzmin, &tzmax);
    
    if (tzmax < hit1 - FLT_EPSILON || tzmin > hit2 + FLT_EPSILON)
        return false; //ray misses cube in z dimension

    hit1 = fmax(fmax(hit1, tzmin), 0.f);
    hit2 = fmin(hit2, tzmax);
    if (hit1 > i->distance)
        return false;
    
    i->position = r->origin + r->direction*hit1;
    i->distance = hit1;

    return true;
}

void treeIntersection(__global struct kdnode *tree,
                      __private Intersection *i,
                      __private Ray *r,
                      __global Triangle *t,
                      __global Triangle *skip) {
    struct kdnode stack[100];
    struct kdnode *stackptr = stack;
    stackptr++;
    *stackptr = tree[0];
    do {
        if (stackptr->num_triangles > 0) {
            for (int ti = 0; ti < stackptr->num_triangles; ti++)
                calculateIntersection(i, r, t+ti+stackptr->triangle_begin, skip);
            stackptr--;
        } else {
            Intersection li = { .triangle = NULL, .distance = i->distance };
            Intersection ri = { .triangle = NULL, .distance = i->distance };
            struct kdnode left = tree[stackptr->left_index];
            struct kdnode right = tree[stackptr->right_index];
            stackptr--;
            if (bboxIntersection(r,&right.box,&ri)) *(++stackptr) = right;
            if (bboxIntersection(r,&left.box,&li))  *(++stackptr) = left;
        }
    } while (stackptr != stack);
}

bool calculateIntersection(__private Intersection *i,
                           __private Ray *r,
                           __global Triangle *t,
                           __global Triangle  *skip) {
    if (t == skip) return false;
    /* Muller-Trömbore algorithm */
    float u, v, time;
    float4 e1 = t->v1 - t->v0;
    float4 e2 = t->v2 - t->v0;
    float4 pvec = cross(r->direction, e2);
    float det = dot(pvec, e1);
    if (fabs(det) < FLT_EPSILON) return false;
    float invdet = 1.0f/det;
    float4 tvec = r->origin - t->v0;
    u = dot(tvec, pvec) * invdet;
    if (u < 0 || u > 1) return false;
    float4 qvec = cross(tvec, e1);
    v = dot(r->direction, qvec) * invdet;
    if (v < 0 || u+v > 1) return false;
    time = dot(e2, qvec) * invdet;
    if (time <= 0 || time >= i->distance) return false;
    i->position = r->origin + time*r->direction;
    i->triangle = t;
    i->distance = time;
    return true;
}

float4 calculateColor(__global struct kdnode *tree,
                      __private Intersection *i,
                      __private Light *l,
                      float4 cam,
                      __global Triangle *triangles) {
    if (i->triangle == NULL) return (float4)(0,0,.3,0);
    cam -= i->position;
    float4 norm = i->triangle->normal;
    float4 light = l->position - i->position;
    if (dot(norm,light) < 0) norm *= -1;
    float dist = length(light);
    Intersection i2 = { .triangle = NULL, .distance = dist};
    Ray r = {i->position + 0.005f * light, normalize(light)};
    treeIntersection(tree, &i2, &r, triangles, i->triangle);
    if (dot(cam,norm) < 0 || i2.distance < dist)
        light = black;
    else
        light = (l->color * max(dot(norm,light),0.f)/(4*M_PI_F*dist*dist));
    return i->triangle->color * (l->glob + light);
}

__kernel void tracePixel(__global struct kdnode* flat_tree,
                         __global Triangle *triangles,
                         Light light,
                         float4 camera,
                         int focal,
                         mat4 R,
                         int width,
                         __global float4 *out) {
    int id = get_global_id(0);
    int x = id % width;
    int y = id / width;
    Intersection i = { .triangle = NULL, .distance = FLT_MAX };
    Ray r = {camera, (float4)(x-width/2,y-width/2,focal,0)};
    r.direction = normalize(mat4VMult2(r.direction,R));
    treeIntersection(flat_tree, &i, &r, triangles, NULL);
    out[x+y*width] = calculateColor(flat_tree, &i, &light, camera, triangles);
}
