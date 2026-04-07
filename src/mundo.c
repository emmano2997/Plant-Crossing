#include "../include/mundo.h"
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define M_PI    3.14159265358979323846
#define N_PART  120

// ─── Estrutura de particula ───────────────────────────────────────────
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float vida;  // 1.0=nova  0.0=morta
    float fase;  // offset individual para o balanco
} Particula;

static Particula partsNeve[N_PART];
static Particula partsFolha[N_PART];
static Particula partsPetala[N_PART];

// vento global 
static float ventoCiclo = 0.0f;
static float ventoX     = 0.0f;
static float ventoZ     = 0.0f;

// ─── Utilitarios 
static float randf(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

static void resetaParticula(Particula* p, float alturaMax) {
    p->x    = randf(-28.0f, 28.0f);
    p->y    = randf(alturaMax * 0.5f, alturaMax);
    p->z    = randf(-28.0f, 28.0f);
    p->vx   = randf(-0.02f,  0.02f);
    p->vy   = randf(-0.05f, -0.02f);
    p->vz   = randf(-0.02f,  0.02f);
    p->vida = 1.0f;
    p->fase = randf(0.0f, 2.0f * (float)M_PI);
}

static int inicializado = 0;
static void inicializaParticulas() {
    if (inicializado) return;
    for (int i = 0; i < N_PART; i++) {
        resetaParticula(&partsNeve[i],    10.0f);
        resetaParticula(&partsFolha[i],    7.0f);
        resetaParticula(&partsPetala[i],   7.0f);
        // distribui verticalmente para nao comecar tudo junto
        partsNeve[i].y   = randf(0.0f, 10.0f);
        partsFolha[i].y  = randf(0.0f,  7.0f);
        partsPetala[i].y = randf(0.0f,  7.0f);
    }
    inicializado = 1;
}
void mundo_update(int estacao) {
    inicializaParticulas();

    // vento oscilante igual ao da arvore
    ventoCiclo += 0.012f;
    ventoX = 0.025f * sinf(ventoCiclo);
    ventoZ = 0.015f * sinf(ventoCiclo * 0.7f);

    if (estacao == 2) {
        // neve: cai quase reto com leve balanco
        for (int i = 0; i < N_PART; i++) {
            Particula* p = &partsNeve[i];
            p->x += p->vx + ventoX + 0.008f * sinf(ventoCiclo + p->fase);
            p->y += p->vy;
            p->z += p->vz + ventoZ;
            p->vida -= 0.004f;
            if (p->vida <= 0.0f || p->y < 0.0f)
                resetaParticula(p, 10.0f);
        }
    } else if (estacao == 1) {
        // folhas outono: mais pesadas, espiral com vento
        for (int i = 0; i < N_PART; i++) {
            Particula* p = &partsFolha[i];
            p->x += p->vx + ventoX * 1.5f + 0.015f * sinf(ventoCiclo * 1.2f + p->fase);
            p->y += p->vy * 0.8f;
            p->z += p->vz + ventoZ * 1.5f;
            p->vida -= 0.003f;
            if (p->vida <= 0.0f || p->y < 0.0f)
                resetaParticula(p, 7.0f);
        }
    } else if (estacao == 3) {
        // petalas primavera: flutuam com vento mais forte
        for (int i = 0; i < N_PART; i++) {
            Particula* p = &partsPetala[i];
            p->x += p->vx + ventoX * 2.0f + 0.02f * sinf(ventoCiclo * 0.8f + p->fase);
            p->y += p->vy * 0.5f + 0.005f * sinf(ventoCiclo * 1.5f + p->fase);
            p->z += p->vz + ventoZ * 2.0f;
            p->vida -= 0.003f;
            if (p->vida <= 0.0f || p->y < 0.0f)
                resetaParticula(p, 7.0f);
        }
    }
}

// ─── Chao por estacao 
static void desenhaChao(int estacao) {
    glDisable(GL_TEXTURE_2D);

    switch (estacao) {
        case 0: glColor3f(0.45f, 0.78f, 0.32f); break; // verao: verde
        case 1: glColor3f(0.50f, 0.62f, 0.25f); break; // outono: amarelado
        case 2: glColor3f(0.88f, 0.92f, 0.95f); break; // inverno: neve
        case 3: glColor3f(0.50f, 0.82f, 0.40f); break; // primavera: verde claro
    }
    glBegin(GL_QUADS);
        glVertex3f(-30.0f, 0.0f, -30.0f);
        glVertex3f(-30.0f, 0.0f,  30.0f);
        glVertex3f( 30.0f, 0.0f,  30.0f);
        glVertex3f( 30.0f, 0.0f, -30.0f);
    glEnd();

    // listras decorativas
    switch (estacao) {
        case 0: glColor3f(0.42f, 0.74f, 0.29f); break;
        case 1: glColor3f(0.46f, 0.58f, 0.22f); break;
        case 2: glColor3f(0.82f, 0.88f, 0.92f); break;
        case 3: glColor3f(0.46f, 0.78f, 0.36f); break;
    }
    for (int i = -5; i <= 5; i++) {
        glBegin(GL_QUADS);
            glVertex3f(i * 4.0f - 0.6f, 0.01f, -30.0f);
            glVertex3f(i * 4.0f - 0.6f, 0.01f,  30.0f);
            glVertex3f(i * 4.0f + 0.6f, 0.01f,  30.0f);
            glVertex3f(i * 4.0f + 0.6f, 0.01f, -30.0f);
        glEnd();
    }
}

static void desenhaCaminho(int estacao) {
    if (estacao == 2) glColor3f(0.80f, 0.84f, 0.90f);
    else              glColor3f(0.60f, 0.50f, 0.40f);

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -1.0f);
        glBegin(GL_QUADS);
            glVertex3f(-17.0f, 0.02f, 0.0f);
            glVertex3f( 17.0f, 0.02f, 0.0f);
            glVertex3f( 17.0f, 0.02f, 2.0f);
            glVertex3f(-17.0f, 0.02f, 2.0f);
        glEnd();
    glPopMatrix();
}

