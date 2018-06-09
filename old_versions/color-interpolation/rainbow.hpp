#ifndef _RAINBOW_
#define _RAINBOW_

#include <glm/glm.hpp>
#include <vector>
#include <SDL.h>

/* Display-related constants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Surface* screen;

/* Color Interpolation vectors */
static std::vector<glm::vec3> leftSide(SCREEN_HEIGHT);
static std::vector<glm::vec3> rightSide(SCREEN_WIDTH);
static std::vector<glm::vec3> color(SCREEN_WIDTH);

/* Color interpolation constants */
static glm::vec3 topLeft(1,0,0);
static glm::vec3 topRight(0,0,1);
static glm::vec3 botLeft(0,1,0);
static glm::vec3 botRight(1,1,0);

/* Draws every pixel visible on the screen */
void draw();

/* Fills result with evenly spaced values, a <= value <= b
 *  - T must implement operator*(float, const T&)
 *  - result.size() controls the number of values interpolated
 *      - case 0: result = { }
 *      - case 1: result = { a + (b-a)/2 }
 *      - case n: result = { a, ... , b }
 */
template <class T>
void interpolate(T a, T b, std::vector<T> &result);

#endif
