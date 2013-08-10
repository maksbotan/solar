
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
    GLfloat radius, orbit_radius;
    GLfloat siderial_year, siderial_day;
    GLfloat orbit_inclination, axis_inclination;
    GLfloat orbitX, orbitZ, orbitPHI, phase; // phi in radians, phase in degrees
    GLuint texture;
    std::vector<Planet> moons;
public:
    Planet(GLfloat radius_, GLfloat orbit_radius_, GLfloat siderial_year_, GLfloat siderial_day_, GLfloat orbit_inclination_, GLfloat axis_inclination_, const char *texture_file, GLfloat phi = 0.0f);
    Planet(Planet &&rvalue);
    ~Planet();
    void physicsStep(int elapsed);
    void render();
    void addMoon(Planet &&moon); // Use rvalue reference to always steal caller's object - avoids copying OpenGL textures
};

#endif
