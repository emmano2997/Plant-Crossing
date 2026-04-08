#include "../include/arvore.h"
#include "../include/textura.h"
#include <math.h>
#include <stdio.h>

#define M_PI 3.14159265358979323846

int estacaoAtual;
int regouEssaEstacao;

float growthScale = 0.1f;
float windOffset  = 0.0f;
float bounceY     = 0.0f;
int   estagioPlantas = 0;

static float rotAngle = 0.0f; // controla vento 

void arvore_init() {

    // estado inicial: brotinho no verao, ainda nao regou
    estagioPlantas   = 0;
    estacaoAtual     = 0;
    regouEssaEstacao = 0;
    growthScale      = 0.01f;
}

// ─── Update  ─────────────────────────────
void arvore_update() {
    if (growthScale < 1.0f) { // tamanho da arvore
        growthScale += 0.004f;
        if (growthScale > 0.85f && growthScale < 1.0f) {
            bounceY = 0.15f * sin((growthScale - 0.85f) * M_PI / 0.15f);
        }
    } else {
        growthScale = 1.0f;
        bounceY     = 0.0f;
    }
    rotAngle  += 0.008f;
    windOffset = 1.2f * sin(rotAngle * 2.0f);
}

// ─── Primitivas ─────────────────────────

static void desenhaCilindro(float raioBase, float raioTopo, float altura, int fatias) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(q, raioBase, raioTopo, altura, fatias, 8);
    glPopMatrix();
    gluDeleteQuadric(q);
}

static void desenhaLosango(float raio, float alturaTotal) {
    float meio = alturaTotal * 0.45f;
    int fatias = 4;

    for (int i = 0; i < fatias; i++) {
        float a0 = (2.0f * M_PI * i)       / fatias;
        float a1 = (2.0f * M_PI * (i + 1)) / fatias;
        float x0 = raio * cos(a0), z0 = raio * sin(a0);
        float x1 = raio * cos(a1), z1 = raio * sin(a1);
        glBegin(GL_TRIANGLES);
            glNormal3f((x0+x1)*0.5f, raio*0.5f, (z0+z1)*0.5f);
            glVertex3f(0.0f, alturaTotal, 0.0f);
            glVertex3f(x0,  meio, z0);
            glVertex3f(x1,  meio, z1);
        glEnd();
    }
    for (int i = 0; i < fatias; i++) {
        float a0 = (2.0f * M_PI * i)       / fatias;
        float a1 = (2.0f * M_PI * (i + 1)) / fatias;
        float x0 = raio * cos(a0), z0 = raio * sin(a0);
        float x1 = raio * cos(a1), z1 = raio * sin(a1);
        glBegin(GL_TRIANGLES);
            glNormal3f((x0+x1)*0.5f, -raio*0.5f, (z0+z1)*0.5f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(x1,  meio, z1);
            glVertex3f(x0,  meio, z0);
        glEnd();
    }
}

static void desenhafruta(float raio) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 0.0f, 0.0f);
    GLUquadric* q = gluNewQuadric();
    gluSphere(q, raio, 5, 5);

    glColor3f(0.2f, 0.5f, 0.1f);
    glPushMatrix(); // caule da fruta
        glTranslatef(0.0f, raio * 1.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(q, raio * 0.15f, raio * 0.12f, raio * 0.4f, 5, 1);
    glPopMatrix();
    gluDeleteQuadric(q);
}
static void desenhaFlor(float raio) {
    glDisable(GL_TEXTURE_2D);

    GLUquadric* q = gluNewQuadric();

    glColor3f(1.0f, 0.8f, 0.0f); 
    gluSphere(q, raio * 0.3f, 6, 6); 

    int numPetalas = 5;

    for (int i = 0; i < numPetalas; i++) {
        float ang = (2.0f * M_PI * i) / numPetalas;

        glPushMatrix();

        // posição ao redor
        float x = cos(ang) * (raio * 0.6f);
        float z = sin(ang) * (raio * 0.6f);

        glTranslatef(x, 0.0f, z);

        // leve inclinação
        glRotatef(-20, 1.0f, 0.0f, 0.0f);

        // girar cada pétala para fora
        glRotatef(-ang * 180.0f / 3.14159f, 0.0f, 1.0f, 0.0f);

        glColor3f(1.0f, 0.4f, 0.7f); // cor da flor rosa
        glScalef(0.8f, 0.2f, 1.2f);
        gluSphere(q, raio * 0.4f, 6, 6); 

        glPopMatrix();
    }

    gluDeleteQuadric(q);
}
static void desenhaNeve(float raio) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.89f, 0.89f, 1.0f); // neve
    GLUquadric* q = gluNewQuadric();
    gluSphere(q, raio, 3, 1);
    gluDeleteQuadric(q);
}

