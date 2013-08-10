
#include <math.h>
#include <stdio.h>

// windows.h must be included before GL headers
#ifdef _MSC_VER
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <vector>

#include "constants.h"
#include "rendering.h"
#include "bmp_loader.h"
#include "planet.h"

static GLfloat xrot = 25.0f, yrot = 30.0f, zrot = 0.0f;
static GLfloat zoom = 5.0f;
static GLfloat xpos = 0.0f, ypos = 0.0f, zpos = 0.0f;

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(xpos, ypos, -10.0f + zpos); // -10.0f brings origin into glFrustum's FOV
    GLfloat sun_p[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sun_p);

    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glRotatef(zrot, 0.0f, 0.0f, 1.0f);

    glScalef(zoom, zoom, zoom);

    //drawAxes();
    drawSky();

    drawSun();
    drawPlanets();
    drawStats();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLint) w, (GLint) h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (w > h)
        glFrustum(-1.0, 1.0, - (double) h/w, (double) h/w, 1.0, 40.0);
    else
        glFrustum(- (double) w/h, (double) w/h, -1.0, 1.0, 1.0, 40.0);

    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    (void) value;

    physicsStep();

    glutPostRedisplay();
    glutTimerFunc(1000 / FPS, timer, 0);
}

void special(int key, int x, int y) {
    (void) x;
    (void) y;

    switch (key) {
        case GLUT_KEY_LEFT:
            yrot += 0.5f;
            break;
        case GLUT_KEY_RIGHT:
            yrot -= 0.5f;
            break;
        case GLUT_KEY_UP:
            xrot += 0.5f;
            break;
        case GLUT_KEY_DOWN:
            xrot -= 0.5f;
            break;
        case GLUT_KEY_PAGE_UP:
            zrot -= 0.5f;
            break;
        case GLUT_KEY_PAGE_DOWN:
            zrot += 0.5f;
            break;
    }
}

void keyboard(unsigned char key, int x, int y) {
    (void) x;
    (void) y;

    switch (key) {
        case '+':
        case '=':
            zoom += 0.05f;
            break;
        case '-':
            zoom -= 0.05f;
            break;
        case 'Q':
        case 'q':
            exit(0);
            break;
        case 'W':
        case 'w':
            zpos += 0.05f;
            break;
        case 'S':
        case 's':
            zpos -= 0.05f;
            break;
        case 'A':
        case 'a':
            xpos += 0.05f;
            break;
        case 'D':
        case 'd':
            xpos -= 0.05f;
            break;
        case 'Z':
        case 'z':
            ypos += 0.05f;
            break;
        case 'X':
        case 'x':
            ypos -= 0.05f;
            break;
        case 'R':
        case 'r':
            xrot = 25.0f, yrot = 30.0f, zrot = 0.0f;
            zoom = 5.0f;
            xpos = 0.0f, ypos = 0.0f, zpos = 0.0f;
            break;
    }
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Earth orbits Sun");

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);

    GLfloat sun_d[] = {7.0f, 7.0f, 7.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_d);
    glEnable(GL_LIGHT0);

    glClearDepth(1.0);

    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);

    glutTimerFunc(1000 / FPS, timer, 0);

    initPlanets();

    starsTexture = loadBMPTexture("textures/starmap.bmp");
    sunTexture = loadBMPTexture("textures/sun.bmp");

    glutMainLoop();

    glDeleteTextures(1, &starsTexture);
    glDeleteTextures(1, &sunTexture);

    return 0;
}
