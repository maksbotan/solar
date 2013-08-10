
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
void drawText(const char *text, GLuint x, GLuint y, GLfloat r, GLfloat g, GLfloat b, GLubyte cont);
void drawStats();
void initPlanets();
void drawPlanets();
void physicsStep();

#endif
