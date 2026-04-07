#include "../include/camera.h"
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int isInsideInterior = 0;

extern int checkWorldCollision(float x, float z);

// Definição das variáveis externas
float camX = 0.0f, camY = 3.0f, camZ = 15.0f;
float camYaw = -90.0f, camPitch = 0.0f;
float camSpeed = 0.2f;
float sensitivity = 0.15f;
int   keys[256] = {0};

// Variáveis de controle do mouse e modo de câmera
static int mouseButtonHeld = 0;
static int lastMouseX = 400, lastMouseY = 300;
static int firstMouse = 1;

// --- NOVAS VARIÁVEIS DE ESTADO ---
static int isFreeCamera = 0; // 0 = Modo Jogador, 1 = Modo Livre
static float playerHeight = 1.5f; // Altura padrão do jogador no chão

void camera_init() {
    camX = 0.0f; camY = playerHeight; camZ = 15.0f;
    camYaw = -90.0f; camPitch = 0.0f;
    firstMouse = 1;
    isFreeCamera = 0; // Inicia como jogador
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
    if (isFreeCamera) {
        // --- MODO LIVRE (Voo) ---
        float fx, fy, fz, rx, rz;
        camera_getFront(&fx, &fy, &fz);
        getRight(&rx, &rz);

        if (keys['w'] || keys['W']) { camX += fx*camSpeed; camY += fy*camSpeed; camZ += fz*camSpeed; }
        if (keys['s'] || keys['S']) { camX -= fx*camSpeed; camY -= fy*camSpeed; camZ -= fz*camSpeed; }
        if (keys['a'] || keys['A']) { camX -= rx*camSpeed; camZ -= rz*camSpeed; }
        if (keys['d'] || keys['D']) { camX += rx*camSpeed; camZ += rz*camSpeed; }
        if (keys[' '])              { camY += camSpeed; } // Sobe
        if (keys['x'] || keys['X']) { camY -= camSpeed; } // Muda para X para não conflitar com o C
        
    } else {
        // --- MODO JOGADOR (FPS) ---
        float yawR = camYaw * M_PI / 180.0f;
        
        // Direções baseadas no ângulo da câmera
        float dirX = cos(yawR);
        float dirZ = sin(yawR);
        float rightX = -dirZ; 
        float rightZ = dirX;

        // Armazena a intenção de movimento (para onde o jogador QUER ir)
        float intentX = 0.0f;
        float intentZ = 0.0f;

        if (keys['w'] || keys['W']) { intentX += dirX * camSpeed; intentZ += dirZ * camSpeed; }
        if (keys['s'] || keys['S']) { intentX -= dirX * camSpeed; intentZ -= dirZ * camSpeed; }
        if (keys['a'] || keys['A']) { intentX -= rightX * camSpeed; intentZ -= rightZ * camSpeed; }
        if (keys['d'] || keys['D']) { intentX += rightX * camSpeed; intentZ += rightZ * camSpeed; }

        // --- SISTEMA DE DESLIZAMENTO (SLIDING) ---
        // Checamos o eixo X e o eixo Z de forma independente.
        // Se bater no eixo X, ele não move o X, mas ainda tenta mover o Z.
        
        if (!checkWorldCollision(camX + intentX, camZ)) {
            camX += intentX;
        }
        if (!checkWorldCollision(camX, camZ + intentZ)) {
            camZ += intentZ;
        }
    }
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
    if (key == 27) exit(0); // Esc

// --- LÓGICA DE TELETRANSPORTE (TECLA Z) COM RAIO DE INTERAÇÃO ---
    if ((key == 'z' || key == 'Z') && !keys[(int)key]) {
        float raioInteracao = 2.0f; // Distância máxima para interagir

        if (!isInsideInterior) {
            // --- MUNDO EXTERIOR PARA INTERIOR ---
            float hx = -10.0f, hz = 0.0f, hRotDeg = 90.0f;
            
            float dx = camX - hx;
            float dz = camZ - hz;
            float radLocal = -hRotDeg * M_PI / 180.0f;
            
            float lx = dx * cos(radLocal) - dz * sin(radLocal);
            float lz = dx * sin(radLocal) + dz * cos(radLocal);

            float distDoor = sqrt(lx*lx + (lz - 2.0f)*(lz - 2.0f));

            if (distDoor <= raioInteracao) {
                // Teletransporta para a sala subterrânea
                camX = 0.0f; 
                camY = -50.0f + playerHeight; 
                camZ = 3.5f; // Fica a 1.5 unidades de distância da porta (Z=5.0)
                
                camYaw = -90.0f; // Olha para o fundo da sala (-Z)
                camPitch = 0.0f; 
                
                isInsideInterior = 1;
            }
        } else {
            // --- INTERIOR PARA MUNDO EXTERIOR ---
            // A porta interna está em X = 0 e Z = 5.0
            float distDoor = sqrt(camX*camX + (camZ - 5.0f)*(camZ - 5.0f));

            if (distDoor <= raioInteracao) {
                // Dados da casa no exterior (devem ser os mesmos usados acima e no main.c)
                float hx = -10.0f, hz = 0.0f, hRotDeg = 90.0f;
                
                // Distância que o jogador deve aparecer à frente da porta externa
                float spawnLocalZ = 0.5f; 
                
                // Converte a rotação da casa para radianos
                float radSpawn = hRotDeg * M_PI / 180.0f;
                
                // Calcula a posição global (X e Z) usando a matriz de rotação
                camX = hx + spawnLocalZ * sin(radSpawn);
                camZ = hz + spawnLocalZ * cos(radSpawn);
                camY = playerHeight; // Retorna para a altura do chão principal
                
                // Calcula para onde o jogador deve olhar (Yaw)
                // Uma casa sem rotação tem a porta virada para +Z (Yaw 90). 
                // Subtraímos a rotação da casa para a câmera alinhar com o mundo.
                camYaw = 90.0f - hRotDeg;
                camPitch = 0.0f; // Nivele a visão do jogador
                
                isInsideInterior = 0;
            }
        }
    }
    
    // --- LÓGICA DE ALTERNÂNCIA (TOGGLE) ---
    // A verificação '!keys[(int)key]' garante que o código rode apenas uma vez quando a tecla é pressionada,
    // evitando que a câmera fique piscando entre os modos se você segurar a tecla C.
    if ((key == 'c' || key == 'C') && !keys[(int)key]) {
        isFreeCamera = !isFreeCamera; // Inverte o estado
        
        if (!isFreeCamera) {
            // Se voltou para o modo jogador, trava a altura de volta para o nível do chão
            camY = playerHeight; 
        }
    }

    keys[(int)key] = 1;
}

void camera_keyUp(unsigned char key, int x, int y) {
    keys[(int)key] = 0;
}

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

void camera_mouseMotion(int x, int y) {
    if (!mouseButtonHeld) return;

    camYaw   += (x - lastMouseX) * sensitivity;
    camPitch += (lastMouseY - y) * sensitivity;

    lastMouseX = x;
    lastMouseY = y;

    if (camPitch >  89.0f) camPitch =  89.0f;
    if (camPitch < -89.0f) camPitch = -89.0f;
}