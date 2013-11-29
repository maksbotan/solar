
#ifndef PLANET_H
#define PLANET_H

// windows.h must be included before GL headers
#ifdef _MSC_VER
#include <windows.h>
#endif
#include <GL/gl.h>

#include <vector>

class Planet {
protected:
    GLfloat radius, semimajor_axis, semiminor_axis;
    GLfloat siderial_year, siderial_day;
    GLfloat orbit_inclination, axis_inclination;
    GLfloat asc_node, arg_periapsis;
    GLfloat orbitX, orbitZ, orbitPHI, phase; // phi in radians, phase in degrees
    GLuint texture;
    std::vector<Planet> moons;
public:
    Planet(GLfloat radius_,
           GLfloat semimajor_axis_,
           GLfloat eccentricity,
           GLfloat siderial_year_,
           GLfloat siderial_day_,
           GLfloat orbit_inclination_,
           GLfloat axis_inclination_,
           GLfloat asc_node_,
           GLfloat arg_periapsis_,
           const char *texture_file,
           GLfloat phi = 0.0f);
    Planet(Planet &&rvalue);
    ~Planet();
    void physicsStep(int elapsed);
    void render(bool orbit = false, bool is_moon = false);
    void addMoon(Planet &&moon); // Use rvalue reference to always steal caller's object - avoids copying OpenGL textures
    void generateLookAt(GLfloat &xpos, GLfloat &ypos, GLfloat &zpos, GLfloat &sight_x, GLfloat &sight_y, GLfloat &sight_z, GLfloat &up_x, GLfloat &up_y, GLfloat &up_z);
};

#endif
