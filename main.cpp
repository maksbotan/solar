
// windows.h must be included before GL headers
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <windows.h>
#endif
#include <math.h>

#include <GL/gl.h>
#include <GL/glut.h>
#include <vector>

#include "constants.h"
#include "rendering.h"
#include "bmp_loader.h"
#include "planet.h"

static GLfloat xpos = 0.0f, ypos = 1.0f, zpos = 2.5f;
static GLfloat sight_x = 0.0f, sight_y = -0.43f, sight_z = -0.9f;
static GLfloat up_x = 0.0f, up_y = 1.0f;
static GLfloat cam_xz_angle = 0.0f, cam_y_angle = -0.45f;
static GLfloat cam_z_angle = M_PI_2;
static int windowW = 800, windowH = 600;
static bool fullscreen = false, orbits = false;

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(xpos,           ypos,           zpos,
              xpos + sight_x, ypos + sight_y, zpos + sight_z,
              up_x,           up_y,           0.0f);

    GLfloat sun_p[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sun_p);

    //drawAxes();
    drawSky();

    drawSun();
    drawPlanets(orbits);
    drawStats();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (!h)
        h = 1;

    glViewport(0, 0, (GLint) w, (GLint) h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (float) w/h, 0.1f, 50.0f);

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
            cam_xz_angle -= 0.02f;
            sight_x = cosf(cam_y_angle) * sinf(cam_xz_angle);
            sight_z = -cosf(cam_y_angle) * cosf(cam_xz_angle);
            break;
        case GLUT_KEY_RIGHT:
            cam_xz_angle += 0.02f;
            sight_x = cosf(cam_y_angle) * sinf(cam_xz_angle);
            sight_z = -cosf(cam_y_angle) * cosf(cam_xz_angle);
            break;
        case GLUT_KEY_UP:
            cam_y_angle += 0.02;
            sight_x = cosf(cam_y_angle) * sinf(cam_xz_angle);
            sight_y = sinf(cam_y_angle);
            sight_z = -cosf(cam_y_angle) * cosf(cam_xz_angle);
            break;
        case GLUT_KEY_DOWN:
            cam_y_angle -= 0.02f;
            sight_x = cosf(cam_y_angle) * sinf(cam_xz_angle);
            sight_y = sinf(cam_y_angle);
            sight_z = -cosf(cam_y_angle) * cosf(cam_xz_angle);
            break;
        case GLUT_KEY_PAGE_UP:
            cam_z_angle += 0.02f;
            up_x = cosf(cam_z_angle);
            up_y = sinf(cam_z_angle);
            break;
        case GLUT_KEY_PAGE_DOWN:
            cam_z_angle -= 0.02f;
            up_x = cosf(cam_z_angle);
            up_y = sinf(cam_z_angle);
            break;
    }
}

void keyboard(unsigned char key, int x, int y) {
    (void) x;
    (void) y;

    switch (key) {
        case 'Q':
        case 'q':
            exit(0);
            break;
        case 'W':
        case 'w':
            xpos += sight_x * 0.08f;
            zpos += sight_z * 0.08f;
            break;
        case 'S':
        case 's':
            xpos -= sight_x * 0.08f;
            zpos -= sight_z * 0.08f;
            break;
        case 'A':
        case 'a':
            xpos -= 0.05f;
            break;
        case 'D':
        case 'd':
            xpos += 0.05f;
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
            xpos = 0.0f, ypos = 1.0f, zpos = 2.5f;
            sight_x = 0.0f, sight_y = -0.43f, sight_z = -0.9f;
            up_x = 0.0f, up_y = 1.0f;
            cam_xz_angle = 0.0f, cam_y_angle = -0.45f;
            cam_z_angle = M_PI_2;
            break;
        case 'F':
        case 'f':
            if (fullscreen)
                glutReshapeWindow(windowW, windowH);
            else {
                windowW = glutGet(GLUT_WINDOW_WIDTH);
                windowH = glutGet(GLUT_WINDOW_HEIGHT);
                glutFullScreen();
            }
            fullscreen = !fullscreen;
            break;
        case 'O':
        case 'o':
            orbits = !orbits;
            break;
    }
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowW, windowH);
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
