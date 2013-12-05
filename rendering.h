
#ifndef RENDERING_H
#define RENDERING_H

#include <GL/gl.h>
#include <SDL_ttf.h>

#include "planet.h"

extern GLuint starsTexture;
extern GLuint sunTexture;

extern std::vector<Planet> planets;

void drawAxes();
void drawEcliptic();
void drawSun();
void drawEarth();
void drawMoon();
void drawSky();
void drawStats(const TTF_Font *font, Uint32 frames, bool help = false);
void initPlanets();
void drawPlanets(bool orbits = false);
void physicsStep(int elapsed);
void freeTextures();

void drawText(const char *text, const TTF_Font *font, GLuint x, GLuint y, bool opengl_coordinates = false, bool center_coordinates = false);

#endif
