
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

#include "constants.h"

static GLfloat earthX = -1.0f, earthZ = 0.0f;
static GLfloat earthPHI = -M_PI, earthPhase = 0.0f;
static GLfloat moonX = 0.2f, moonZ = 0.0f;
static GLfloat moonPHI = 0.0f;
static GLfloat sunPhase = 0.0f;
static GLfloat days = 0.0f;
static GLuint months = 0;

GLuint earthTexture = 0, moonTexture = 0, starsTexture = 0, sunTexture = 0;

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

void drawEcliptic() {
    glPushMatrix();

    glRotatef(ECLIPTIC_INCLINATION, 0.0f, 1.0f, 0.0f);

    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_QUADS);
        glVertex3f(-1.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(1.0f, 0.0f, -1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
    glEnd();

    glPopMatrix();
}

void drawEarth() {
    glPushMatrix();

    glTranslatef(earthX, 0.0f, earthZ);

    glRotatef(ECLIPTIC_INCLINATION, 0.0f, 1.0f, 0.0f); // Ecliptic is inclined wrt sun equator
    glRotatef(EARTH_AXIS_INCLINATION, 1.0f, 0.0f, 0.0f); // Earth axis is inclined wrt ecliptic
    glRotatef(earthPhase, 0.0f, 1.0f, 0.0f); // Finally handle everyday rotation

    // First draw axis
    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, -EARTH_AXIS_HALFSIZE, 0.0f);
    glVertex3f(0.0f, EARTH_AXIS_HALFSIZE, 0.0f);
    glEnd();

    glRotatef(90.0f, -1.0f, 0.0f, 0.0f); // Rotate a bit so texture is applied correctly
    glBindTexture(GL_TEXTURE_2D, earthTexture);

    GLUquadricObj *earth = gluNewQuadric();
    gluQuadricDrawStyle(earth, GLU_FILL);
    gluQuadricTexture(earth, GLU_TRUE);
    gluQuadricNormals(earth, GLU_SMOOTH);

    gluSphere(earth, EARTH_RADIUS, 50, 50);

    gluDeleteQuadric(earth);
    glBindTexture(GL_TEXTURE_2D, 0);

    glPopMatrix();
}

void drawMoon() {
    glPushMatrix();

    // Bring origin to Earth center
    glTranslatef(earthX, 0.0f, earthZ);
    glRotatef(ECLIPTIC_INCLINATION, 0.0f, 1.0f, 0.0f);
    glRotatef(MOON_INCLINATION, 0.0f, 1.0f, 0.0f); // Now we are in plane of Moon's orbit

    glTranslatef(moonX, 0.0f, moonZ); // Bring us to actual moon position on orbit
    glRotatef(90.0f, -1.0f, 0.0f, 0.0f); // Correct for texturing again
    glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, moonTexture);

    GLUquadricObj *moon = gluNewQuadric();
    gluQuadricDrawStyle(moon, GLU_FILL);
    gluQuadricTexture(moon, GLU_TRUE);
    gluQuadricNormals(moon, GLU_SMOOTH);

    gluSphere(moon, MOON_RADIUS, 50, 50);

    gluDeleteQuadric(moon);
    glBindTexture(GL_TEXTURE_2D, 0);

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

    int months_len = snprintf(NULL, 0, "%u", months) + 1;
    char *months_str = (char*) malloc(months_len);
    snprintf(months_str, months_len, "%u", months);
    drawText(months_str, 0, 0, 1.0f, 1.0f, 1.0f, 1);
    free(months_str);
}

void physicsStep() {
    // First recalculate Earth's position on orbit
    earthPHI += DAYS_PER_SECOND * 2*M_PI / (FPS * SIDERIAL_YEAR);
    if (earthPHI > 2*M_PI) // Keep it small
        earthPHI -= 2*M_PI;

    days += DAYS_PER_SECOND / FPS;

    earthX = cos(-earthPHI);
    earthZ = sin(-earthPHI);

    // Then handle its rotation: one full revolution each day
    earthPhase += 360 * (double) DAYS_PER_SECOND / FPS;
    if (earthPhase >= 360) // earthPhase is in degrees
        earthPhase -= 360;

    // Moon orbital movement
    moonPHI += DAYS_PER_SECOND * 2*M_PI / (FPS * SIDERIAL_MONTH);
    moonX = MOON_ORBIT_RADIUS * cosf(-moonPHI);
    moonZ = MOON_ORBIT_RADIUS * sinf(-moonPHI);
    if (moonPHI > 2*M_PI) {
        moonPHI -= 2*M_PI;
        months++;
    }

    // Sun rotation
    sunPhase += 360 * DAYS_PER_SECOND / (FPS * SUN_SIDERIAL_PERIOD);
    if (sunPhase > 360)
        sunPhase -= 360;
}
