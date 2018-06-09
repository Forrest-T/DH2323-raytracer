#include <glm/glm.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <SDL.h>

#include "SDLauxiliary.h"
#include "rainbow.hpp"

using std::vector;
using glm::vec3;

int main() {
    screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
    // initialize interpolation vectors for the left and right columns
    interpolate(topLeft, botLeft, leftSide);
    interpolate(topRight, botRight, rightSide);
    draw();
    while (NoQuitMessageSDL()) usleep(100000);
    SDL_SaveBMP(screen, "screenshot.bmp");
    return 0;
}

void draw() {
    if(SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        // create an interpolation vector for x with the given y
        interpolate(leftSide[y], rightSide[y], color);
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            PutPixelSDL(screen, x, y, color[x]);
        }
    }
    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

template <class T>
void interpolate(T a, T b, vector<T> &result) {
    float frac = 0;
    size_t size = result.size();
    // bit magic: if size is 1, denominator becomes 2
    size_t den = (size-1)|((!(size>>1))<<1);
    for (size_t i = 0; i < size; i++) {
        // bit magic: if size is 1, numerator becomes 1
        frac = static_cast<float>(i|(!(size>>1))) / den;
        // basic linear interpolation over the interval [0,1]
        result[i] = (1-frac)*a + frac*b;
    }
}
