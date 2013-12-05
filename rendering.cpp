
// windows.h must be included before GL headers
#ifdef _MSC_VER
#include <windows.h>
#define snprintf _snprintf
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>

#ifndef GL_BGRA
#include <GL/glext.h>
#endif

#include "constants.h"
#include "planet.h"
#include "rendering.h"
#include "bmp_loader.h"

static GLfloat sunPhase = 0.0f;
static GLfloat days = 0.0f;
static std::vector<GLuint> button_textures;

GLuint starsTexture = 0, sunTexture = 0;
std::vector<Planet> planets;

void drawAxes() {
    glLineWidth(3.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(9.0f, 0.0f, 0.0f);
    glVertex3f(-9.0f, 0.0f, 0.0f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 9.0f, 0.0f);
    glVertex3f(0.0f, -9.0f, 0.0f);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 9.0f);
    glVertex3f(0.0f, 0.0f, -9.0f);
    glEnd();
}

void drawSun() {
    glPushMatrix();

    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glRotatef(sunPhase, 0.0f, 1.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, sunTexture);

    GLfloat emission[] = {3.0f, 3.0f, 0.0f, 0.7f};
    GLfloat zero_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);

    GLUquadricObj *sun = gluNewQuadric();
    gluQuadricDrawStyle(sun, GLU_FILL);
    gluQuadricTexture(sun, GLU_TRUE);
    gluQuadricNormals(sun, GLU_SMOOTH);

    gluSphere(sun, SUN_RADIUS, 50, 50);

    gluDeleteQuadric(sun);
    glBindTexture(GL_TEXTURE_2D, 0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zero_emission);

    glPopMatrix();
}

void drawSky() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, starsTexture);

    GLUquadricObj *stars = gluNewQuadric();
    gluQuadricDrawStyle(stars, GLU_FILL);
    gluQuadricTexture(stars, GLU_TRUE);
    gluQuadricNormals(stars, GLU_SMOOTH);
    gluQuadricOrientation(stars, GLU_INSIDE);
    gluSphere(stars, ASTRONOMIC_UNIT * 10.0f, 50, 50);

    gluDeleteQuadric(stars);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawText(const char *text, const TTF_Font *font, GLuint x, GLuint y, bool opengl_coordinates, bool center_coordinates) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport); // [x, y, w, h]
    y = opengl_coordinates ? y : (viewport[3] - y);

    SDL_Color white = {255, 255, 255, 0};
    SDL_Surface *text_surface = TTF_RenderText_Blended(const_cast<TTF_Font*>(font), text, white);
    GLuint texture = 0;

    if (center_coordinates) {
        x -= text_surface->w / 2;
        y -= text_surface->h / 2;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, text_surface->w, text_surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, text_surface->pixels);

    glBegin(GL_QUADS);
    glTexCoord2d(0, 1); glVertex2i(x, y);
    glTexCoord2d(1, 1); glVertex2i(x + text_surface->w, y);
    glTexCoord2d(1, 0); glVertex2i(x + text_surface->w, y + text_surface->h);
    glTexCoord2d(0, 0); glVertex2i(x, y + text_surface->h);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture);
    SDL_FreeSurface(text_surface);

}


void drawButton(GLuint texture, GLuint y) {
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex2i(10, y);
    glTexCoord2d(1, 0); glVertex2i(10 + 50, y);
    glTexCoord2d(1, 1); glVertex2i(10 + 50, y + 50);
    glTexCoord2d(0, 1); glVertex2i(10, y + 50);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}

const char *elapsedDaysText = "Days elapsed: %.2f",
           *elapsedMonthsText = "Siderial months elapsed: %u",
           *fpsText = "FPS: %u",
           *aboutText[] = {
               "Simple Solar System model",
               "Controls:",
               " - w, a, s, d, z, x: camera movement",
               " - left, right, up, down, PgUp, PgDown: camera rotation",
               " - t: toggle speed acceleration by factor of 100",
               " - r: reset camera to initial position",
               " - o: orbits toggle",
               " - f: toggle fullscreen",
               " - v: toggle VSync",
               " - h: this help",
               " - q: quit program",
               ""};

