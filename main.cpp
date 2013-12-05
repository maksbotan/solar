
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

static GLfloat xpos = 0.0f, ypos = ASTRONOMIC_UNIT * 1.0f, zpos = ASTRONOMIC_UNIT * 2.5f;
static GLfloat sight_x = 0.0f, sight_y = -0.43f, sight_z = -0.9f;
static GLfloat up_x = 0.0f, up_y = 1.0f, up_z = 0.0f;
static bool orbits = false, running = true, vsync = true, help = false;
static Uint32 last_time = 0, frames = 0;
static SDL_Window *window = NULL;
static TTF_Font *font = NULL;
static int speed_factor = 1;

void cross_product(const GLfloat a_x, const GLfloat a_y, const GLfloat a_z,
                   const GLfloat b_x, const GLfloat b_y, const GLfloat b_z,
                   GLfloat &c_x, GLfloat &c_y, GLfloat &c_z) {
    c_x = a_y * b_z - a_z * b_y;
    c_y = a_z * b_x - a_x * b_z;
    c_z = a_x * b_y - a_y * b_x;
}

void normalize_vector(GLfloat &x, GLfloat &y, GLfloat &z) {
    GLfloat norm = sqrt(x*x + y*y + z*z);
    x /= norm; y /= norm; z /= norm;
}

/*
 * This function takes the vector (dx, dy, dz) in camera basis (sight, up, side) and
 * transforms it into world coordinates. Result is stored in (x, y, z).
 */
void translate_in_camera_basis(const GLfloat dx, const GLfloat dy, const GLfloat dz, GLfloat &x, GLfloat &y, GLfloat &z) {
    GLfloat side_x, side_y, side_z;
    cross_product(sight_x, sight_y, sight_z, up_x, up_y, up_z, side_x, side_y, side_z);
    normalize_vector(side_x, side_y, side_z);
    GLfloat temp_x, temp_y, temp_z;

    // Now simple transition matrix multiplication
    temp_x = sight_x * dx + up_x * dy + side_x * dz;
    temp_y = sight_y * dx + up_y * dy + side_y * dz;
    temp_z = sight_z * dx + up_z * dy + side_z * dz;

    normalize_vector(temp_x, temp_y, temp_z);

    x = temp_x; y = temp_y; z = temp_z;
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(xpos,           ypos,           zpos,
              xpos + sight_x, ypos + sight_y, zpos + sight_z,
              up_x,           up_y,           up_z);

    GLfloat sun_p[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sun_p);

    //drawAxes();
    drawSky();

    drawSun();
    drawPlanets(orbits);
    if (font)
        drawStats(font, 1000 * frames / SDL_GetTicks(), help);

    SDL_GL_SwapWindow(window);
}

