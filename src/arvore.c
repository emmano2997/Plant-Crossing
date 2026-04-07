#include "../include/arvore.h"
#include <math.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define M_PI 3.14159265358979323846

GLuint texTronco;
GLuint texFolha;

int estacaoAtual;
int regouEssaEstacao;

float growthScale = 0.1f;
float windOffset  = 0.0f;
float bounceY     = 0.0f;
int   estagioPlantas = 0;

static float rotAngle = 0.0f; // controla câmera/vento internamente

// textura  
static GLuint carregaTextura(const char* arquivo) {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int largura, altura, canais;
    unsigned char* dados = stbi_load(arquivo, &largura, &altura, &canais, 0);
    if (dados) {
        GLenum formato = (canais == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0,
                     formato, GL_UNSIGNED_BYTE, dados);
        stbi_image_free(dados);
    } else {
        printf("Erro ao carregar textura: %s\n", arquivo);
    }
    return texID;
}

void arvore_init() {
    texTronco = carregaTextura("texture/log-texture-brown.jpg");
    texFolha  = carregaTextura("texture/leaf-texture.jpeg");
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
    gluSphere(q, raio, 8, 8);

    glColor3f(0.2f, 0.5f, 0.1f);
    glPushMatrix();
        glTranslatef(0.0f, raio * 1.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(q, raio * 0.15f, raio * 0.12f, raio * 0.4f, 5, 1);
    glPopMatrix();
    gluDeleteQuadric(q);
}
static void desenhaFlor(float raio) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 0.6f, 0.8f); // Rosa claro
    GLUquadric* q = gluNewQuadric();
    gluSphere(q, raio, 6, 6); // Uma esfera pequena como flor
    gluDeleteQuadric(q);
}

static void desenhaNeve(float raio) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.95f, 0.95f, 1.0f); // Branco neve
    GLUquadric* q = gluNewQuadric();
    gluSphere(q, raio, 6, 6);
    gluDeleteQuadric(q);
}

// ─── Copa  ─────────
static void desenhaCopa(float raioBase, float alturaBase, int comFruta, int estacao) {
glDisable(GL_TEXTURE_2D);
    if (estacao != 2) {
        switch (estacao) {
            case 0: glColor3f(0.18f, 0.62f, 0.18f); break; // Verão: Verde
            case 1: glColor3f(0.80f, 0.40f, 0.10f); break; // Outono: Laranja/Marrom
            case 3: glColor3f(0.40f, 0.85f, 0.40f); break; // Primavera: Verde Claro
        }

        // Camada 1
        glPushMatrix();
            glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
            desenhaLosango(raioBase, alturaBase);
        glPopMatrix();

        // Camada 2 (tom levemente diferente)
        if(estacao == 0) glColor3f(0.25f, 0.75f, 0.20f);
        glPushMatrix();
            glTranslatef(0.0f, alturaBase * 0.50f, 0.0f);
            desenhaLosango(raioBase * 0.75f, alturaBase * 0.82f);
        glPopMatrix();

        // Camada 3
        if(estacao == 0) glColor3f(0.35f, 0.88f, 0.28f);
        glPushMatrix();
            glTranslatef(0.0f, alturaBase * 0.95f, 0.0f);
            glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
            desenhaLosango(raioBase * 0.50f, alturaBase * 0.62f);
        glPopMatrix();
    } else {
        // Inverno: Apenas neve no topo
        glPushMatrix();
            glTranslatef(0.0f, 0.1f, 0.0f);
            desenhaNeve(raioBase * 0.2f);
        glPopMatrix();
    }

    // Extras: Frutas, Flores ou Neve no topo
    float raioExtra = raioBase * 0.18f;
    for (int i = 0; i < 3; i++) {
        float angulo = (2.0f * M_PI * i) / 3;
        float dist   = raioBase * 0.65f;
        float px = dist * cos(angulo);
        float pz = dist * sin(angulo);
        float py = alturaBase * 0.40f;

        glPushMatrix();
            glTranslatef(px, py, pz);
            if (estacao == 0 && comFruta) desenhafruta(raioExtra); // Verão + Estágio 3
            else if (estacao == 3) desenhaFlor(raioExtra * 0.8f);   // Primavera
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

// ─── Tronco morto  ───────────────────────────
static void desenhaTroncoMorto(float altura, float raio, int nivel) {
    if (nivel < 0) return;

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.38f, 0.30f, 0.22f);
    desenhaCilindro(raio, raio * 0.72f, altura, 8);
    glTranslatef(0.0f, altura, 0.0f);

    if (nivel == 0) return;

    glPushMatrix();
        glRotatef( 35.0f, 0.0f, 0.0f, 1.0f);
        glRotatef( 45.0f, 0.0f, 1.0f, 0.0f);
        desenhaTroncoMorto(altura * 0.60f, raio * 0.65f, nivel - 1);
    glPopMatrix();
    glPushMatrix();
        glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
        desenhaTroncoMorto(altura * 0.60f, raio * 0.65f, nivel - 1);
    glPopMatrix();
}

// ─── desenharArvore — função pública, mesma assinatura do colega ──────
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