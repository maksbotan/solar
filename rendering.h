
#ifndef RENDERING_H
#define RENDERING_H

#include <GL/gl.h>

extern GLuint starsTexture;
extern GLuint sunTexture;

void drawAxes();
void drawEcliptic();
void drawSun();
void drawEarth();
void drawMoon();
void drawSky();
void drawStats(const TTF_Font *font, Uint32 frames);
void initPlanets();
void drawPlanets(bool orbits = false);
void physicsStep(int elapsed);

#endif
