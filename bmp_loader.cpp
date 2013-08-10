
#include <stdlib.h>
#include <stdio.h>

// windows.h must be included before GL headers
#ifdef _MSC_VER
#include <windows.h>
#endif

#include <GL/gl.h>
#ifndef GL_BGR
#include <GL/glext.h>
#endif

/*
 * This code is partially taken from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
 * Structs are taken from Wikipedia
 */

#pragma pack(push, 1)
typedef struct {
    GLushort type;
    GLuint size;
    GLushort reserved1;
    GLushort reserved2;
    GLuint offset;
} BMPFileHeader;

typedef struct {
    GLuint size;
    GLint width;
    GLint height;
    GLushort planes;
    GLushort bitCount;
    GLuint compression;
    GLuint sizeImage;
    GLint XPelsPerMeter;
    GLint YPelsPerMeter;
    GLuint clrUsed;
    GLuint clrImportant;
} BMPInfoHeader;
#pragma pack(pop)

GLuint loadBMPTexture(const char *filename) {
    FILE *bmp = fopen(filename, "rb");
    if (!bmp) {
        perror("Cannot open image file");
        return 0;
    }

    BMPFileHeader hdr;
    BMPInfoHeader info;

    fread(&hdr, 1, sizeof(hdr), bmp);
    fread(&info, 1, sizeof(info), bmp);

    if (info.width == 0 || info.height == 0 || hdr.offset == 0 || info.sizeImage == 0) {
        fprintf(stderr, "Malformed BMP file\n");
        fclose(bmp);
        return 0;
    }

    if (info.bitCount != 24 || info.compression != 0) { // 0 is BI_RGB, uncompressed image
        fprintf(stderr, "Only 24-bit uncompressed BMP's are supported\n");
        fclose(bmp);
        return 0;
    }

    GLubyte *data = (GLubyte*) malloc(info.sizeImage);
    fread(data, 1, info.sizeImage, bmp);
    fclose(bmp);

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info.width, info.height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    free(data);

    return texture;
}