// ─── copa ─────────────────────────
static void desenhaCopa(float raioBase, float alturaBase, int comFruta, int estacao) {
    glDisable(GL_TEXTURE_2D);

    // copa por Estação
    switch (estacao) {
        case 0 : // Verão: Verde 
            glColor3f(0.18f, 0.62f, 0.18f); 
            break;
        case 1: // Outono: Laranja
            glColor3f(0.70f, 0.30f, 0.20f); 
            break;
        case 2: // Inverno: Copa Pálida 
            glColor3f(0.70f, 0.75f, 0.80f); 
            break;
        case 3: // Primavera: Verde 
            glColor3f(0.30f, 0.85f, 0.30f); 
            break;
    }
    // Camada 1 (Base)
    glPushMatrix();
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        desenhaLosango(raioBase, alturaBase);
    glPopMatrix();

    // Camada 2 (Meio)
    if(estacao == 0) glColor3f(0.25f, 0.75f, 0.20f);
    else if(estacao == 1) glColor3f(0.80f, 0.40f, 0.10f); 
    else if(estacao == 2) glColor3f(0.75f, 0.80f, 0.85f); 
    else if (estacao == 3) glColor3f(0.40f, 0.85f, 0.40f);
    
    glPushMatrix();
        glTranslatef(0.0f, alturaBase * 0.50f, 0.0f);
        desenhaLosango(raioBase * 0.75f, alturaBase * 0.82f);
    glPopMatrix();

    // Camada 3 (Topo)
    if(estacao == 0) glColor3f(0.35f, 0.88f, 0.28f);
    else if(estacao == 1) glColor3f(0.85f, 0.45f, 0.15f); 
    else if(estacao == 2) glColor3f(0.80f, 0.85f, 0.90f); 

    glPushMatrix();
        glTranslatef(0.0f, alturaBase * 0.95f, 0.0f);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        desenhaLosango(raioBase * 0.50f, alturaBase * 0.62f);
    glPopMatrix();

    // Extras
    if (estacao == 2) {
        // No inverno, adicionamos neve extra
        glPushMatrix();
            glTranslatef(0.0f, alturaBase * 1.5f, 0.0f);
            desenhaNeve(raioBase * 0.25f);
        glPopMatrix();
    }

    float raioExtra = raioBase * 0.16f;
    for (int i = 0; i < 5; i++) {
        float angulo = (2.0f * M_PI * i) / 4;
        float dist   = raioBase * 0.65f;
        float px = dist * cos(angulo);
        float pz = dist * sin(angulo);
        float py = alturaBase * 0.60f;

        glPushMatrix();
            glTranslatef(px, py, pz);
            if (estacao == 0 && comFruta) {
                desenhafruta(raioExtra);
            } else if (estacao == 3) {
                desenhaFlor(raioExtra * 0.5f);
            } else if (estacao == 2) {
                desenhaNeve(raioExtra * 0.3f); 
            }
        glPopMatrix();
    }
}

// ─── Tronco vivo  ────────────────────
static void desenhaTronco(float altura, float raio, int nivel, int comFruta) {
    if (nivel < 0) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texTronco);
    glColor3f(0.72f, 0.50f, 0.28f);
    desenhaCilindro(raio, raio * 0.72f, altura, 8);
    glTranslatef(0.0f, altura, 0.0f);

    if (nivel == 0) {
        glDisable(GL_TEXTURE_2D);
        desenhaCopa(altura * 1.5f, altura * 1.5f, comFruta, estacaoAtual);
        return;
    }

    glPushMatrix();
        glRotatef(28.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        desenhaTronco(altura * 0.70f, raio * 0.68f, nivel - 1, comFruta);
    glPopMatrix();

    glPushMatrix();
        glRotatef(-28.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
        desenhaTronco(altura * 0.70f, raio * 0.68f, nivel - 1, comFruta);
    glPopMatrix();

    glPushMatrix();
        glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        desenhaTronco(altura * 0.65f, raio * 0.65f, nivel - 1, comFruta);
    glPopMatrix();

    glPushMatrix();
        glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        desenhaTronco(altura * 0.65f, raio * 0.65f, nivel - 1, comFruta);
    glPopMatrix();
}

// ─── Tronco morto  
static void desenhaTroncoMorto(float altura, float raio, int nivel) {
    if (nivel < 0) return;

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.38f, 0.30f, 0.22f);
    desenhaCilindro(raio, raio * 0.72f, altura, 8);
    glTranslatef(0.0f, altura, 0.0f);

    if (nivel == 0) return;

    glPushMatrix();
        glRotatef( 35.0f, 0.0f, 1.0f, 1.0f);
        glRotatef( 45.0f, 0.0f, 1.0f, 0.0f);
        desenhaTroncoMorto(altura * 0.60f, raio * 0.65f, nivel - 1);
    glPopMatrix();
    glPushMatrix();
        glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
        desenhaTroncoMorto(altura * 0.60f, raio * 0.65f, nivel - 1);
    glPopMatrix();
}