static void desenhaFolhasNoChao(int estacao) { //outono
    if (estacao != 1) return;
    glColor3f(0.70f, 0.30f, 0.00f);
    for (int i = 0; i < 15; i++) {
        glPushMatrix();
            glTranslatef(11.5f + (i % 5), 0.02f, -2.0f + (i / 3));
            glRotatef(i * 30, 0, 1, 0);
            glScalef(0.2f, 0.01f, 0.3f);
            glutSolidSphere(1.0f, 5, 5);
        glPopMatrix();
    }
}
static void desenhaCercaUnidade() {
    glColor3f(0.50f, 0.35f, 0.05f);
    glPushMatrix();
        glTranslatef(0, 0.5f, 0);
        glScalef(0.1f, 1.0f, 0.1f);
        glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.5f, 0.4f, 0);
        glScalef(1.0f, 0.15f, 0.05f);
        glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.5f, 0.8f, 0);
        glScalef(1.0f, 0.15f, 0.05f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

static void desenhaNeve() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(3.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < N_PART; i++) {
        Particula* p = &partsNeve[i];
        glColor4f(0.95f, 0.97f, 1.0f, p->vida * 0.9f);
        glVertex3f(p->x, p->y, p->z);
    }
    glEnd();
    glDisable(GL_BLEND);
    glPointSize(1.0f);
}

static void desenhaFolhasVoando() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < N_PART; i++) {
        Particula* p = &partsFolha[i];
        glPushMatrix();
            glTranslatef(p->x, p->y, p->z);
            glRotatef(ventoX * 1800.0f, 0.0f, 0.0f, 1.0f);
            glScalef(0.12f, 0.01f, 0.18f);
            glColor4f(0.80f, 0.35f + p->vida * 0.1f, 0.0f, p->vida * 0.85f);
            glutSolidSphere(1.0f, 5, 5);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
}

static void desenhaPetalas() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < N_PART; i++) {
        Particula* p = &partsPetala[i];
        glPushMatrix();
            glTranslatef(p->x, p->y, p->z);
            glRotatef(ventoX * 1200.0f, 0.0f, 1.0f, 0.0f);
            glScalef(0.10f, 0.01f, 0.14f);
            glColor4f(1.0f, 0.65f + p->vida * 0.1f, 0.80f, p->vida * 0.80f);
            glutSolidSphere(1.0f, 5, 5);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
}

void mundo_desenhar(int estacao) {
    inicializaParticulas();

    desenhaChao(estacao);
    desenhaCaminho(estacao);
    desenhaFolhasNoChao(estacao);

    // cercas
    for (float i = -30.0f; i < 30.0f; i += 1.0f) {
        glPushMatrix(); glTranslatef(i, 0, 29.8f);
            desenhaCercaUnidade();
        glPopMatrix();
    }
    for (float i = -29.0f; i < 31.0f; i += 1.0f) {
        glPushMatrix(); glTranslatef(-29.8f, 0.0f, i); glRotatef(90, 0, 1, 0);
            desenhaCercaUnidade();
        glPopMatrix();
    }
    for (float i = -30.0f; i < 30.0f; i += 1.0f) {
        glPushMatrix(); glTranslatef(i, 0, -29.8f);
            desenhaCercaUnidade();
        glPopMatrix();
    }
    for (float i = -29.5f; i < 31.0f; i += 1.0f) {
        glPushMatrix(); glTranslatef(29.8f, 0.0f, i); glRotatef(90, 0, 1, 0);
            desenhaCercaUnidade();
        glPopMatrix();
    }

    // particulas por estacao
    switch (estacao) {
        case 1: desenhaFolhasVoando(); break;
        case 2: desenhaNeve();         break;
        case 3: desenhaPetalas();      break;
    }
}
int verificaColisao(float proxX, float proxZ) {
    if (proxX > 29.0f || proxX < -29.0f) return 1;
    if (proxZ > 29.0f || proxZ < -29.0f) return 1;
    return 0;
}