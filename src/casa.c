#include "../include/casa.h"
#include <GL/glut.h>
#include <math.h>

// Dimensões base da casa
#define W  2.0f   // meia-largura
#define H  3.0f   // altura das paredes
#define D  2.0f   // meia-profundidade
#define OV 0.3f   // beiral (overhang) do telhado
#define RH 2.2f   // altura do pico do telhado acima das paredes
#define WT 0.2f   // espessura da parede (NOVO)

// Macros auxiliares
#define QUAD(x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4) \
    glBegin(GL_QUADS);                               \
    glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2);      \
    glVertex3f(x3,y3,z3); glVertex3f(x4,y4,z4);      \
    glEnd();

#define TRI(x1,y1,z1, x2,y2,z2, x3,y3,z3) \
    glBegin(GL_TRIANGLES);                \
    glVertex3f(x1,y1,z1);                 \
    glVertex3f(x2,y2,z2);                 \
    glVertex3f(x3,y3,z3);                 \
    glEnd();


// ── Helper: Desenhar Caixas com Espessura ─────────────────────
static void desenhaCaixa(float cx, float cy, float cz, float sx, float sy, float sz) {
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glScalef(sx, sy, sz);
    glBegin(GL_QUADS);
    // Frente e Trás
    glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
    // Cima e Baixo
    glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
    glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
    // Direita e Esquerda
    glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f,-0.5f,-0.5f);
    glEnd();
    glPopMatrix();
}

// ── Forro do Teto ───────────────────────────────────
static void desenhaInterior() {
    // Forro / Teto reto ampliado
    // Usamos (W + OV) e (D + OV) para que ele encontre a borda do telhado
    glColor3f(0.78f, 0.52f, 0.08f); 
    QUAD(
        -W - OV, H,  D + OV,  // Canto frontal esquerdo (limite do beiral)
         W + OV, H,  D + OV,  // Canto frontal direito
         W + OV, H, -D - OV,  // Canto traseiro direito
        -W - OV, H, -D - OV   // Canto traseiro esquerdo
    );
}

// ── Paredes (Com espessura) ───────────────────────────────────
static void desenhaParedes() {
    // --- PAREDES EXTERNAS ---
    glColor3f(0.95f, 0.93f, 0.88f); 
    QUAD(-W,   0, D,   -0.5f, 0, D,   -0.5f, H, D,   -W,   H, D)     // Frontal esq
    QUAD( 0.5f,0, D,    W,    0, D,    W,    H, D,    0.5f, H, D)    // Frontal dir
    QUAD(-0.5f,2.2f,D,  0.5f,2.2f,D,  0.5f,H,D,  -0.5f,H,D)          // Acima da porta
    QUAD( W, 0,-D,  -W, 0,-D,  -W, H,-D,   W, H,-D)                  // Traseira
    QUAD(-W, 0,-D,  -W, 0, D,  -W, H, D,  -W, H,-D)                  // Lateral esq
    QUAD( W, 0, D,   W, 0,-D,   W, H,-D,   W, H, D)                  // Lateral dir

    // --- PAREDES INTERNAS (Recuadas pela espessura WT) ---
    glColor3f(0.85f, 0.82f, 0.77f); // Levemente mais escuro para sombreamento interno
    QUAD(-W+WT, 0, D-WT,  -0.5f, 0, D-WT,  -0.5f, H, D-WT,  -W+WT, H, D-WT) 
    QUAD( 0.5f, 0, D-WT,   W-WT, 0, D-WT,   W-WT, H, D-WT,   0.5f, H, D-WT) 
    QUAD(-0.5f, 2.2f, D-WT,  0.5f, 2.2f, D-WT,  0.5f, H, D-WT,  -0.5f, H, D-WT) 
    QUAD( W-WT, 0, -D+WT,  -W+WT, 0, -D+WT,  -W+WT, H, -D+WT,   W-WT, H, -D+WT) 
    QUAD(-W+WT, 0, -D+WT,  -W+WT, 0, D-WT,  -W+WT, H, D-WT,  -W+WT, H, -D+WT) 
    QUAD( W-WT, 0, D-WT,   W-WT, 0, -D+WT,   W-WT, H, -D+WT,   W-WT, H, D-WT) 

    // --- BATENTE DA PORTA (Conecta o lado externo e interno) ---
    glColor3f(0.80f, 0.78f, 0.73f);
    QUAD(-0.5f, 0, D,  -0.5f, 0, D-WT,  -0.5f, 2.2f, D-WT,  -0.5f, 2.2f, D) // Batente Esq
    QUAD( 0.5f, 0, D-WT,  0.5f, 0, D,  0.5f, 2.2f, D,  0.5f, 2.2f, D-WT)    // Batente Dir
    QUAD(-0.5f, 2.2f, D,  -0.5f, 2.2f, D-WT,  0.5f, 2.2f, D-WT,  0.5f, 2.2f, D) // Batente Cima
}