void reshape(int w, int h) {
    if (!h)
        h = 1;

    glViewport(0, 0, (GLint) w, (GLint) h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (float) w/h,  0.1f / ASTRONOMIC_UNIT, ASTRONOMIC_UNIT * 50.0f);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(SDL_Scancode key) {
    Uint32 flags;

    GLfloat side_x, side_y, side_z;

    switch (key) {
        case SDL_SCANCODE_Q:
            running = false;
            break;
        case SDL_SCANCODE_W:
            xpos += sight_x * 0.008f * speed_factor;
            ypos += sight_y * 0.008f * speed_factor;
            zpos += sight_z * 0.008f * speed_factor;
            break;
        case SDL_SCANCODE_S:
            xpos -= sight_x * 0.008f * speed_factor;
            ypos -= sight_y * 0.008f * speed_factor;
            zpos -= sight_z * 0.008f * speed_factor;
            break;
        case SDL_SCANCODE_A:
            cross_product(up_x, up_y, up_z, sight_x, sight_y, sight_z, side_x, side_y, side_z);
            normalize_vector(side_x, side_y, side_z);
            xpos += side_x * 0.008f * speed_factor;
            ypos += side_y * 0.008f * speed_factor;
            zpos += side_z * 0.008f * speed_factor;
            break;
        case SDL_SCANCODE_D:
            cross_product(sight_x, sight_y, sight_z, up_x, up_y, up_z, side_x, side_y, side_z);
            normalize_vector(side_x, side_y, side_z);
            xpos += side_x * 0.008f * speed_factor;
            ypos += side_y * 0.008f * speed_factor;
            zpos += side_z * 0.008f * speed_factor;
            break;
        case SDL_SCANCODE_Z:
            xpos += up_x * 0.008f * speed_factor;
            ypos += up_y * 0.008f * speed_factor;
            zpos += up_z * 0.008f * speed_factor;
            break;
        case SDL_SCANCODE_X:
            xpos -= up_x * 0.008f * speed_factor;
            ypos -= up_y * 0.008f * speed_factor;
            zpos -= up_z * 0.008f * speed_factor;
            break;
        case SDL_SCANCODE_R:
            xpos = 0.0f, ypos = ASTRONOMIC_UNIT * 1.0f, zpos = ASTRONOMIC_UNIT * 2.5f;
            sight_x = 0.0f, sight_y = -0.43f, sight_z = -0.9f;
            up_x = 0.0f, up_y = 1.0f;
            cross_product(sight_x, sight_y, sight_z, up_x, up_y, up_z, side_x, side_y, side_z);
            cross_product(side_x, side_y, side_z, sight_x, sight_y, sight_z, up_x, up_y, up_z);
            normalize_vector(up_x, up_y, up_z);
            break;
        case SDL_SCANCODE_F:
            flags = SDL_GetWindowFlags(window);
            SDL_SetWindowFullscreen(window, flags ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
            break;
        case SDL_SCANCODE_O:
            orbits = !orbits;
            break;
        // First two rotations are easy, because the do not change the up vector
        case SDL_SCANCODE_LEFT:
            translate_in_camera_basis(cosf(-0.02f), 0.0f, sinf(-0.02f), sight_x, sight_y, sight_z);
            break;
        case SDL_SCANCODE_RIGHT:
            translate_in_camera_basis(cosf(0.02f), 0.0f, sinf(0.02f), sight_x, sight_y, sight_z);
            break;
        // In the second two rotations we need to recalculate up vector as [side x sight]
        case SDL_SCANCODE_UP:
            cross_product(sight_x, sight_y, sight_z, up_x, up_y, up_z, side_x, side_y, side_z);
            translate_in_camera_basis(cosf(0.02f), sinf(0.02f), 0.0f, sight_x, sight_y, sight_z);
            cross_product(side_x, side_y, side_z, sight_x, sight_y, sight_z, up_x, up_y, up_z);
            normalize_vector(sight_x, sight_y, sight_z);
            break;
        case SDL_SCANCODE_DOWN:
            cross_product(sight_x, sight_y, sight_z, up_x, up_y, up_z, side_x, side_y, side_z);
            translate_in_camera_basis(cosf(-0.02f), sinf(-0.02f), 0.0f, sight_x, sight_y, sight_z);
            cross_product(side_x, side_y, side_z, sight_x, sight_y, sight_z, up_x, up_y, up_z);
            normalize_vector(sight_x, sight_y, sight_z);
            break;
        // Last two do not change the sight vector
        case SDL_SCANCODE_PAGEUP:
            translate_in_camera_basis(0.0f, cosf(-0.02f), sinf(-0.02f), up_x, up_y, up_z);
            break;
        case SDL_SCANCODE_PAGEDOWN:
            translate_in_camera_basis(0.0f, cosf(0.02f), sinf(0.02f), up_x, up_y, up_z);
            break;
        case SDL_SCANCODE_T:
            if (speed_factor == 100)
                speed_factor = 1;
            else
                speed_factor = 100;
            break;
        case SDL_SCANCODE_V:
            vsync = !vsync;
            SDL_GL_SetSwapInterval(vsync);
            break;
        case SDL_SCANCODE_H:
            help = !help;
            break;
        default:
            break;
    }
}

void mouse(Sint32 x, Sint32 y) {
    if (x < 10 || x > 60 || y < 79)
        return;

    Sint32 y1 = 79;
    size_t i;

    for (i = 0; i < planets.size(); i++) {
        if (y > y1 && y < y1 + 50)
            break;
        y1 += 59;
    }

    if (i == planets.size())
        return;

    GLfloat side_x, side_y, side_z;

    planets[i].generateLookAt(xpos, ypos, zpos, sight_x, sight_y, sight_z, up_x, up_y, up_z);
    cross_product(sight_x, sight_y, sight_z, up_x, up_y, up_z, side_x, side_y, side_z);
    cross_product(side_x, side_y, side_z, sight_x, sight_y, sight_z, up_x, up_y, up_z);
    normalize_vector(up_x, up_y, up_z);
}

int main(int, char **) {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    window = SDL_CreateWindow("Solar system", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(vsync); // Enable VSYNC
    reshape(WIDTH, HEIGHT); // SDL does not send resize event on startup

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

    GLfloat side_x, side_y, side_z;
    cross_product(sight_x, sight_y, sight_z, up_x, up_y, up_z, side_x, side_y, side_z);
    cross_product(side_x, side_y, side_z, sight_x, sight_y, sight_z, up_x, up_y, up_z);
    normalize_vector(up_x, up_y, up_z);

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
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        mouse(event.button.x, event.button.y);
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
    freeTextures();

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
