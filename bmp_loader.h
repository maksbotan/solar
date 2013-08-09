
#ifndef BMP_LOADER_H
#define BMP_LOADER_H

// windows.h must be included before GL headers
#ifdef _MSC_VER
#include <windows.h>
#endif

#include <GL/gl.h>

GLuint loadBMPTexture(const char *filename);

#endif
