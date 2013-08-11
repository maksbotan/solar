
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

Planet::Planet(GLfloat radius_, GLfloat orbit_radius_, GLfloat siderial_year_, GLfloat siderial_day_, GLfloat orbit_inclination_, GLfloat axis_inclination_, const char *texture_file, GLfloat phi) :
    radius(radius_),
    orbit_radius(orbit_radius_),
    siderial_year(siderial_year_),
    siderial_day(siderial_day_),
    orbit_inclination(orbit_inclination_),
    axis_inclination(axis_inclination_),
    orbitX(0.0f),
    orbitZ(0.0f),
    orbitPHI(phi),
    phase(0.0f)
{
    texture = loadBMPTexture(texture_file);
}

Planet::Planet(Planet &&rvalue) :
    radius(rvalue.radius),
    orbit_radius(rvalue.orbit_radius),
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
    orbitPHI += DAYS_PER_SECOND * 2*M_PI / (FPS * siderial_year);
    if (orbitPHI >= 2*M_PI)
        orbitPHI -= 2*M_PI; // Keep it small

    // '-' for counterclockwise orbiting
    orbitX = orbit_radius * cos(-orbitPHI);
    orbitZ = orbit_radius * sin(-orbitPHI);

    phase += 360 * DAYS_PER_SECOND / (FPS * siderial_day);
    if (phase >= 360.0f)
        phase -= 360.0f;

    for (auto it = moons.begin(); it != moons.end(); it++)
        it->physicsStep(elapsed);
}

void Planet::render() {
    glPushMatrix();

    glTranslatef(orbitX, 0.0f, orbitZ);

    glRotatef(orbit_inclination, 0.0f, 1.0f, 0.0f); // Orbit is inclined wrt sun equator

    for (auto it = moons.begin(); it != moons.end(); it++)
        it->render();

    glRotatef(axis_inclination, 1.0f, 0.0f, 0.0f); // Axis is inclined wrt orbit
    glRotatef(phase, 0.0f, 1.0f, 0.0f); // Finally handle everyday rotation

    glColor3f(1.0f, 1.0f, 1.0f);
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