// ── Porta (Com volume) ────────────────────────────────────────
static void desenhaPorta() {
    float zF = D - 0.05f;  // Face frontal
    float zB = D - 0.15f;  // Face traseira
    
    // Corpo da porta (Frente e Trás)
    glColor3f(0.82f, 0.22f, 0.12f);
    QUAD(-0.5f, 0, zF,  0.5f, 0, zF,  0.5f, 2.2f, zF, -0.5f, 2.2f, zF)
    QUAD(-0.5f, 0, zB,  0.5f, 0, zB,  0.5f, 2.2f, zB, -0.5f, 2.2f, zB)
    
    // Laterais da porta
    glColor3f(0.62f, 0.16f, 0.08f);
    QUAD(-0.5f, 0, zF,  -0.5f, 0, zB,  -0.5f, 2.2f, zB,  -0.5f, 2.2f, zF)
    QUAD( 0.5f, 0, zB,   0.5f, 0, zF,   0.5f, 2.2f, zF,   0.5f, 2.2f, zB)

    // Arco superior (agora com espessura)
    float ar = 0.5f, ay = 2.2f;
    glColor3f(0.72f, 0.18f, 0.10f);
    
    glBegin(GL_TRIANGLE_FAN); // Face frontal do arco
        glVertex3f(0, ay, zF);
        for (int i = 0; i <= 6; i++) {
            float ang = 3.14159f * i / 6.0f;
            glVertex3f(-ar * cos(ang), ay + ar * sin(ang), zF);
        }
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN); // Face traseira do arco
        glVertex3f(0, ay, zB);
        for (int i = 0; i <= 6; i++) {
            float ang = 3.14159f * i / 6.0f;
            glVertex3f(-ar * cos(ang), ay + ar * sin(ang), zB);
        }
    glEnd();
    
    glBegin(GL_QUAD_STRIP); // Borda do arco (espessura)
        for (int i = 0; i <= 6; i++) {
            float ang = 3.14159f * i / 6.0f;
            float px = -ar * cos(ang);
            float py = ay + ar * sin(ang);
            glVertex3f(px, py, zF);
            glVertex3f(px, py, zB);
        }
    glEnd();

    // Janela dupla (frente e trás)
    glColor3f(0.08f, 0.08f, 0.12f);
    QUAD(-0.22f, 1.55f, zF+0.01f,  0.22f, 1.55f, zF+0.01f,  0.22f, 2.05f, zF+0.01f, -0.22f, 2.05f, zF+0.01f) 
    QUAD(-0.22f, 1.55f, zB-0.01f,  0.22f, 1.55f, zB-0.01f,  0.22f, 2.05f, zB-0.01f, -0.22f, 2.05f, zB-0.01f) 

    // Cruz da janela (frente)
    glColor3f(0.6f, 0.15f, 0.1f);
    QUAD(-0.22f,1.77f,zF+0.02f,  0.22f,1.77f,zF+0.02f, 0.22f,1.81f,zF+0.02f, -0.22f,1.81f,zF+0.02f)
    QUAD(-0.02f,1.55f,zF+0.02f,  0.02f,1.55f,zF+0.02f, 0.02f,2.05f,zF+0.02f, -0.02f,2.05f,zF+0.02f)
    
    // Maçanetas
    glColor3f(0.75f, 0.60f, 0.1f); 
    glPushMatrix(); glTranslatef(0.35f, 1.1f, zF+0.03f); glutSolidSphere(0.04f, 8, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(0.35f, 1.1f, zB-0.03f); glutSolidSphere(0.04f, 8, 8); glPopMatrix();
}

