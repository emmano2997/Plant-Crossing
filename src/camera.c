#include "../include/camera.h"
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Definição das variáveis externas
float camX = 0.0f, camY = 3.0f, camZ = 15.0f;
float camYaw = -90.0f, camPitch = 0.0f;
float camSpeed = 0.2f;
float sensitivity = 0.15f;
int   keys[256] = {0};
static int mouseButtonHeld = 0;

static int lastMouseX = 400, lastMouseY = 300;
static int firstMouse = 1;

void camera_init() {
    camX = 0.0f; camY = 3.0f; camZ = 15.0f;
    camYaw = -90.0f; camPitch = 0.0f;
    firstMouse = 1;
}

void camera_getFront(float *fx, float *fy, float *fz) {
    float yawR   = camYaw   * M_PI / 180.0f;
    float pitchR = camPitch * M_PI / 180.0f;
    *fx = cos(pitchR) * cos(yawR);
    *fy = sin(pitchR);
    *fz = cos(pitchR) * sin(yawR);
}

static void getRight(float *rx, float *rz) {
    float fx, fy, fz;
    camera_getFront(&fx, &fy, &fz);
    *rx = -fz;
    *rz =  fx;
}

void camera_processMovement() {
    float fx, fy, fz, rx, rz;
    camera_getFront(&fx, &fy, &fz);
    getRight(&rx, &rz);

    if (keys['w'] || keys['W']) { camX += fx*camSpeed; camY += fy*camSpeed; camZ += fz*camSpeed; }
    if (keys['s'] || keys['S']) { camX -= fx*camSpeed; camY -= fy*camSpeed; camZ -= fz*camSpeed; }
    if (keys['a'] || keys['A']) { camX -= rx*camSpeed; camZ -= rz*camSpeed; }
    if (keys['d'] || keys['D']) { camX += rx*camSpeed; camZ += rz*camSpeed; }
    if (keys[' '])              { camY += camSpeed; }
    if (keys['c'] || keys['C']) { camY -= camSpeed; }
}

void camera_lookAt() {
    float fx, fy, fz;
    camera_getFront(&fx, &fy, &fz);
    gluLookAt(
        camX, camY, camZ,
        camX+fx, camY+fy, camZ+fz,
        0.0f, 1.0f, 0.0f
    );
}

void camera_keyDown(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    keys[(int)key] = 1;
}

void camera_keyUp(unsigned char key, int x, int y) {
    keys[(int)key] = 0;
}

// Função para capturar clique do mouse
void camera_mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mouseButtonHeld = (state == GLUT_DOWN);
        if (mouseButtonHeld) {
            lastMouseX = x;
            lastMouseY = y;
            firstMouse = 0;
        }
    }
}

// Mude camera_mouseMotion para só agir quando o botão estiver pressionado
void camera_mouseMotion(int x, int y) {
    if (!mouseButtonHeld) return; // ← só rotaciona se botão pressionado

    camYaw   += (x - lastMouseX) * sensitivity;
    camPitch += (lastMouseY - y) * sensitivity;

    lastMouseX = x;
    lastMouseY = y;

    if (camPitch >  89.0f) camPitch =  89.0f;
    if (camPitch < -89.0f) camPitch = -89.0f;
}