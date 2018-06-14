#include <iostream>
#include "glm/glm.hpp"
#include "SDL.h"
#include "SDLauxiliary.h"
#include "Kdtree.hpp"


using namespace std;
using glm::vec3;
using glm::mat3;


// ----------------------------------------------------------------------------
// GLOBAL VARIABLES
vec3 indirectLight = 0.5f*vec3( 1, 1, 1 );
vec3 lightPos( 0, -0.5, -0.7 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
mat3 R;
float yaw;
vec3 rightx( R[0][0], R[0][1], R[0][2] );
vec3 downx( R[1][0], R[1][1], R[1][2] );
vec3 forwardx( R[2][0], R[2][1], R[2][2] );
SDL_Surface* screen;
int t;
vec3 camerapos (0,0,-3);
vector<Triangle> triangles;

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
vec3 DirectLight( const Intersection& i );
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection);
bool intersectionTree(Kdtree* node, glm::vec3 start, glm::vec3 dir, Intersection& treeintersection, int x, int y);

int main( int argc, char* argv[] )
{
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
    t = SDL_GetTicks();    // Set start value for timer.
    LoadTestModel(triangles );
    
    while( NoQuitMessageSDL() )
    {
        Draw();
        Update();
        
    }
    
    SDL_SaveBMP( screen, "screenshot.bmp" );
    return 0;
}

void Update()
{
    
    
    // Compute frame time:
    int t2 = SDL_GetTicks();
    float dt = float(t2-t);
    t = t2;
    cout << "Render time: " << dt << " ms." << endl;
    Uint8* keystate = SDL_GetKeyState( 0 );
    if( keystate[SDLK_UP] )
    {
        // Move camera forward
        camerapos.z +=1;
        
    }
    if( keystate[SDLK_DOWN] )
    {
        // Move camera backward
        camerapos.z -=1;
    }
    if( keystate[SDLK_LEFT] )
    {
        cout<< "Left"<<endl;
        yaw += 0.5;
        vec3 ent1( cos(yaw) , 0 , sin(yaw) );
        vec3 ent2( 0 , 1 , 0 );
        vec3 ent3( -sin(yaw) , 0 , cos(yaw) );
        R = mat3(ent1,ent2,ent3);
    }
    if( keystate[SDLK_RIGHT] )
    {
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

void Draw()
{
    
    Intersection intersection;
    Kdtree *node = new Kdtree();
    node = node->buildTree(triangles);
    if( SDL_MUSTLOCK(screen) )
        SDL_LockSurface(screen);
    
    for( int y=0; y<SCREEN_HEIGHT; ++y )
    {
        for( int x=0; x<SCREEN_WIDTH; ++x )
        {
            vec3 dir ((x- SCREEN_WIDTH/2), (y- SCREEN_HEIGHT/2), SCREEN_WIDTH);
            dir =  R* dir;
            
            intersectionTree(node, camerapos, dir, intersection, x, y);
            /*
            vec3 dir ((x- SCREEN_WIDTH/2), (y- SCREEN_HEIGHT/2), SCREEN_WIDTH);
            dir =  R* dir;
            vec3 color( 1, 0.5, 0.5 );
            
            if (ClosestIntersection(camerapos, dir, triangles, intersection)){
                light = DirectLight(intersection);
                light = light + indirectLight;
                PutPixelSDL( screen, x, y, light*  triangles[intersection.index].color );
            }
            else {
                vec3 color (0,0,0);
                PutPixelSDL( screen, x, y, color );
            }
            */
        }
    }
    
    if( SDL_MUSTLOCK(screen) )
        SDL_UnlockSurface(screen);
    
    SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
//KD TREE NODE INTERSECTION DETECTION
bool intersectionTree(Kdtree* node, glm::vec3 start, glm::vec3 dir, Intersection& treeintersection, int x, int y){
    vec3 light;
    Intersection nodeIntersect;
    bool leftFlag = false;
    bool rightFlag = false;
    if (node->bounding_box.BoxIntersection(start, dir, treeintersection)){ //check if ray intersects the bounding box
        if ( node-> leftchild != nullptr || node->rightchild != nullptr){
        //if(node->leftchild->triangles.size()> 0 || node->rightchild->triangles.size()> 0){ //see if any children has primitives
            if ( node->leftchild !=nullptr){
                leftFlag = intersectionTree(node->leftchild, start, dir, treeintersection, x, y);
            }
            if ( node->rightchild !=nullptr){
                rightFlag = intersectionTree(node->rightchild, start, dir, treeintersection,x ,y);
            }
            return rightFlag || leftFlag;
        }
        else{
            // WHERE THE BUG IS
            
            if (ClosestIntersection(camerapos, dir, node->triangles, treeintersection)){
                light = DirectLight(treeintersection);
                light = light + indirectLight;
                //cout<<triangles[treeintersection.index].midpoint<<std::endl;
                PutPixelSDL( screen, x, y, light*  node->triangles[treeintersection.index].color );
            }
            else {
                vec3 color (0,0,0);
                PutPixelSDL( screen, x, y, color );
            }
        }
        
    }
    return false;
}
vec3 DirectLight( const Intersection& i ){
    float pi = 3.14159265358979323846264338327950288;
    //find r
    vec3 r = glm::normalize(lightPos - i.position);
    //find n
    vec3 n = triangles[i.index].normal;
    //do product mult
    float mult = glm::dot(r, n);
    //use equation
    float length = glm::length(lightPos - i.position);
    float lightout = (max(mult, 0.f))/(4.f*pi*length*length);
    
    //Shadowboxing
    Intersection shadow;
    vec3 shadowdir = glm::normalize(i.position - lightPos);
    bool intersect= ClosestIntersection(lightPos, shadowdir, triangles, shadow);
    if (intersect && (shadow.distance < length - 0.001f)){
        vec3 color (0,0,0);
        return color;
    }
    //return
    return lightColor*lightout;
}
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& xtriangles, Intersection& closestIntersection){
    
    float distance = std::numeric_limits<float>::max();
    int   index = -27;
    using glm::vec3;
    using glm::mat3;
    
    for (int i = 0; i < xtriangles.size(); i++){
        Triangle triangle = xtriangles[i];
        vec3 v0 = triangle.v0;
        vec3 v1 = triangle.v1;
        vec3 v2 = triangle.v2;
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        vec3 b = start - v0;
        mat3 A( -dir, e1, e2 );
        vec3 x = glm::inverse( A ) * b;
        
        if ((x.x >= 0) && (x.y >= 0) && (x.z >= 0 ) && (x.z + x.y <= 1)){
            if ( distance > x.x){
                distance = x.x;
                index = i;
            }
        }
    }
    
    if (index >= 0) {
        closestIntersection.distance = distance;
        closestIntersection.index = index;
        closestIntersection.position = start + (distance * dir);
        return true;
    }
    else{
        return false;
    }
    
    //    vec3 v0 = triangle.v0;
    //    vec3 v1 = triangle.v1;
    //    vec3 v2 = triangle.v2;
    //    vec3 e1 = v1 - v0;
    //    vec3 e2 = v2 - v0;
    //    vec3 b = start - v0;
    //    mat3 A( -dir, e1, e2 );
    //    vec3 x = glm::inverse( A ) * b;
    
}




