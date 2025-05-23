#pragma once

#include <SDL3/SDL_rect.h>

float square(float);
float sqrDist(SDL_FPoint, SDL_FPoint);
SDL_FRect center(SDL_FRect);
bool pointOnLine(SDL_FPoint p, SDL_FPoint lineStart, SDL_FPoint lineEnd, float lineThickness);
bool pointInFRect(SDL_FPoint, SDL_FRect);
bool pointInCircle(SDL_FPoint p, float r, SDL_FPoint cp);
bool pointWithin(SDL_FPoint b, SDL_FPoint a, float dist);
bool rectsOverlap(SDL_FRect, SDL_FRect);
bool compare(SDL_FPoint, SDL_FPoint);
SDL_FPoint rectToPoint(SDL_FRect);
