#include <glm/glm.hpp>
#include <iostream>
#include <iterator>
#include <SDL.h>

#include "SDL_helper.hpp"
#include "raytracer.hpp"
#include "Triangle.hpp"
#include "BBox.hpp"
#include "model.hpp"

using std::cout;
using std::endl;
using std::vector;
using glm::vec3;
using glm::mat3;

int main() {
    screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, false);
    LoadTestModel(triangles);
    t = SDL_GetTicks();    // Set start value for timer.

    while(NoQuitMessageSDL()) {
        update();
        draw();
    }

    SDL_SaveBMP(screen, "screenshot.bmp");
    return 0;
}

void update() {
    // Compute frame time:
    int t2 = SDL_GetTicks();
    float dt = float(t2-t);
    t = t2;
    cout << "Render time: " << dt << " ms." << endl;
    Uint8 *keystate = SDL_GetKeyState(0);
    // update appropriate camera fields if key press is detected
    if (keystate[SDLK_UP])
        camera.z += camera_speed;
    if (keystate[SDLK_DOWN])
        camera.z -= camera_speed;
    if (keystate[SDLK_LEFT])
        yaw -= yaw_speed;
    if (keystate[SDLK_RIGHT])
        yaw += yaw_speed;
    if (keystate[SDLK_w])
        light_pos.z += light_speed;
    if (keystate[SDLK_s])
        light_pos.z -= light_speed;
    if (keystate[SDLK_d])
        light_pos.x += light_speed;
    if (keystate[SDLK_a])
        light_pos.x -= light_speed;
    if (keystate[SDLK_q])
        light_pos.y += light_speed;
    if (keystate[SDLK_e])
        light_pos.y -= light_speed;
    // calculate the rotation matrix in the y-direction
    R[0][0] = R[2][2] = cos(yaw);
    R[0][2] = -(R[2][0] = sin(yaw));
}

void draw() {
    tree = new KD_Tree(triangles);
    if(SDL_MUSTLOCK(screen))
        SDL_LockSurface(screen);

    Intersection i;

    for(int y=0; y < SCREEN_HEIGHT; ++y)
        for(int x=0; x < SCREEN_WIDTH; ++x) {
            i.distance = FMAX;
            PutPixelSDL(screen, x, y,
                // calculate intersection
//                (ClosestIntersection(
//                    camera,
//                    normalize(R*vec3(x-SCREEN_HEIGHT/2, y-SCREEN_HEIGHT/2, focal)),
//                    triangles, i)
                (tree->intersect(
                    &ClosestIntersection,
                    camera,
                    normalize(R*vec3(x-SCREEN_HEIGHT/2, y-SCREEN_HEIGHT/2, focal)),
                    i, -1)
                 // use result to determine what color to draw
                 )? DirectLight(i)+light_dir*triangles[i.index].color: black);
        }

    if(SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    delete tree;
    tree = nullptr;
}

bool ClosestIntersection(const glm::vec3 &start, const glm::vec3 &d,
                         const std::vector<Triangle> &triangles,
                         Intersection &intersection, int exclude) {
    bool found = false;
    float t, u, v, f, distance;
    intersection.distance = std::numeric_limits<float>::max();
    for (vector<Triangle>::const_iterator it = triangles.begin();
            it != triangles.end(); ++it) {
        /* don't count intersections with this particular triangle */
        if (exclude >= 0 && (triangles.begin()+exclude) == it) continue;
        vec3 e1(it->v1 - it->v0);
        vec3 e2(it->v2 - it->v0);
        /* make sure d isn't parallel to the triangle plane */
        if (fabs(DOT(d, cross(e1, e2))) < EPSILON) continue;
        vec3 s(start - it->v0); // s = start - v0
        vec3 c = cross(d, e2);  // c = d x e2
        f = 1.0f/DOT(c, e1);    // f = 1/det(-d,e1,e2) = e1(d x e2)
        u = f * DOT(c, s);      // u = f*det(-d,s,e2) = s(d x e2)
        /* make sure u is in [0,1] */
        if (u < 0 || u > 1) continue;
        c = cross(s, e1);       // c = s x e1
        v = f * DOT(c, d);      // v = f*det(-d,e1,s) = d(s x e1)
        /* make sure v is in [0,1] (and v+u <= 1) */
        if (v < 0 || v+u > 1) continue;
        t = f * DOT(c, e2);     // t = f*det(s,e1,e2) = e2(s x e1)
        /* make sure the intersection is in front of the camera */
        if (t <= 0) continue;
        distance = t*d.length();
        found = true;
        /* keep the closest intersection */
        if (distance < intersection.distance) {
            intersection.index = it - triangles.begin();
            intersection.distance = distance;
            intersection.position = start+t*d;
        }
    }
    return found;
}

vec3 DirectLight(const intersection& i) {
    vec3 norm(triangles[i.index].normal);
    vec3 light(light_pos - i.position);
    float dist = DOT(light,light);
    /* look for objects that would block the light */
    Intersection intersection;
    intersection.distance = FMAX;
    light = normalize(light);
    tree->intersect(&ClosestIntersection, i.position, light, intersection, i.index);
    /* if the light was blocked, return black */
    if (intersection.distance < dist) return black;
    /* otherwise, scale the triangle color by the light color */
    return triangles[i.index].color * light_col * float(MAX(DOT(norm,light),0.f)) / float(4*M_PI*dist) ;
}