// ── Moldura de madeira (Vigas 3D) ─────────────────────────────
static void desenhaMoldura() {
    float bw = 0.13f;     // largura da viga
    float bDepth = 0.08f;  // profundidade da viga
    float zFront = D + 0.02f + bDepth/2.0f;
    float zBack  = -D - 0.02f - bDepth/2.0f;
    
    glColor3f(0.88f, 0.52f, 0.08f); // Madeira clara

    // --- FACE FRONTAL ---
    desenhaCaixa(-W + bw/2.0f, H/2.0f, zFront, bw, H, bDepth); // Vertical Esq
    desenhaCaixa( W - bw/2.0f, H/2.0f, zFront, bw, H, bDepth); // Vertical Dir
    desenhaCaixa(0, H - bw/2.0f, zFront, W*2.0f, bw, bDepth);   // Horizontal Topo

    // --- FACE TRASEIRA ---
    desenhaCaixa(-W + bw/2.0f, H/2.0f, zBack, bw, H, bDepth);
    desenhaCaixa( W - bw/2.0f, H/2.0f, zBack, bw, H, bDepth);
    desenhaCaixa(0, H - bw/2.0f, zBack, W*2.0f, bw, bDepth);

    // --- VIGAS LATERAIS (Esquerda: Parede em X = -W) ---
    // Recuamos 0.01f no eixo X para evitar o flickering (Z-fighting) com a parede
    float leftX = -W - 0.01f - bDepth/2.0f;
    
    // Vertical Frontal Esq (Fica na quina da frente)
    desenhaCaixa(leftX, H/2.0f, D - bw/2.0f, bDepth, H, bw);   
    // Vertical Traseira Esq (Fica na quina de trás)
    desenhaCaixa(leftX, H/2.0f, -D + bw/2.0f, bDepth, H, bw);  
    // Horizontal Topo Esq (Conecta as duas verticais)
    desenhaCaixa(leftX, H - bw/2.0f, 0, bDepth, bw, D*2.0f);   

    // --- VIGAS LATERAIS (Direita: Parede em X = W) ---
    float rightX = W + 0.01f + bDepth/2.0f;
    
    // Vertical Frontal Dir
    desenhaCaixa(rightX, H/2.0f, D - bw/2.0f, bDepth, H, bw);   
    // Vertical Traseira Dir
    desenhaCaixa(rightX, H/2.0f, -D + bw/2.0f, bDepth, H, bw);  
    // Horizontal Topo Dir
    desenhaCaixa(rightX, H - bw/2.0f, 0, bDepth, bw, D*2.0f);

    // --- VIGAS DIAGONAIS (Ajustadas para o topo da porta) ---
    // O ponto de encontro agora é Y = 2.2f (altura da porta) e X = 0
    float zF = D + 0.02f + bDepth;
    float zB = D + 0.02f;
    float doorTopY = 2.2f;

    // Diagonal Esquerda
    float d1x[4] = {-W+bw, -W+bw*2, 0+bw*0.5f, 0};
    float d1y[4] = {H-bw, H-bw, doorTopY, doorTopY};
    glBegin(GL_QUADS);
        for(int i=0;i<4;i++) glVertex3f(d1x[i], d1y[i], zF);
        for(int i=3;i>=0;i--) glVertex3f(d1x[i], d1y[i], zB);
        for(int i=0;i<4;i++) { 
            int n = (i+1)%4;
            glVertex3f(d1x[i], d1y[i], zF); glVertex3f(d1x[i], d1y[i], zB);
            glVertex3f(d1x[n], d1y[n], zB); glVertex3f(d1x[n], d1y[n], zF);
        }
    glEnd();

    // Diagonal Direita
    float d2x[4] = {W-bw*2, W-bw, 0, 0-bw*0.5f};
    float d2y[4] = {H-bw, H-bw, doorTopY, doorTopY};
    glBegin(GL_QUADS);
        for(int i=0;i<4;i++) glVertex3f(d2x[i], d2y[i], zF);
        for(int i=3;i>=0;i--) glVertex3f(d2x[i], d2y[i], zB);
        for(int i=0;i<4;i++) {
            int n = (i+1)%4;
            glVertex3f(d2x[i], d2y[i], zF); glVertex3f(d2x[i], d2y[i], zB);
            glVertex3f(d2x[n], d2y[n], zB); glVertex3f(d2x[n], d2y[n], zF);
        }
    glEnd();
}

