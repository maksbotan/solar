
// windows.h must be included before GL headers
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <windows.h>
#endif
#include <math.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>

#include <GL/gl.h>
#include <GL/glu.h>
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
static bool orbits = false, running = true;
static Uint32 last_time = 0, frames = 0;
static SDL_Window *window = NULL;
static TTF_Font *font = NULL;

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
    if (font)
        drawStats(font, 1000 * frames / SDL_GetTicks());

    SDL_GL_SwapWindow(window);
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

void keyboard(SDL_Scancode key) {
    Uint32 flags;

    switch (key) {
        case SDL_SCANCODE_Q:
            running = false;
            break;
        case SDL_SCANCODE_W:
            xpos += sight_x * 0.08f;
            ypos += sight_y * 0.08f;
            zpos += sight_z * 0.08f;
            break;
        case SDL_SCANCODE_S:
            xpos -= sight_x * 0.08f;
            ypos -= sight_y * 0.08f;
            zpos -= sight_z * 0.08f;
            break;
        case SDL_SCANCODE_A:
            xpos -= -sight_z * 0.08f;
            zpos -= sight_x * 0.08f;
            break;
        case SDL_SCANCODE_D:
            xpos += -sight_z * 0.08f;
            zpos += sight_x * 0.08f;
            break;
        case SDL_SCANCODE_Z:
            ypos += -sight_z * 0.08f;
            zpos += sight_y * 0.08f;
            break;
        case SDL_SCANCODE_X:
            ypos -= -sight_z * 0.08f;
            zpos -= sight_y * 0.08f;
            break;
        case SDL_SCANCODE_R:
            xpos = 0.0f, ypos = 1.0f, zpos = 2.5f;
            sight_x = 0.0f, sight_y = -0.43f, sight_z = -0.9f;
            up_x = 0.0f, up_y = 1.0f;
            cam_xz_angle = 0.0f, cam_y_angle = -0.45f;
            cam_z_angle = M_PI_2;
            break;
        case SDL_SCANCODE_F:
            flags = SDL_GetWindowFlags(window);
            SDL_SetWindowFullscreen(window, flags ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
            break;
        case SDL_SCANCODE_O:
            orbits = !orbits;
            break;
        case SDL_SCANCODE_LEFT:
            cam_xz_angle -= 0.02f;
            sight_x = cosf(cam_y_angle) * sinf(cam_xz_angle);
            sight_z = -cosf(cam_y_angle) * cosf(cam_xz_angle);
            break;
        case SDL_SCANCODE_RIGHT:
            cam_xz_angle += 0.02f;
            sight_x = cosf(cam_y_angle) * sinf(cam_xz_angle);
            sight_z = -cosf(cam_y_angle) * cosf(cam_xz_angle);
            break;
        case SDL_SCANCODE_UP:
            if (cam_y_angle > M_PI / 3) // Do not look up to avoid gimbal locks and other problems
                break;
            cam_y_angle += 0.02;
            sight_x = cosf(cam_y_angle) * sinf(cam_xz_angle);
            sight_y = sinf(cam_y_angle);
            sight_z = -cosf(cam_y_angle) * cosf(cam_xz_angle);
            break;
        case SDL_SCANCODE_DOWN:
            if (cam_y_angle < -M_PI / 3) // And do not look down
                break;
            cam_y_angle -= 0.02f;
            sight_x = cosf(cam_y_angle) * sinf(cam_xz_angle);
            sight_y = sinf(cam_y_angle);
            sight_z = -cosf(cam_y_angle) * cosf(cam_xz_angle);
            break;
        case SDL_SCANCODE_PAGEUP:
            cam_z_angle += 0.02f;
            up_x = cosf(cam_z_angle);
            up_y = sinf(cam_z_angle);
            break;
        case SDL_SCANCODE_PAGEDOWN:
            cam_z_angle -= 0.02f;
            up_x = cosf(cam_z_angle);
            up_y = sinf(cam_z_angle);
            break;
        default:
            break;
    }
}

int main(int, char **) {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    window = SDL_CreateWindow("Solar system", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable VSYNC

    font = TTF_OpenFont("Vera.ttf", 16);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat sun_d[] = {7.0f, 7.0f, 7.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_d);
    glEnable(GL_LIGHT0);

    glClearDepth(1.0);

    initPlanets();

    starsTexture = loadBMPTexture("textures/starmap.bmp");
    sunTexture = loadBMPTexture("textures/sun.bmp");

    last_time = SDL_GetTicks();
    Uint32 dt = 1000 / FPS, delta = 0;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                        reshape(event.window.data1, event.window.data2);
                    break;
                case SDL_KEYDOWN:
                    keyboard(event.key.keysym.scancode);
                    break;
            }

        renderScene();
        Uint32 time = SDL_GetTicks();
        delta += time - last_time;
        while (delta >= dt) { // Maintain constant physics step and free framerate
            physicsStep(dt);
            delta -= dt;
        }
        frames++;
        last_time = time;
    }

    glDeleteTextures(1, &starsTexture);
    glDeleteTextures(1, &sunTexture);

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
