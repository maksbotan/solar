
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

Planet::Planet(GLfloat radius_,
               GLfloat semimajor_axis_,
               GLfloat eccentricity,
               GLfloat siderial_year_,
               GLfloat siderial_day_,
               GLfloat orbit_inclination_,
               GLfloat axis_inclination_,
               GLfloat asc_node_,
               GLfloat arg_periapsis_,
               const char *texture_file,
               GLfloat phi) :
    radius(radius_),
    semimajor_axis(semimajor_axis_),
    siderial_year(siderial_year_),
    siderial_day(siderial_day_),
    orbit_inclination(orbit_inclination_),
    axis_inclination(axis_inclination_),
    asc_node(asc_node_),
    arg_periapsis(arg_periapsis_),
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
    asc_node(rvalue.asc_node),
    arg_periapsis(rvalue.arg_periapsis),
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

void Planet::render(bool orbit, bool is_moon) {
    glPushMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);

    if (!is_moon) // When rendering moons we are already in planet's orbital plane
        glRotatef(ECLIPTIC_INCLINATION, 0.0f, 0.0f, 1.0f); // Enter ecliptic plane - common plane of reference
    glRotatef(asc_node, 0.0f, 1.0f, 0.0f); // Place orbital nodes
    glRotatef(orbit_inclination, 0.0f, 0.0f, 1.0f); // Now handle inclination
    glRotatef(arg_periapsis, 0.0f, 1.0f, 0.0f); // And finally argument of periapsis

    if (orbit) {
        glPushMatrix();
        glRotatef(90.f, 1.0f, 0.0f, 0.0f);
        glScalef(1.0f, semiminor_axis / semimajor_axis, 1.0f);
        drawTorus(semimajor_axis, 5, 300);
        glPopMatrix();
    }

    /*
    glBegin(GL_LINES);
    glVertex3f(-semimajor_axis, 0.0f, 0.0f);
    glVertex3f(semimajor_axis, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -semiminor_axis);
    glVertex3f(0.0f, 0.0f, semiminor_axis);
    glEnd();
    */

    glTranslatef(orbitX, 0.0f, orbitZ);

    for (auto it = moons.begin(); it != moons.end(); it++)
        it->render(orbit, true);

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

void Planet::generateLookAt(GLfloat &xpos, GLfloat &ypos, GLfloat &zpos, GLfloat &sight_x, GLfloat &sight_y, GLfloat &sight_z, GLfloat &up_x, GLfloat &up_y, GLfloat &up_z) {
    glPushMatrix();

    glLoadIdentity(); // Reset all current transformations

    // First of all, enter correct coordinate system for planet
    glRotatef(ECLIPTIC_INCLINATION, 0.0f, 0.0f, 1.0f);
    glRotatef(asc_node, 0.0f, 1.0f, 0.0f);
    glRotatef(orbit_inclination, 0.0f, 0.0f, 1.0f);
    glRotatef(arg_periapsis, 0.0f, 1.0f, 0.0f);

    // Get OpenGL's model-view matrix
    GLfloat mat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mat);

    glPopMatrix();

    GLfloat norm = sqrt(orbitX * orbitX + orbitZ * orbitZ);
    GLfloat model_x = orbitX * (1 + 8*radius/norm), model_y = radius, model_z = orbitZ * (1 + 8*radius/norm);

    // And multiply it to (orbitX, 0, orbitZ) in order to get world coordinates for the center of the planet
    GLfloat x = mat[0]*model_x + mat[4]*model_y + mat[8]*model_z;
    GLfloat y = mat[1]*model_x + mat[5]*model_y + mat[9]*model_z;
    GLfloat z = mat[2]*model_x + mat[6]*model_y + mat[10]*model_z;

    // Calculate up vector the same way and normalize it
    GLfloat norm_up = sqrt(mat[4]*mat[4] + mat[5]*mat[5] + mat[6]*mat[6]);
    up_x = mat[4] / norm_up;
    up_y = mat[5] / norm_up;
    up_z = mat[6] / norm_up;

    xpos = x;
    ypos = y;
    zpos = z;

    // Normalize the sight vector too
    GLfloat norm_sight = sqrt(x*x + y*y + z*z);

    sight_x = -x / norm_sight;
    sight_y = -y / norm_sight;
    sight_z = -z / norm_sight;
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

                x = (r + .0001*cos(s*twopi/numc))*cos(t*twopi/numt);
                y = (r + .0001*cos(s*twopi/numc))*sin(t*twopi/numt);
                z = .0001 * sin(s * twopi / numc);
                glVertex3f(x, y, z);
            }
        }
    glEnd();
    }
}
