typedef enum state {
    DEFAULT,
    CHECK_LEFT,
    CHECK_RIGHT,
    RETURN_LEFT,
    RETURN_RIGHT
} State;
typedef struct {
    Intersection temp;
    Intersection left;
    Intersection right;
    int node;
    State state;
    bool leftFlag;
    bool rightFlag;
} stackFrame;

bool bboxIntersection(Ray *r, __global BBox *b);
bool calculateIntersection(Intersection *i, Ray *r, __global Triangle *t, __global Triangle  *skip);
void treeIntersection(__global struct kdnode *tree, Intersection *i, Ray *r, __global Triangle *t, __global Triangle *skip);
float4 calculateColor(__global struct kdnode *tree, Intersection *i, Light *l, float4 cam, __global Triangle *triangles, int num_triangles);

constant float4 black = (float4)(0,0,0,0);

void swap(__private float *a, __private float *b) {
    float c = *a;
    *a = *b;
    *b = c;
}

bool bboxIntersection(Ray *r, __global BBox *b) {
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

    hit1 = fmax(hit1, tzmin);
    hit2 = fmin(hit2, tzmax);

    if (hit2 > 0.f)
        hit1 = fmax(hit1,0.f);

    return (hit1 >= 0.f);
}

void treeIntersection(__global struct kdnode *tree,
                      Intersection *i,
                      Ray *r,
                      __global Triangle *t,
                      __global Triangle *skip) {
    // TODO calculate tree depth at runtime?
    stackFrame stack[100];
    stack[0].node = 0;
    stack[0].state = DEFAULT;
    int stackptr = 0;
    do {
    if (       stack[stackptr].state == DEFAULT 
            || stack[stackptr-1].state == CHECK_LEFT
            || stack[stackptr-1].state == CHECK_RIGHT) {
        stack[stackptr].temp.distance = FLT_MAX;
        if (tree[stack[stackptr].node].num_triangles > 0) { // leaf node
            for (int ti = 0; ti < tree[stack[stackptr].node].num_triangles; ti++)
                if (calculateIntersection(&(stack[stackptr].temp), r, &(t[ti+tree[stack[stackptr].node].triangle_begin]), skip)) {
                    if (stack[stackptr].state == DEFAULT) {
                        if (stack[stackptr].temp.distance < i->distance)
                            *i = stack[stackptr].temp;
                    } else if ( stack[stackptr-1].state == CHECK_LEFT && stack[stackptr].temp.distance < stack[stackptr-1].left.distance) {
                                stack[stackptr-1].left = stack[stackptr].temp;
                                stack[stackptr-1].leftFlag = true;
                                stack[stackptr-1].state = RETURN_LEFT;
                    } else if ( stack[stackptr-1].state == CHECK_RIGHT && stack[stackptr].temp.distance < stack[stackptr-1].right.distance) {
                                stack[stackptr-1].right = stack[stackptr].temp;
                                stack[stackptr-1].rightFlag = true;
                                stack[stackptr-1].state = RETURN_RIGHT;
                    }
                }
            stackptr--;
        } else { // intermediate node
            if (stackptr == 0)
                stack[stackptr].left = *i;
            else if (stack[stackptr-1].state == CHECK_LEFT)
                stack[stackptr].left = stack[stackptr-1].left;
            else
                stack[stackptr].left = stack[stackptr-1].right;
            stack[stackptr].leftFlag = false;
            if (bboxIntersection(r, &(tree[stack[stackptr].node].box))) {
                // "recursive" call to the left child
                stack[stackptr].state = CHECK_LEFT;
                stackptr++;
                stack[stackptr].node = tree[stack[stackptr-1].node].left_index;
            } else {
                stack[stackptr].state = RETURN_LEFT;
            }
        }
    } else if (stack[stackptr].state == RETURN_LEFT) {
            if (stackptr == 0)
                stack[stackptr].right = *i;
            else if (stack[stackptr-1].state == CHECK_LEFT)
                stack[stackptr].right = stack[stackptr-1].left;
            else
                stack[stackptr].right = stack[stackptr-1].right;
        stack[stackptr].rightFlag = false;
        if (bboxIntersection(r, &(tree[stack[stackptr].node].box))) {
            // "recursive" call to the right child
            stack[stackptr].state = CHECK_RIGHT;
            stackptr++;
            stack[stackptr].node = tree[stack[stackptr-1].node].left_index;
        } else {
            stack[stackptr].state = RETURN_RIGHT;
        }
    } else if (stack[stackptr].state == RETURN_RIGHT) {
        Intersection *intersection;
        if (stackptr == 0)
            intersection = i;
        else if (stack[stackptr-1].state == CHECK_LEFT)
            intersection = &(stack[stackptr-1].left);
        else
            intersection = &(stack[stackptr-1].right);
        if (!stack[stackptr].leftFlag && stack[stackptr].rightFlag)
            *intersection = stack[stackptr].right;
        else if (!stack[stackptr].rightFlag && stack[stackptr].leftFlag)
            *intersection = stack[stackptr].left;
        else if (stack[stackptr].rightFlag && stack[stackptr].leftFlag)
            *intersection = stack[stackptr].left.distance < stack[stackptr].right.distance ?
                stack[stackptr].left : stack[stackptr].right;
        stackptr--;
    }
    } while (stackptr >= 0);
}

bool calculateIntersection(Intersection *i,
                            Ray *r,
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
                      Intersection *i,
                      Light *l,
                      float4 cam,
                      __global Triangle *triangles,
                      int num_triangles) {
    if (i->triangle == NULL) return black;
    cam -= i->position;
    float4 norm = i->triangle->normal;
    float4 light = l->position - i->position;
    if (dot(norm,light) < 0) norm *= -1;
    float dist = length(light);
    Intersection i2 = { .triangle = NULL, .distance = dist};
    Ray r = {i->position + FLT_EPSILON * light, normalize(light)};
    for (int j = 0; j < num_triangles; j++)
        treeIntersection(tree, &i2, &r, &triangles[j], i->triangle);
    if (dot(cam,norm) < 0 || i2.distance < dist)
        light = black;
    else
        light = (l->color * max(dot(norm,light),0.f)/(4*M_PI_F*dist*dist));
    return i->triangle->color * (l->glob + light);
}

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
    Intersection i = { .triangle = NULL, .distance = FLT_MAX };
    Ray r = {camera, (float4)(x-width/2,y-width/2,focal,0)};
    r.direction = normalize(r.direction);
    for (int j = 0; j < num_triangles; j++)
        treeIntersection(flat_tree, &i, &r, &triangles[j], NULL);
    out[x+y*width] = calculateColor(flat_tree, &i, &light, camera, triangles, num_triangles);
}
