#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#include "../include/camera.h"
#include "../include/arvore.h"
#include "../include/casa.h"
#include "../include/casa_interior.h"  

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

#define CASA_X      -15.0f
#define CASA_Y      0.0f
#define CASA_Z      0.0f
#define CASA_ROT    90.0f

#define ARVORE_X    15.0f
#define ARVORE_Y    0.0f
#define ARVORE_Z    0.0f
#define ARVORE_ROT  0.0f

// Macro auxiliar para verificar se o ponto está dentro de uma caixa retangular
#define IN_BOX(x, z, minX, maxX, minZ, maxZ) \
    ((x) > (minX) - PR && (x) < (maxX) + PR && \
     (z) > (minZ) - PR && (z) < (maxZ) + PR)


// Função que o camera.c chama para saber se pode andar
int checkWorldCollision(float px, float pz) {
    extern float camY;

    // Raio de colisão do jogador (espessura do corpo)
    float PR = 0.6f; 

    if (camY < -25.0f) {
        // --- COLISÃO DO INTERIOR (SALA 10x10) ---
        // A sala expandida está centralizada em (0, -50, 0) com meia-largura 5.0
        float tamanho = 5.0f;

        // Verificamos se a posição da câmera + o seu raio ultrapassa as paredes
        // Se X + raio > 5.0 (Parede Direita)
        if (px + PR + 0.6f >  tamanho) return 1;
        // Se X - raio < -5.0 (Parede Esquerda)
        if (px - PR - 0.6f < -tamanho) return 1;
        // Se Z + raio > 5.0 (Parede Frontal)
        if (pz + PR + 0.6f >  tamanho) return 1;
        // Se Z - raio < -5.0 (Parede Traseira)
        if (pz - PR - 0.6f < -tamanho) return 1;

        return 0;
    } else {
        // --- COLISÃO DO EXTERIOR (CASA) ---
        float dx = px - CASA_X;
        float dz = pz - CASA_Z;
        
        float rad = -CASA_ROT * (float)M_PI / 180.0f;
        float lx = dx * cosf(rad) - dz * sinf(rad);
        float lz = dx * sinf(rad) + dz * cosf(rad);

        float W = 2.0f;
        float D = 2.0f;
        float WT = 0.2f;

        // Parede Esquerda
        if (IN_BOX(lx, lz, -W - PR, -W + WT + PR, -D - PR, D + PR)) return 1;
        
        // Parede Direita
        if (IN_BOX(lx, lz, W - WT - PR, W + PR, -D - PR, D + PR)) return 1;
        
        // Parede Traseira
        if (IN_BOX(lx, lz, -W - PR, W + PR, -D - PR, -D + WT + PR)) return 1;
        
        // Parede Frontal (Pedaço esquerdo da porta)
        if (IN_BOX(lx, lz, -W - PR, -0.5f + PR, D - WT - PR, D + PR)) return 1;
        
        // Parede Frontal (Pedaço direito da porta)
        if (IN_BOX(lx, lz, 0.5f - PR, W + PR, D - WT - PR, D + PR)) return 1;

        return 0; 
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera_lookAt(); // ← toda lógica de câmera encapsulada aqui

    // cenario
    mundo_desenhar(estacaoAtual);

    // Árvore
    desenharArvore(ARVORE_X, ARVORE_Y, ARVORE_Z, ARVORE_ROT);

    // Casa
    desenharCasa(CASA_X, CASA_Y, CASA_Z, CASA_ROT);
    // Desenha o interior expandido lá embaixo (Y = -50)
    desenharInteriorExpandido(0.0f, -50.0f, 0.0f);

    glutSwapBuffers();
}
void teclado(unsigned char key, int x, int y) {
    switch (key) {
        case 'r': 
            // Jogador regou a planta
            arvore_regar();
            break;
 
        case 'f':  {
            // Jogador dormiu — passa a estacao
            int morreu = arvore_dormir();
            if (morreu) {
                printf("Sua planta morreu! Tente novamente.\n");
            }
            break;
        }
    }
    // repassa para a camera (nao quebra os controles do colega)
    camera_keyDown(key, x, y);
}

void update(int value) {
    arvore_update();
    camera_processMovement();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0/600.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    arvore_init();
    camera_init();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Projeto CG - plant crossing");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);       // ← callbacks da câmera
    glutKeyboardUpFunc(camera_keyUp);
    glutMouseFunc(camera_mouseButton);
    glutMotionFunc(camera_mouseMotion); 
    
    glutTimerFunc(16, update, 0);
   

    glutMainLoop();
    return 0;
}
// ─── Resumo dos controles ─────────────────────────────────────────────
// C  →  alterna entre camera jogador e camera livre (free look)
// Z  →  teletransporta para dentro da casa (e volta para fora se ja estiver dentro)
// R  →  rega a planta (so funciona uma vez por estacao)
// F  →  dorme / passa para proxima estacao
//        se regou: planta cresce (estagio++)
//        se nao regou: planta regride (estagio--)
//        se era brotinho sem rega: MORRE (estagio=4)
