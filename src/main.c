#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#include "../include/camera.h"
#include "../include/arvore.h"
#include "../include/casa.h"
#include "../include/casa_interior.h"  
#include "../include/mundo.h"

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
        if (px + PR >  tamanho) return 1;
        // Se X - raio < -5.0 (Parede Esquerda)
        if (px - PR < -tamanho) return 1;
        // Se Z + raio > 5.0 (Parede Frontal)
        if (pz + PR >  tamanho) return 1;
        // Se Z - raio < -5.0 (Parede Traseira)
        if (pz - PR < -tamanho) return 1;

        // --- COLISÃO DOS MÓVEIS (AABB Geometrics) ---
        // IN_BOX inflaciona automaticamente com o raio PR, criando a malha de colisão
        
        // 1. Cama (No canto traseiro esquerdo)
        if (IN_BOX(px, pz, -5.0f, -2.0f, -5.0f, -1.0f)) return 1;
        
        // 2. Mesa de Cabeceira (Ao lado da cama)
        if (IN_BOX(px, pz, -1.8f, -0.6f, -5.0f, -3.8f)) return 1;
        
        // 3. Armário (No canto traseiro direito)
        if (IN_BOX(px, pz, 2.0f, 5.0f, -5.0f, -3.2f)) return 1;
        
        // 4. Mesa (Centralizada na parede direita)
        if (IN_BOX(px, pz, 3.0f, 5.0f, -1.4f, 1.4f)) return 1;
        
        // 5. Cadeira (Na frente da mesa, inclui área de folga para a rotação)
        if (IN_BOX(px, pz, 1.6f, 3.0f, -0.6f, 0.6f)) return 1;

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

        // --- COLISÃO DA ÁRVORE (Cilindro) ---
        // Calcula a distância do jogador até o centro da árvore
        float distArvore = sqrt((px - ARVORE_X)*(px - ARVORE_X) + (pz - ARVORE_Z)*(pz - ARVORE_Z));
        float raioTronco = 0.5f; // Raio físico estimado do tronco
        
        // Se a distância for menor que o tronco + a barriga do jogador (PR), colide!
        if (distArvore < raioTronco + PR) return 1;

        // --- COLISÃO DAS CERCAS (Limites do Mundo) ---
        // Como o mundo vai de -30 a 30, limitamos o jogador em um quadrado de ±29.0
        float limiteMundo = 29.0f; 
        
        if (px + PR > limiteMundo) return 1;  // Bateu na cerca Direita
        if (px - PR < -limiteMundo) return 1; // Bateu na cerca Esquerda
        if (pz + PR > limiteMundo) return 1;  // Bateu na cerca Frontal
        if (pz - PR < -limiteMundo) return 1; // Bateu na cerca Traseira

        return 0; 
    }
}

