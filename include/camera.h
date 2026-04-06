#ifndef CAMERA_H
#define CAMERA_H

// Estado da câmera (acessível por outros arquivos)
extern float camX, camY, camZ;
extern float camYaw, camPitch;
extern int   keys[256];

void camera_init();
void camera_getFront(float *fx, float *fy, float *fz);
void camera_processMovement();
void camera_lookAt();

// Callbacks — registre no main com glutKeyboardFunc etc.
void camera_keyDown(unsigned char key, int x, int y);
void camera_keyUp(unsigned char key, int x, int y);
void camera_mouseMotion(int x, int y);
void camera_mouseButton(int button, int state, int x, int y);

#endif