void drawStats(const TTF_Font *font, Uint32 frames, bool help) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport); // [x, y, w, h]

    // Temporaly set 2D projection and disable lights
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, viewport[2], 0.0, viewport[3]);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);


    int days_len = snprintf(NULL, 0, elapsedDaysText, days) + 1;
    char *days_str = (char*) malloc(days_len);
    snprintf(days_str, days_len, elapsedDaysText, days);
    drawText(days_str, font, 10, 20);
    free(days_str);

    GLuint months = floorf(days / SIDERIAL_MONTH);

    int months_len = snprintf(NULL, 0, elapsedMonthsText, months) + 1;
    char *months_str = (char*) malloc(months_len);
    snprintf(months_str, months_len, elapsedMonthsText, months);
    drawText(months_str, font, 10, 45);
    free(months_str);

    int fps_len = snprintf(NULL, 0, fpsText, frames) + 1;
    char *fps_str = (char*) malloc(fps_len);
    snprintf(fps_str, months_len, fpsText, frames);
    drawText(fps_str, font, 10, 70);
    free(fps_str);

    Sint32 y1 = 79 + 50;

    size_t i = 0;
    for (auto it = planets.begin(); it != planets.end(); it++) {
        it->showTitle(font);
        drawButton(button_textures[i++], viewport[3] - y1);
        y1 += 59;
    }

    if (help) {
        // 266 and 362 are calculated for this particular font and text
        const char **aboutString = aboutText;
        y1 = (viewport[3] - 266) / 2;
        while (**aboutString)
            drawText(*(aboutString++), font, (viewport[2] - 362) / 2, y1 += 25);
    }

    // Restore original matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void initPlanets() {
    //                       Radius               A                        Ecc    Year    Day      Incl   Tilt    Node    Perih.  Texture                 Phase  Name
    planets.push_back(Planet(EARTH_RADIUS * 0.38, ASTRONOMIC_UNIT * 0.39f, 0.2f,  87.9f,  58.6f,   7.0f,  0.03f,  48.33f, 29.12f, "textures/mercury.bmp", -M_PI, "Mercury"));
    planets.push_back(Planet(EARTH_RADIUS * 0.93, ASTRONOMIC_UNIT * 0.7f,  0.006f,224.7f, -243.0f, 3.39f, 177.3f, 76.67f, 55.18f, "textures/venus.bmp",   -M_PI, "Venus"));

    planets.push_back(Planet(EARTH_RADIUS, ASTRONOMIC_UNIT, 0.016f, SIDERIAL_YEAR, 1.0f, 0.0f, EARTH_AXIS_INCLINATION, 0.0f, 0.0f, "textures/earth.bmp", -M_PI, "Earth"));
    planets.back().addMoon(Planet(MOON_RADIUS, MOON_ORBIT_RADIUS, 0.05f, SIDERIAL_MONTH, SIDERIAL_MONTH, MOON_INCLINATION, 6.68f, 0.0f, 0.0f, "textures/moon.bmp", 0.0f, "Moon"));

    planets.push_back(Planet(EARTH_RADIUS * 0.53, ASTRONOMIC_UNIT * 1.52f, 0.09f, 686.9f, 1.02f,   1.85f, 25.19f, 49.5f,  286.5,  "textures/mars.bmp",    -M_PI, "Mars"));

    planets.push_back(Planet(EARTH_RADIUS * 11.0f,ASTRONOMIC_UNIT * 5.2f,  0.04f, 4332.5f,0.41f,   1.3f,  3.13f,  100.4f, 275.0f, "textures/jupiter.bmp", -M_PI, "Jupiter"));
    planets.back().addMoon(Planet(EARTH_RADIUS * 0.28f, ASTRONOMIC_UNIT * 0.002f, 0.004f, 1.8f, 1.8f, 0.05f, 0.0f, 0.0f, 0.0f, "textures/io.bmp", 0.0f, "Io"));
    planets.back().addMoon(Planet(EARTH_RADIUS * 0.245f, ASTRONOMIC_UNIT * 0.0044f, 0.009f, 3.55f, 3.55f, 0.47f, 0.1f, 0.0f, 0.0f, "textures/europa.bmp", 0.0f, "Europa"));
    planets.back().addMoon(Planet(EARTH_RADIUS * 0.413f, ASTRONOMIC_UNIT * 0.00715, 0.0013f, 7.15f, 7.15f, 0.2f, 0.3f, 0.0f, 0.0f, "textures/ganymede.bmp", 0.0f, "Ganymede"));
    planets.back().addMoon(Planet(EARTH_RADIUS * 0.378f, ASTRONOMIC_UNIT * 0.01259f, 0.007f, 16.69f, 16.69f, 0.192f, 0.0f, 0.0f, 0.0f, "textures/callisto.bmp", 0.0f, "Callisto"));

    button_textures.push_back(loadBMPTexture("textures/buttons/mercury.bmp"));
    button_textures.push_back(loadBMPTexture("textures/buttons/venus.bmp"));
    button_textures.push_back(loadBMPTexture("textures/buttons/earth.bmp"));
    button_textures.push_back(loadBMPTexture("textures/buttons/mars.bmp"));
    button_textures.push_back(loadBMPTexture("textures/buttons/jupiter.bmp"));
}

void freeTextures() {
    for (auto it = button_textures.begin(); it != button_textures.end(); it++)
        glDeleteTextures(1, &(*it));
}

void drawPlanets(bool orbits) {
    for (auto it = planets.begin(); it != planets.end(); it++)
        it->render(orbits);
}

void physicsStep(int elapsed) {
    for (auto it = planets.begin(); it != planets.end(); it++)
        it->physicsStep(elapsed);

    days += DAYS_PER_SECOND / FPS;

    // Sun rotation
    sunPhase += 360 * DAYS_PER_SECOND * elapsed / (1000 * SUN_SIDERIAL_PERIOD);
    if (sunPhase > 360)
        sunPhase -= 360;
}