// ── Telhado (Com Forro Duplo) ─────────────────────────────────
static void desenhaTelhado() {
    float FL[3] = {-W-OV, H,  D+OV}; 
    float FR[3] = { W+OV, H,  D+OV}; 
    float BL[3] = {-W-OV, H, -D-OV}; 
    float BR[3] = { W+OV, H, -D-OV}; 
    float RF[3] = {0, H+RH,  D};     
    float RB[3] = {0, H+RH, -D};     

    // Camada inferior para dar espessura ao beiral
    float dy = -0.1f;
    float FL_b[3] = {FL[0], FL[1]+dy, FL[2]};
    float FR_b[3] = {FR[0], FR[1]+dy, FR[2]};
    float BL_b[3] = {BL[0], BL[1]+dy, BL[2]};
    float BR_b[3] = {BR[0], BR[1]+dy, BR[2]};
    float RF_b[3] = {RF[0], RF[1]+dy, RF[2]};
    float RB_b[3] = {RB[0], RB[1]+dy, RB[2]};

    // Casca superior do telhado
    glColor3f(0.88f, 0.62f, 0.10f); 
    TRI(FL[0],FL[1],FL[2],  FR[0],FR[1],FR[2],  RF[0],RF[1],RF[2]) 
    TRI(BR[0],BR[1],BR[2],  BL[0],BL[1],BL[2],  RB[0],RB[1],RB[2]) 
    QUAD(FL[0],FL[1],FL[2],  RF[0],RF[1],RF[2],  RB[0],RB[1],RB[2],  BL[0],BL[1],BL[2]) 
    QUAD(FR[0],FR[1],FR[2],  BR[0],BR[1],BR[2],  RB[0],RB[1],RB[2],  RF[0],RF[1],RF[2]) 

    // Casca inferior (Forro de madeira embaixo do beiral)
    glColor3f(0.78f, 0.52f, 0.08f); 
    TRI(FL_b[0],FL_b[1],FL_b[2],  RF_b[0],RF_b[1],RF_b[2],  FR_b[0],FR_b[1],FR_b[2])
    TRI(BR_b[0],BR_b[1],BR_b[2],  RB_b[0],RB_b[1],RB_b[2],  BL_b[0],BL_b[1],BL_b[2])
    QUAD(FL_b[0],FL_b[1],FL_b[2], BL_b[0],BL_b[1],BL_b[2], RB_b[0],RB_b[1],RB_b[2], RF_b[0],RF_b[1],RF_b[2])
    QUAD(FR_b[0],FR_b[1],FR_b[2], RF_b[0],RF_b[1],RF_b[2], RB_b[0],RB_b[1],RB_b[2], BR_b[0],BR_b[1],BR_b[2])

    // Fechamento lateral (Fascia boards / calhas)
    glColor3f(0.68f, 0.42f, 0.06f);
    QUAD(FL[0],FL[1],FL[2],  FL_b[0],FL_b[1],FL_b[2],  RF_b[0],RF_b[1],RF_b[2],  RF[0],RF[1],RF[2]) 
    QUAD(FR[0],FR[1],FR[2],  RF[0],RF[1],RF[2],  RF_b[0],RF_b[1],RF_b[2],  FR_b[0],FR_b[1],FR_b[2]) 
    QUAD(BL[0],BL[1],BL[2],  RB[0],RB[1],RB[2],  RB_b[0],RB_b[1],RB_b[2],  BL_b[0],BL_b[1],BL_b[2]) 
    QUAD(BR[0],BR[1],BR[2],  BR_b[0],BR_b[1],BR_b[2],  RB_b[0],RB_b[1],RB_b[2],  RB[0],RB[1],RB[2]) 
    QUAD(FL[0],FL[1],FL[2],  BL[0],BL[1],BL[2],  BL_b[0],BL_b[1],BL_b[2],  FL_b[0],FL_b[1],FL_b[2]) 
    QUAD(FR[0],FR[1],FR[2],  FR_b[0],FR_b[1],FR_b[2],  BR_b[0],BR_b[1],BR_b[2],  BR[0],BR[1],BR[2]) 

    // Cumeeira
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
    float t = (cx) / (W+OV);       
    float cbase = H + RH * (1-t) - 0.1f;
    float ctop  = cbase + 1.1f;

    glColor3f(0.48f, 0.48f, 0.52f);
    QUAD(cx-cw, cbase, cz+cw,  cx+cw, cbase, cz+cw,  cx+cw, ctop, cz+cw,  cx-cw, ctop, cz+cw)
    QUAD(cx+cw, cbase, cz-cw,  cx-cw, cbase, cz-cw,  cx-cw, ctop, cz-cw,  cx+cw, ctop, cz-cw)
    QUAD(cx-cw, cbase, cz-cw,  cx-cw, cbase, cz+cw,  cx-cw, ctop, cz+cw,  cx-cw, ctop, cz-cw)
    QUAD(cx+cw, cbase, cz+cw,  cx+cw, cbase, cz-cw,  cx+cw, ctop, cz-cw,  cx+cw, ctop, cz+cw)
    
    glColor3f(0.38f, 0.38f, 0.42f);
    QUAD(cx-cw, ctop, cz-cw,  cx-cw, ctop, cz+cw,  cx+cw, ctop, cz+cw,  cx+cw, ctop, cz-cw)
}

