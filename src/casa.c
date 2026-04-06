#include "../include/casa.h"
#include <GL/glut.h>

// Dimensões base da casa
#define W  2.0f   // meia-largura
#define H  3.0f   // altura das paredes
#define D  2.0f   // meia-profundidade
#define OV 0.3f   // beiral (overhang) do telhado
#define RH 2.2f   // altura do pico do telhado acima das paredes

// Macros auxiliares para reduzir repetição
#define QUAD(x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4) \
    glBegin(GL_QUADS);                                  \
    glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2);        \
    glVertex3f(x3,y3,z3); glVertex3f(x4,y4,z4);        \
    glEnd();

#define TRI(x1,y1,z1, x2,y2,z2, x3,y3,z3) \
    glBegin(GL_TRIANGLES);                  \
    glVertex3f(x1,y1,z1);                  \
    glVertex3f(x2,y2,z2);                  \
    glVertex3f(x3,y3,z3);                  \
    glEnd();

// ── Paredes ───────────────────────────────────────────────────
static void desenhaParedes() {
    glColor3f(0.95f, 0.93f, 0.88f); // creme

    // Frontal — esquerda da porta
    QUAD(-W,   0, D,   -0.5f, 0, D,   -0.5f, H, D,   -W,   H, D)
    // Frontal — direita da porta
    QUAD( 0.5f,0, D,    W,    0, D,    W,    H, D,    0.5f, H, D)
    // Frontal — acima da porta
    QUAD(-0.5f,2.2f,D,  0.5f,2.2f,D,  0.5f,H,D,  -0.5f,H,D)
    // Traseira
    QUAD( W, 0,-D,  -W, 0,-D,  -W, H,-D,   W, H,-D)
    // Lateral esquerda
    QUAD(-W, 0,-D,  -W, 0, D,  -W, H, D,  -W, H,-D)
    // Lateral direita
    QUAD( W, 0, D,   W, 0,-D,   W, H,-D,   W, H, D)
}

// ── Porta ─────────────────────────────────────────────────────
static void desenhaPorta() {
    // Corpo da porta
    glColor3f(0.82f, 0.22f, 0.12f); // vermelho tijolo
    QUAD(-0.5f, 0,    D+0.01f,
          0.5f, 0,    D+0.01f,
          0.5f, 2.2f, D+0.01f,
         -0.5f, 2.2f, D+0.01f)

    // Arco superior (aproximado por 3 quads em leque)
    float ar = 0.5f, ay = 2.2f, az = D+0.01f;
    glColor3f(0.72f, 0.18f, 0.10f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, ay, az);
        for (int i = 0; i <= 6; i++) {
            float ang = 3.14159f * i / 6.0f;
            glVertex3f(-ar * __builtin_cosf(ang), ay + ar * __builtin_sinf(ang), az);
        }
    glEnd();

    // Janela da porta
    glColor3f(0.08f, 0.08f, 0.12f);
    QUAD(-0.22f, 1.55f, D+0.02f,
          0.22f, 1.55f, D+0.02f,
          0.22f, 2.05f, D+0.02f,
         -0.22f, 2.05f, D+0.02f)

    // Cruz da janela
    glColor3f(0.6f, 0.15f, 0.1f);
    // Horizontal
    QUAD(-0.22f,1.77f,D+0.03f,  0.22f,1.77f,D+0.03f,
          0.22f,1.81f,D+0.03f, -0.22f,1.81f,D+0.03f)
    // Vertical
    QUAD(-0.02f,1.55f,D+0.03f,  0.02f,1.55f,D+0.03f,
          0.02f,2.05f,D+0.03f, -0.02f,2.05f,D+0.03f)

    // Maçaneta
    glColor3f(0.75f, 0.60f, 0.1f); // dourado
    glPushMatrix();
        glTranslatef(0.35f, 1.1f, D+0.03f);
        glutSolidSphere(0.06f, 6, 6);
    glPopMatrix();
}