// --- NOVA FUNÇÃO: Renderizador de UI 2D ---
void renderText2D(float x, float y, const char *string) {
    // Pega o tamanho atual da janela para o mapeamento 2D
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    // Salva matriz de projeção 3D e entra no modo 2D ortográfico
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height); 

    // Salva matriz de modelagem e reseta
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Desativa testes 3D para desenhar "por cima" de tudo
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING); // Caso você use luzes futuramente

    // Posiciona e desenha o texto
    glRasterPos2f(x, y);
    for (const char *c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Restaura configurações 3D
    glEnable(GL_DEPTH_TEST);
    
    // Restaura as matrizes
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// --- Checa se o jogador está perto da cama ---
int camera_canInteractWithBed() {
    // Acessa as variáveis globais da câmera e estado do interior
    extern float camX, camZ;
    extern int isInsideInterior;

    // Só pode interagir com a cama se estiver dentro de casa
    if (isInsideInterior) {
        // Centro da cama baseado no seu casa_interior.c (X = -3.5f, Z = -3.0f)
        float bedX = -3.5f;
        float bedZ = -3.0f;

        // Calcula a distância do jogador até o centro da cama
        float dx = camX - bedX;
        float dz = camZ - bedZ;
        float distBed = sqrt(dx*dx + dz*dz);

        // Raio de alcance (3.0f é o suficiente para englobar a cama e a mesa de cabeceira)
        float raioInteracaoCama = 3.0f;

        if (distBed <= raioInteracaoCama) {
            return 1; // Pode dormir!
        }
    }
    return 0; // Muito longe ou do lado de fora
}

// --- Checa se o jogador está perto da árvore ---
int camera_canInteractWithTree() {
    extern float camX, camZ;
    extern int isInsideInterior;

    // Só pode interagir com a árvore se estiver do lado de fora
    if (!isInsideInterior) {
        float dx = camX - ARVORE_X;
        float dz = camZ - ARVORE_Z;
        float distTree = sqrt(dx*dx + dz*dz);
        
        float raioInteracaoArvore = 4.0f; // Distância para alcançar a planta

        if (distTree <= raioInteracaoArvore) {
            return 1;
        }
    }
    return 0;
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

    desenharRegador(ARVORE_X + 0.5f, ARVORE_Y, ARVORE_Z -0.8f);

    // Casa
    desenharCasa(CASA_X, CASA_Y, CASA_Z, CASA_ROT);
    // Desenha o interior expandido lá embaixo (Y = -50)
    desenharInteriorExpandido(0.0f, -50.0f, 0.0f);

    // --- DESENHO DO HUD / UI ---
    extern int isSleepingAnimation;
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    if (!isSleepingAnimation) {
        // 1. Interação com a Porta
        if (camera_canInteractWithDoor()) {
            glColor3f(0.0f, 0.0f, 0.0f); 
            if (isInsideInterior) renderText2D(21.0f, screenHeight - 39.0f, "'Z' para Sair");
            else renderText2D(21.0f, screenHeight - 39.0f, "'Z' para Entrar");

            glColor3f(1.0f, 1.0f, 0.0f); 
            if (isInsideInterior) renderText2D(20.0f, screenHeight - 40.0f, "'Z' para Sair");
            else renderText2D(20.0f, screenHeight - 40.0f, "'Z' para Entrar");
        }

        // 2. Interação com a Cama
        if (camera_canInteractWithBed()) {
            glColor3f(0.0f, 0.0f, 0.0f); 
            renderText2D(21.0f, screenHeight - 69.0f, "'F' para Dormir");
            
            glColor3f(1.0f, 1.0f, 0.0f); 
            renderText2D(20.0f, screenHeight - 70.0f, "'F' para Dormir");
        }

        // 3. Interação com a Árvore
        // Acessa as variáveis de estado da planta que estão em arvore.h
        extern int regouEssaEstacao;
        extern int estagioPlantas;

        if (camera_canInteractWithTree() && !regouEssaEstacao && estagioPlantas != 4) {
            // Colocamos o Y mais para baixo (-100) para não encavalar com os outros textos
            glColor3f(0.0f, 0.0f, 0.0f); 
            renderText2D(21.0f, screenHeight - 99.0f, "'R' para Regar");
            
            glColor3f(0.2f, 0.8f, 1.0f); // Azul claro para combinar com a água!
            renderText2D(20.0f, screenHeight - 100.0f, "'R' para Regar");
        }
    }

    glutSwapBuffers();
}

void teclado(unsigned char key, int x, int y) {
    // Impede o jogador de tentar regar ou dormir DE NOVO enquanto o mundo gira
    extern int isSleepingAnimation;
    if (isSleepingAnimation) return; 

    switch (key) {
        case 'r': 
        case 'R':
            if (camera_canInteractWithTree()) {
                arvore_regar();
            } else {
                printf("Voce precisa chegar mais perto da arvore para rega-la!\n");
            }
            break;
 
        case 'f': 
        case 'F': {
            if (camera_canInteractWithBed()) {
                int morreu = arvore_dormir();
                if (morreu) {
                    printf("Sua planta morreu! Tente novamente.\n");
                }
                
                // INICIA A CUTSCENE AQUI
                camera_startSleepAnimation();
                
            } else {
                printf("Voce precisa estar perto da sua cama para dormir!\n");
            }
            break;
        }
    }
    camera_keyDown(key, x, y);
}

void update(int value) {
    arvore_update();
    mundo_update(estacaoAtual);
    camera_processMovement();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
    
}
// ─── Mudancas na main.c para a iluminacao 
//
// 1. No init(), APOS camera_init(), adicionar:
//
//    mundo_iluminacao_init();
//
//    Por que aqui?
//    glEnable(GL_LIGHTING) so funciona depois que a janela OpenGL
//    foi criada (glutCreateWindow). O init() ja e chamado apos isso,
//    entao e o lugar certo. Colocar antes de glutMainLoop garante
//    que a luz esta configurada antes do primeiro frame ser desenhado.

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0/600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    arvore_init();
    camera_init();
    mundo_iluminacao_init();
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
