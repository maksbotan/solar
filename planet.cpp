
// windows.h must be included before GL headers
#ifdef _MSC_VER
#include <windows.h>
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "constants.h"
#include "bmp_loader.h"
#include "planet.h"

void drawTorus(double, int, int);

Planet::Planet(GLfloat radius_, GLfloat semimajor_axis_, GLfloat eccentricity, GLfloat siderial_year_, GLfloat siderial_day_, GLfloat orbit_inclination_, GLfloat axis_inclination_, const char *texture_file, GLfloat phi) :
    radius(radius_),
    semimajor_axis(semimajor_axis_),
    siderial_year(siderial_year_),
    siderial_day(siderial_day_),
    orbit_inclination(orbit_inclination_),
    axis_inclination(axis_inclination_),
    orbitX(0.0f),
    orbitZ(0.0f),
    orbitPHI(phi),
    phase(0.0f)
{
    semiminor_axis = semimajor_axis * sqrtf(1.0f - eccentricity*eccentricity);
    texture = loadBMPTexture(texture_file);
}

Planet::Planet(Planet &&rvalue) :
    radius(rvalue.radius),
    semimajor_axis(rvalue.semimajor_axis),
    semiminor_axis(rvalue.semiminor_axis),
    siderial_year(rvalue.siderial_year),
    siderial_day(rvalue.siderial_day),
    orbit_inclination(rvalue.orbit_inclination),
    axis_inclination(rvalue.axis_inclination),
    orbitX(rvalue.orbitX),
    orbitZ(rvalue.orbitZ),
    orbitPHI(rvalue.orbitPHI),
    phase(rvalue.phase)
{
    texture = rvalue.texture;
    rvalue.texture = 0;
    for (auto it = rvalue.moons.begin(); it != rvalue.moons.end(); it++)
        moons.push_back(std::move(*it));
}

Planet::~Planet() {
    glDeleteTextures(1, &texture);
}

void Planet::physicsStep(int elapsed) {
    orbitPHI += DAYS_PER_SECOND * 2*M_PI * elapsed / (1000 * siderial_year);
    if (orbitPHI >= 2*M_PI)
        orbitPHI -= 2*M_PI; // Keep it small

    // '-' for counterclockwise orbiting
    orbitX = semimajor_axis * cos(-orbitPHI);
    orbitZ = semiminor_axis * sin(-orbitPHI);

    phase += 360 * DAYS_PER_SECOND * elapsed / (1000 * siderial_day);
    if (phase >= 360.0f)
        phase -= 360.0f;

    for (auto it = moons.begin(); it != moons.end(); it++)
        it->physicsStep(elapsed);
}

void Planet::render(bool orbit) {
    glPushMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);

    glRotatef(orbit_inclination, 0.0f, 0.0f, 1.0f);

    if (orbit) {
        glPushMatrix();
        glRotatef(90.f, 1.0f, 0.0f, 0.0f);
        glScalef(1.0f, semiminor_axis / semimajor_axis, 1.0f);
        drawTorus(semimajor_axis, 5, 100);
        glPopMatrix();
    }

    glTranslatef(orbitX, 0.0f, orbitZ);

    for (auto it = moons.begin(); it != moons.end(); it++)
        it->render(orbit);

    glRotatef(axis_inclination, 1.0f, 0.0f, 0.0f); // Axis is inclined wrt orbit
    glRotatef(phase, 0.0f, 1.0f, 0.0f); // Finally handle everyday rotation

    glRotatef(90.0f, -1.0f, 0.0f, 0.0f); // Rotate a bit so texture is applied correctly
    glBindTexture(GL_TEXTURE_2D, texture);

    GLUquadricObj *planet = gluNewQuadric();
    gluQuadricDrawStyle(planet, GLU_FILL);
    gluQuadricTexture(planet, GLU_TRUE);
    gluQuadricNormals(planet, GLU_SMOOTH);

    gluSphere(planet, radius, 50, 50);

    gluDeleteQuadric(planet);
    glBindTexture(GL_TEXTURE_2D, 0);


    glPopMatrix();
}

void Planet::addMoon(Planet &&moon) {
    moons.push_back(std::move(moon));
}

// Taken from google, claimed to be from Red Book
void drawTorus(double r, int numc, int numt) {
    int i, j, k;
    double s, t, x, y, z, twopi;

    twopi = 2 * (double) M_PI;
    for (i = 0; i < numc; i++) {
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= numt; j++) {
            for (k = 1; k >= 0; k--) {
                s = (i + k) % numc + 0.5;
                t = j % numt;

                x = (r + .001*cos(s*twopi/numc))*cos(t*twopi/numt);
                y = (r + .001*cos(s*twopi/numc))*sin(t*twopi/numt);
                z = .001 * sin(s * twopi / numc);
                glVertex3f(x, y, z);
            }
        }
    glEnd();
    }
}