// ── Janela Decorativa ──────────────────────────────
static void desenhaJanelaDir() {
    float jx = W + 0.01f; 
    float frameW = 0.1f; // Largura da moldura da janela

    // Moldura de contorno (Mesmo material da cruz: marrom escuro)
    glColor3f(0.6f, 0.15f, 0.1f);
    // Superior e Inferior
    QUAD(jx, 2.0f, 0.6f+frameW, jx, 2.0f, -0.6f-frameW, jx, 2.0f+frameW, -0.6f-frameW, jx, 2.0f+frameW, 0.6f+frameW)
    QUAD(jx, 1.0f-frameW, 0.6f+frameW, jx, 1.0f-frameW, -0.6f-frameW, jx, 1.0f, -0.6f-frameW, jx, 1.0f, 0.6f+frameW)
    // Laterais da moldura
    QUAD(jx, 1.0f-frameW, 0.6f, jx, 1.0f-frameW, 0.6f+frameW, jx, 2.0f+frameW, 0.6f+frameW, jx, 2.0f+frameW, 0.6f)
    QUAD(jx, 1.0f-frameW, -0.6f-frameW, jx, 1.0f-frameW, -0.6f, jx, 2.0f+frameW, -0.6f, jx, 2.0f+frameW, -0.6f-frameW)

    // Vidro
    glColor3f(0.1f, 0.15f, 0.25f);
    QUAD(jx, 1.0f, 0.6f, jx, 1.0f, -0.6f, jx, 2.0f, -0.6f, jx, 2.0f, 0.6f)
    
    // Cruz da janela
    glColor3f(0.6f, 0.15f, 0.1f);
    QUAD(jx+0.01f, 1.45f, 0.6f, jx+0.01f, 1.45f, -0.6f, jx+0.01f, 1.55f, -0.6f, jx+0.01f, 1.55f, 0.6f)
    QUAD(jx+0.01f, 1.0f, 0.05f, jx+0.01f, 1.0f, -0.05f, jx+0.01f, 2.0f, -0.05f, jx+0.01f, 2.0f, 0.05f)
}

// ── Funções Públicas ──────────────────────────────────────────
void criarCasa() {
    desenhaInterior();
    desenhaParedes();
    desenhaPorta();
    desenhaJanelaDir();
    desenhaMoldura();
    desenhaTelhado();
    desenhaChamine();
}

void desenharCasa(float x, float y, float z, float rotacao) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotacao, 0.0f, 1.0f, 0.0f);
    criarCasa();
    glPopMatrix();
}