// ─── arvore_regar 
// Chamada quando o jogador interage com a planta para regar.
// So pode regar se a planta nao estiver morta.
void arvore_regar() {
    if (estagioPlantas == 4) return; // morta nao pode ser regada
    regouEssaEstacao = 1;
    printf("[Arvore] Regada! estacao=%d estagio=%d\n", estacaoAtual, estagioPlantas);
}

// ─── arvore_dormir 
// Chamada quando o jogador interage com a cama.
// Logica:
//   - Se regou: estagio avanca (max 3=madura)
//   - Se nao regou: estagio regride; se ja era 0 (brotinho) morre
//   - Sempre: avanca a estacao e reseta a flag de rega
// Retorna 1 se a planta morreu, 0 caso contrario.
int arvore_dormir() {
    int morreu = 0;

    if (estagioPlantas == 4) {
        // ja esta morta, so avanca estacao
        estacaoAtual = (estacaoAtual + 1) % 4;
        regouEssaEstacao = 0;
        return 0;
    }

    if (regouEssaEstacao) {
        // avanca estagio (teto: 3 = madura com fruta)
        if (estagioPlantas < 3) {
            estagioPlantas++;
        }
        // reinicia animacao de crescimento para o novo estagio
        growthScale = 0.01f;
        bounceY     = 0.0f;
    } else {
        // nao regou: regride ou morre
        if (estagioPlantas == 0) {
            estagioPlantas = 4; // brotinho sem rega = morte
            morreu = 1;
        } else {
            estagioPlantas--;
            growthScale = 0.01f;
            bounceY     = 0.0f;
        }
    }

    // avanca estacao (loop: 0->1->2->3->0)
    estacaoAtual = (estacaoAtual + 1) % 4;

    // reseta flag de rega para a proxima estacao
    regouEssaEstacao = 0;

    printf("[Arvore] Dormiu! novo estagio=%d nova estacao=%d morreu=%d\n",
           estagioPlantas, estacaoAtual, morreu);

    return morreu;
}

// ─── desenharArvore — 
// Estágios: 0=brotinho  1=jovem  2=adulta  3=madura(+fruta)  4=morta
void desenharArvore(float x, float y, float z, float rotacao) {
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(rotacao, 0.0f, 1.0f, 0.0f);
        glScalef(growthScale, growthScale, growthScale);
        glTranslatef(0.0f, bounceY, 0.0f);
        glRotatef(windOffset, 0.0f, 0.0f, 1.0f);

        switch (estagioPlantas) {
            case 0: // brotinho: pequeno, nível 0, sem fruta
                glScalef(0.35f, 0.35f, 0.35f);
                desenhaTronco(3.0f, 0.26f, 0, 0);
                break;
            case 1: // jovem: médio, nível 1, sem fruta
                glScalef(0.60f, 0.60f, 0.60f);
                desenhaTronco(3.0f, 0.26f, 1, 0);
                break;
            case 2: // adulta: tamanho cheio, sem fruta
                desenhaTronco(3.0f, 0.26f, 2, 0);
                break;
            case 3: // madura: tamanho cheio, com fruta
                desenhaTronco(3.0f, 0.26f, 2, 1);
                break;
            case 4: // morta: tronco seco sem folhas
                desenhaTroncoMorto(3.0f, 0.26f, 2);
                break;
        }
    glPopMatrix();
}