
// windows.h must be included before GL headers
#ifdef _MSC_VER
#include <windows.h>
#define snprintf _snprintf
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>

#include "constants.h"
#include "planet.h"

static GLfloat sunPhase = 0.0f;
static GLfloat days = 0.0f;

static std::vector<Planet> planets;

GLuint starsTexture = 0, sunTexture = 0;

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
    gluSphere(stars, 5.0f, 50, 50);

    gluDeleteQuadric(stars);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawText(const char *text, GLuint x, GLuint y, GLfloat r, GLfloat g, GLfloat b, GLubyte cont) {
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

    glColor3f(r, g, b);

    if (!cont)
        glRasterPos2i(x, viewport[3] - y); // Invert y axis to usual direction

    while (*text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);

    // Restore original matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

const char *elapsedDaysText = "Days elapsed: ",
           *elapsedMonthsText = "Siderial months elapsed: ";

void drawStats() {
    drawText(elapsedDaysText, 10, 20, 1.0f, 1.0f, 1.0f, 0);

    int days_len = snprintf(NULL, 0, "%.2f", days) + 1;
    char *days_str = (char*) malloc(days_len);
    snprintf(days_str, days_len, "%.2f", days);
    drawText(days_str, 0, 0, 1.0f, 1.0f, 1.0f, 1);
    free(days_str);

    drawText(elapsedMonthsText, 10, 40, 1.0f, 1.0f, 1.0f, 0);

    GLuint months = floorf(days / SIDERIAL_MONTH);

    int months_len = snprintf(NULL, 0, "%u", months) + 1;
    char *months_str = (char*) malloc(months_len);
    snprintf(months_str, months_len, "%u", months);
    drawText(months_str, 0, 0, 1.0f, 1.0f, 1.0f, 1);
    free(months_str);
}

void initPlanets() {
    planets.push_back(Planet(EARTH_RADIUS, 1.0f, 0.016f, SIDERIAL_YEAR, 1.0f, ECLIPTIC_INCLINATION, EARTH_AXIS_INCLINATION, "textures/earth.bmp", -M_PI));
    planets.back().addMoon(Planet(MOON_RADIUS, MOON_ORBIT_RADIUS, 0.05f, SIDERIAL_MONTH, SIDERIAL_MONTH, MOON_INCLINATION, 0.0f, "textures/moon.bmp", 0.0f));

    //                       Radius               A      Ecc     Year    Day      Incl   Tilt    Texture                 Phase
    planets.push_back(Planet(EARTH_RADIUS * 0.38, 0.39f, 0.2f,  87.9f,  58.6f,   3.38f, 0.03f,  "textures/mercury.bmp", -M_PI)); // Mercury
    planets.push_back(Planet(EARTH_RADIUS * 0.93, 0.7f,  0.006f,224.7f, -243.0f, 3.7f,  177.3f, "textures/venus.bmp",   -M_PI)); // Venus
    planets.push_back(Planet(EARTH_RADIUS * 0.53, 1.52f, 0.09f, 686.9f, 1.02f,   5.65f, 25.19f, "textures/mars.bmp",    -M_PI)); // Mars
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