// ── Moldura de madeira ────────────────────────────────────────
static void desenhaMoldura() {
    float bw = 0.13f;
    glColor3f(0.88f, 0.52f, 0.08f); // laranja madeira

    // Vigas verticais frontais
    QUAD(-W,    0, D+0.02f,  -W+bw, 0, D+0.02f,  -W+bw, H, D+0.02f,  -W,   H, D+0.02f)
    QUAD( W-bw, 0, D+0.02f,   W,    0, D+0.02f,   W,    H, D+0.02f,   W-bw,H, D+0.02f)

    // Viga horizontal superior frontal
    QUAD(-W, H-bw, D+0.02f,   W, H-bw, D+0.02f,   W, H, D+0.02f,  -W, H, D+0.02f)

    // Vigas diagonais (estilo enxaimel)
    glBegin(GL_QUADS);
        glVertex3f(-W+bw, H, D+0.02f);
        glVertex3f(-W+bw*2, H, D+0.02f);
        glVertex3f(0, H-bw*2, D+0.02f);
        glVertex3f(0-bw*0.5f, H-bw*2, D+0.02f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f(W-bw*2, H, D+0.02f);
        glVertex3f(W-bw, H, D+0.02f);
        glVertex3f(0+bw*0.5f, H-bw*2, D+0.02f);
        glVertex3f(0, H-bw*2, D+0.02f);
    glEnd();

    // Vigas nas arestas laterais
    QUAD(-W-0.01f, 0, -D,  -W-0.01f, 0, D,  -W-0.01f, H, D,  -W-0.01f, H, -D)
    QUAD( W+0.01f, 0,  D,   W+0.01f, 0,-D,   W+0.01f, H,-D,   W+0.01f, H,  D)
}

// ── Telhado ───────────────────────────────────────────────────
// Ridge corre ao longo do eixo Z (frente-trás), pico no X=0
static void desenhaTelhado() {
    // Vértices do beiral
    float FL[3] = {-W-OV, H,  D+OV};  // frente-esquerda
    float FR[3] = { W+OV, H,  D+OV};  // frente-direita
    float BL[3] = {-W-OV, H, -D-OV};  // trás-esquerda
    float BR[3] = { W+OV, H, -D-OV};  // trás-direita
    // Vértices do cumeeiro
    float RF[3] = {0, H+RH,  D};      // cumeeiro-frente
    float RB[3] = {0, H+RH, -D};      // cumeeiro-trás

    glColor3f(0.88f, 0.62f, 0.10f); // amarelo-laranja

    // Gable frontal (triângulo visível da frente)
    TRI(FL[0],FL[1],FL[2],  FR[0],FR[1],FR[2],  RF[0],RF[1],RF[2])
    // Gable traseiro
    TRI(BR[0],BR[1],BR[2],  BL[0],BL[1],BL[2],  RB[0],RB[1],RB[2])
    // Slope esquerdo
    QUAD(FL[0],FL[1],FL[2],  RF[0],RF[1],RF[2],  RB[0],RB[1],RB[2],  BL[0],BL[1],BL[2])
    // Slope direito
    QUAD(FR[0],FR[1],FR[2],  BR[0],BR[1],BR[2],  RB[0],RB[1],RB[2],  RF[0],RF[1],RF[2])

    // Cumeeira (viga no topo, tom mais escuro)
    glColor3f(0.78f, 0.50f, 0.06f);
    float rw = 0.1f;
    QUAD(-rw, H+RH-rw,  D,   rw, H+RH-rw,  D,   rw, H+RH+rw,  D,  -rw, H+RH+rw,  D)
    QUAD(-rw, H+RH-rw, -D,   rw, H+RH-rw, -D,   rw, H+RH+rw, -D,  -rw, H+RH+rw, -D)
    QUAD(-rw, H+RH+rw, -D,  -rw, H+RH+rw,  D,   rw, H+RH+rw,  D,   rw, H+RH+rw, -D)
}

// ── Chaminé ───────────────────────────────────────────────────
static void desenhaChamine() {
    float cx = 0.7f, cz = -0.6f;
    float cw = 0.25f;
    // altura da base: interpola na inclinação do telhado direito
    // slope direito: de (W+OV, H) até (0, H+RH) em X
    float t = (cx) / (W+OV);       // 0 = cumeeiro, 1 = beira
    float cbase = H + RH * (1-t) - 0.1f;
    float ctop  = cbase + 1.1f;

    glColor3f(0.48f, 0.48f, 0.52f);
    // Quatro faces
    QUAD(cx-cw, cbase, cz+cw,  cx+cw, cbase, cz+cw,  cx+cw, ctop, cz+cw,  cx-cw, ctop, cz+cw)
    QUAD(cx+cw, cbase, cz-cw,  cx-cw, cbase, cz-cw,  cx-cw, ctop, cz-cw,  cx+cw, ctop, cz-cw)
    QUAD(cx-cw, cbase, cz-cw,  cx-cw, cbase, cz+cw,  cx-cw, ctop, cz+cw,  cx-cw, ctop, cz-cw)
    QUAD(cx+cw, cbase, cz+cw,  cx+cw, cbase, cz-cw,  cx+cw, ctop, cz-cw,  cx+cw, ctop, cz+cw)
    // Tampa
    glColor3f(0.38f, 0.38f, 0.42f);
    QUAD(cx-cw, ctop, cz-cw,  cx-cw, ctop, cz+cw,  cx+cw, ctop, cz+cw,  cx+cw, ctop, cz-cw)
}

// ── Degraus ───────────────────────────────────────────────────
static void desenhaDegraus() {
    glColor3f(0.42f, 0.42f, 0.46f);
    // Degrau superior (mais próximo da porta)
    QUAD(-0.55f,0.2f,D+0.05f,  0.55f,0.2f,D+0.05f,  0.55f,0.2f,D+0.35f,  -0.55f,0.2f,D+0.35f)
    QUAD(-0.55f,0,   D+0.35f,  0.55f,0,   D+0.35f,  0.55f,0.2f,D+0.35f,  -0.55f,0.2f,D+0.35f)
    // Degrau inferior
    QUAD(-0.65f,0,D+0.35f,  0.65f,0,D+0.35f,  0.65f,0,D+0.65f,  -0.65f,0,D+0.65f)
    QUAD(-0.65f,0,D+0.65f,  0.65f,0,D+0.65f,  0.65f,0,D+0.65f,  -0.65f,0,D+0.65f)
}

// ── Função pública ────────────────────────────────────────────
void criarCasa() {
    desenhaParedes();
    desenhaPorta();
    desenhaMoldura();
    desenhaTelhado();
    desenhaChamine();
    desenhaDegraus();
}

void desenharCasa(float x, float y, float z, float rotacao) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotacao, 0.0f, 1.0f, 0.0f);
    criarCasa();
    glPopMatrix();
}