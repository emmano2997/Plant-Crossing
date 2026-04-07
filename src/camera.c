#include "../include/camera.h"
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int isInsideInterior = 0;

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

static int isFreeCamera = 0; // 0 = Modo Jogador, 1 = Modo Livre
static float playerHeight = 1.5f; // Altura padrão do jogador no chão

// --- VARIÁVEIS DE ANIMAÇÃO ---
int isSleepingAnimation = 0;
static float animProgress = 0.0f;

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

void camera_startSleepAnimation() {
    isSleepingAnimation = 1;
    animProgress = 0.0f;
    isInsideInterior = 0; // Força a câmera para o mundo exterior
}

void camera_updateAnimation() {
    if (!isSleepingAnimation) return;

    animProgress += 0.005f; // Velocidade da animação (0.005 dá uns 3.3 segundos a 60FPS)
    
    if (animProgress >= 1.0f) {
        // FIM DA ANIMAÇÃO - Teleporta de volta para dentro do quarto
        isSleepingAnimation = 0;
        isInsideInterior = 1;
        
        // Posição: Ao lado da cama (-1.5) e alinhado com o centro dela (-2.5)
        camX = -1.5f; 
        camY = -50.0f + playerHeight; 
        camZ = -2.5f; 
        
        // Olha para a porta (Porta interna fica em X=0, Z=5)
        float dx = 0.0f - camX;
        float dz = 5.0f - camZ;
        camYaw = atan2(dz, dx) * 180.0f / M_PI;
        camPitch = 0.0f;
        return;
    }

    // --- MATEMÁTICA DO REDEMOINHO (ESPIRAL 3D) ---
    float hx = -10.0f, hz = 0.0f; // Centro do redemoinho (A Casa)
    
    // Raio diminui de 28.0 (atrás da árvore em X=15) até 2.0 (porta da casa em X=-8)
    float radius = 28.0f * (1.0f - animProgress) + 2.0f * animProgress;
    
    // Ângulo vai de 0 até 2*PI (Uma volta completa ao redor do mundo)
    float angle = animProgress * 2.0f * M_PI; 
    
    // Altura começa em 12.0 (vista aérea) e desce até a altura do jogador (3.0)
    camY = 12.0f * (1.0f - animProgress) + playerHeight * animProgress;

    // Aplica as equações paramétricas polares
    camX = hx + radius * cos(angle);
    camZ = hz + radius * sin(angle);

    // Força a câmera a sempre olhar para o centro da casa durante o redemoinho
    float lookX = -10.0f, lookY = 2.0f, lookZ = 0.0f;
    float dx = lookX - camX;
    float dy = lookY - camY;
    float dz = lookZ - camZ;
    
    camYaw = atan2(dz, dx) * 180.0f / M_PI;
    float distXZ = sqrt(dx*dx + dz*dz);
    camPitch = atan2(dy, distXZ) * 180.0f / M_PI;
}

void camera_processMovement() {
    if (isSleepingAnimation) {
        camera_updateAnimation();
        return; // BLOQUEIA ANDAR

    }else if (isFreeCamera) {
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

int camera_canInteractWithDoor() {
    float raioInteracao = 2.0f;

    if (!isInsideInterior) {
        float hx = -10.0f, hz = 0.0f, hRotDeg = 90.0f;
        float dx = camX - hx;
        float dz = camZ - hz;
        float radLocal = -hRotDeg * M_PI / 180.0f;
        
        float lx = dx * cos(radLocal) - dz * sin(radLocal);
        float lz = dx * sin(radLocal) + dz * cos(radLocal);

        float distDoor = sqrt(lx*lx + (lz - 2.0f)*(lz - 2.0f));
        return (distDoor <= raioInteracao);
    } else {
        float distDoor = sqrt(camX*camX + (camZ - 5.0f)*(camZ - 5.0f));
        return (distDoor <= raioInteracao);
    }
}

void camera_keyDown(unsigned char key, int x, int y) {
    if (key == 27) exit(0);

    if (isSleepingAnimation) return; // BLOQUEIA BOTÕES DO TECLADO DURANTE ANIMAÇÃO
    
    // --- LÓGICA DE TELETRANSPORTE (TECLA Z) ---
    if ((key == 'z' || key == 'Z') && !keys[(int)key]) {
        if (camera_canInteractWithDoor()) {
            if (!isInsideInterior) {
                // Vai para o interior
                camX = 0.0f; 
                camY = -50.0f + playerHeight; 
                camZ = 3.5f; 
                camYaw = -90.0f; 
                camPitch = 0.0f; 
                isInsideInterior = 1;
            } else {
                // Vai para o exterior
                float hx = -10.0f, hz = 0.0f, hRotDeg = 90.0f;
                float spawnLocalZ = 0.0f; 
                float radSpawn = hRotDeg * M_PI / 180.0f;
                
                camX = hx + spawnLocalZ * sin(radSpawn);
                camZ = hz + spawnLocalZ * cos(radSpawn);
                camY = playerHeight; 
                camYaw = 90.0f - hRotDeg;
                camPitch = 0.0f; 
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
    if (isSleepingAnimation) return; // BLOQUEIA OLHAR AO REDOR DURANTE ANIMAÇÃO

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