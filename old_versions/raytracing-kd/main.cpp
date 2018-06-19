#include <iostream>
#include "glm/glm.hpp"
#include "SDL.h"
#include "SDL_helper.hpp"
#include "Kdtree.hpp"
#include "model.hpp"
#include "main.hpp"

using namespace std;
using glm::vec3;
using glm::mat3;

int main() {
    screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
    t = SDL_GetTicks();
    LoadTestModel(triangles);
    
//    while(NoQuitMessageSDL()) {
//        Draw();
//        Update();
//    }
    Draw();
    Update();
    
    SDL_SaveBMP( screen, "screenshot.bmp" );
    return 0;
}

void Update() {
    // Compute frame time:
    int t2 = SDL_GetTicks();
    float dt = float(t2-t);
    t = t2;
    cout << "Render time: " << dt << " ms." << endl;
    Uint8* keystate = SDL_GetKeyState( 0 );
    if( keystate[SDLK_UP]) {
        // Move camera forward
        camerapos.z +=1;
        
    }
    if( keystate[SDLK_DOWN]) {
        // Move camera backward
        camerapos.z -=1;
    }
    if( keystate[SDLK_LEFT]) {
        cout<< "Left"<<endl;
        yaw += 0.5;
        vec3 ent1( cos(yaw) , 0 , sin(yaw) );
        vec3 ent2( 0 , 1 , 0 );
        vec3 ent3( -sin(yaw) , 0 , cos(yaw) );
        R = mat3(ent1,ent2,ent3);
    }
    if( keystate[SDLK_RIGHT]) {
        cout<< "Right"<<endl;
        yaw -= 0.5;
        vec3 ent1( cos(yaw) , 0 , sin(yaw) );
        vec3 ent2( 0 , 1 , 0 );
        vec3 ent3( -sin(yaw) , 0 , cos(yaw) );
        R = mat3(ent1,ent2,ent3);
    }
    if( keystate[SDLK_w] )
        lightPos += forwardx;
    if( keystate[SDLK_s] )
        lightPos -= forwardx;
    if( keystate[SDLK_d] )
        lightPos += rightx;
    if( keystate[SDLK_a] )
        lightPos -= rightx;
    if( keystate[SDLK_q] )
        lightPos += downx;
    if( keystate[SDLK_e] )
        lightPos -= downx;
}

void Draw() {
    vec3 light;
    Intersection intersection;
    tree = new KD_Tree(triangles);
    if( SDL_MUSTLOCK(screen) )
        SDL_LockSurface(screen);
    
    for( int y=0; y<SCREEN_HEIGHT; ++y ) {
        for( int x=0; x<SCREEN_WIDTH; ++x ) {
            vec3 dir ((x- SCREEN_WIDTH/2), (y- SCREEN_HEIGHT/2), SCREEN_WIDTH);
            dir =  glm::normalize(R* dir);
            intersection.distance = FMAX;
            
            if (intersectionTree(tree->root, camerapos, dir, intersection)){
                light = DirectLight(intersection)*triangles[intersection.index].color;
                light += indirectLight*triangles[intersection.index].color;
                PutPixelSDL( screen, x, y, light);
            } else {
                vec3 color (0,0,0);
                PutPixelSDL( screen, x, y, color );
            }
        }
    }
    
    delete tree;
    if( SDL_MUSTLOCK(screen) )
        SDL_UnlockSurface(screen);
    SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

//KD TREE NODE INTERSECTION DETECTION
bool intersectionTree(KD_Node* node, glm::vec3 start, glm::vec3 dir, Intersection& treeintersection) {
    Intersection i;
    if (!node->bounding_box.BoxIntersection(start, dir, i))
        return false;
    i.distance = FMAX;

    if (node->leaf) {
        if (ClosestIntersection(start, dir, node->triangles, i)) {
            if (i.distance < treeintersection.distance){
                treeintersection = i;
                return true;
            }
        }
        return false;
    }

    Intersection left = treeintersection, right = treeintersection;
    bool leftFlag = false, rightFlag = false;
    leftFlag = intersectionTree(node->leftchild, start, dir, left);
    rightFlag = intersectionTree(node->rightchild, start, dir, right);
    if (!leftFlag && !rightFlag)
        return false;
    else if (!leftFlag)
        treeintersection = right;
    else if (!rightFlag)
        treeintersection = left;
    else
        treeintersection = left.distance < right.distance ? left : right;
    return true;
}

vec3 DirectLight( const Intersection& i ){
    vec3 norm(triangles[i.index].normal);
    vec3 light(lightPos - i.position);
    float dist = dot(light,light);
    /* look for objects that would block the light */
    Intersection intersection;
    intersection.distance = FMAX;
    light = normalize(light);
    intersectionTree(tree->root, i.position, light, intersection);
    /* if the light was blocked, return black */
    if (intersection.distance < dist) return vec3(0,0,0);
    /* otherwise, scale the triangle color by the light color */
    return triangles[i.index].color * lightColor * float(std::max(dot(norm,light),0.f)) / float(4*M_PI*dist) ;
}
bool ClosestIntersection(vec3 start, vec3 d, const vector<Triangle>& triangles, Intersection& intersection){
    
    bool found = false;
    float t, u, v, f, distance;
    intersection.distance = std::numeric_limits<float>::max();
    for (vector<Triangle>::const_iterator it = triangles.begin();
            it != triangles.end(); ++it) {
        vec3 e1(it->v1 - it->v0);
        vec3 e2(it->v2 - it->v0);
        /* make sure d isn't parallel to the triangle plane */
        if (fabs(dot(d, cross(e1, e2))) < EPSILON) continue;
        vec3 s(start - it->v0); // s = start - v0
        vec3 c = cross(d, e2);  // c = d x e2
        f = 1.0f/dot(c, e1);    // f = 1/det(-d,e1,e2) = e1(d x e2)
        u = f * dot(c, s);      // u = f*det(-d,s,e2) = s(d x e2)
        /* make sure u is in [0,1] */
        if (u < 0 || u > 1) continue;
        c = cross(s, e1);       // c = s x e1
        v = f * dot(c, d);      // v = f*det(-d,e1,s) = d(s x e1)
        /* make sure v is in [0,1] (and v+u <= 1) */
        if (v < 0 || v+u > 1) continue;
        t = f * dot(c, e2);     // t = f*det(s,e1,e2) = e2(s x e1)
        /* make sure the intersection is in front of the camera */
        if (t <= 0) continue;
        distance = t*d.length();
        /* keep the closest intersection */
        if (distance < intersection.distance) {
            intersection.index = it - triangles.begin();
            intersection.distance = distance;
            intersection.position = start+t*d;
            found = true;
        }
    }
    return found;
}
