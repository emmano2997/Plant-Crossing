#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint texTronco, texFolha, texChao;

GLuint carregaTextura(const char* arquivo) {
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
        glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0, formato, GL_UNSIGNED_BYTE, dados);
        stbi_image_free(dados);
    } else {
        printf("Erro ao carregar textura: %s\n", arquivo);
    }
    return texID;
}

// ─── Variáveis globais ──────────────────────────────────────────────
float growthScale   = 0.01f;
float rotationAngle = 0.0f;
float windOffset    = 0.0f;   // balanço do vento
float bounceY       = 0.0f;   // "bounce" de crescimento da arvore

// ─── Primitivas ─────────────────────────────────────────────────────

void desenhaCilindro(float raioBase, float raioTopo, float altura, int fatias) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(q, raioBase, raioTopo, altura, fatias, 8);
    glPopMatrix();
    gluDeleteQuadric(q);
}

// Losango 3D low-poly (formato de diamante, 4 faces)
void desenhaLosango(float raio, float alturaTotal) {
    float meio = alturaTotal * 0.45f; // ponto mais largo ligeiramente abaixo do centro
    int fatias = 4; // 4 lados = losango

    // Metade superior (centro → topo)
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
    // Metade inferior (centro → base)
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

// ─── (fruto) ────────────────────────────────────────────────
void desenhafruta(float raio) {
    // corpo da fruta
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 0.0f, 0.0f);
    GLUquadric* q = gluNewQuadric();
    gluSphere(q, raio, 8, 8);   // 8 fatias 

    // pequeno cabo verde no topo
    glColor3f(0.2f, 0.5f, 0.1f);
    glPushMatrix();
        glTranslatef(0.0f, raio * 1.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(q, raio * 0.15f, raio * 0.12f, raio * 0.4f, 5, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
}

// ─── Copa da árvore (camadas de losangos + frutas) ───────────────────
void desenhaCopa(float raioBase, float alturaBase) {
    glDisable(GL_TEXTURE_2D); // losango usa cor sólida flat

    // Camada 1 – base (maior, mais escura), rotacionada 45° para variar faces
    glColor3f(0.18f, 0.62f, 0.18f);
    glPushMatrix();
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        desenhaLosango(raioBase, alturaBase);
    glPopMatrix();

    // Camada 2 – meio
    glColor3f(0.25f, 0.75f, 0.20f);
    glPushMatrix();
        glTranslatef(0.0f, alturaBase * 0.50f, 0.0f);
        desenhaLosango(raioBase * 0.75f, alturaBase * 0.82f);
    glPopMatrix();

    // Camada 3 – topo (menor, mais clara)
    glColor3f(0.35f, 0.88f, 0.28f);
    glPushMatrix();
        glTranslatef(0.0f, alturaBase * 0.95f, 0.0f);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        desenhaLosango(raioBase * 0.50f, alturaBase * 0.62f);
    glPopMatrix();

    // ── frutas na copa ──────────────────────
    float raiofruta = raioBase * 0.18f;
        for (int i = 0; i < 3; i++) {
        float angulo = (2.0f * M_PI * i) / 3;
        float dist   = raioBase * 0.65f;
        float px     = dist * cos(angulo);
        float pz     = dist * sin(angulo);
        float py     = alturaBase * 0.40f;
        glPushMatrix();
            glTranslatef(px, py, pz);
            desenhafruta(raiofruta);
        glPopMatrix();
    }
    // fruta no topo
    glPushMatrix();
        glTranslatef(0.0f, alturaBase * 1.55f, 0.0f);
        desenhafruta(raiofruta * 0.85f);
    glPopMatrix();
}

// ─── Tronco principal + galhos ──────────────────────────────────────
void desenhaTronco(float altura, float raio, int nivel) {
    if (nivel < 0) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texTronco);
    glColor3f(0.72f, 0.50f, 0.28f);
    desenhaCilindro(raio, raio * 0.72f, altura, 8);

    glTranslatef(0.0f, altura, 0.0f);

    if (nivel == 0) {
        // folhagem no topo dos galhos terminais
        glDisable(GL_TEXTURE_2D);
        desenhaCopa(altura * 1.5f, altura * 1.5f);
        return;
    }

    // Galho esquerdo
    glPushMatrix();
        glRotatef(28.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        desenhaTronco(altura * 0.70f, raio * 0.68f, nivel - 1);
    glPopMatrix();

    // Galho direito
    glPushMatrix();
        glRotatef(-28.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
        desenhaTronco(altura * 0.70f, raio * 0.68f, nivel - 1);
    glPopMatrix();

    // Galho frontal
    glPushMatrix();
        glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        desenhaTronco(altura * 0.65f, raio * 0.65f, nivel - 1);
    glPopMatrix();

    // Galho traseiro
    glPushMatrix();
        glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        desenhaTronco(altura * 0.65f, raio * 0.65f, nivel - 1);
    glPopMatrix();
}

// ─── Chão com gramado estilo ─────────────────────────────────────
void desenhaChao() {
    // plano
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.45f, 0.78f, 0.32f);
    glBegin(GL_QUADS);
        glVertex3f(-30.0f, 0.0f, -30.0f);
        glVertex3f(-30.0f, 0.0f,  30.0f);
        glVertex3f( 30.0f, 0.0f,  30.0f);
        glVertex3f( 30.0f, 0.0f, -30.0f);
    glEnd();

    // Chao estilizado
    glColor3f(0.42f, 0.74f, 0.29f);
    for (int i = -5; i <= 5; i++) {
        glBegin(GL_QUADS);
            glVertex3f(i * 4.0f - 0.6f, 0.01f, -30.0f);
            glVertex3f(i * 4.0f - 0.6f, 0.01f,  30.0f);
            glVertex3f(i * 4.0f + 0.6f, 0.01f,  30.0f);
            glVertex3f(i * 4.0f + 0.6f, 0.01f, -30.0f);
        glEnd();
    }

    // Sombra embaixo da árvore (círculo escuro suave)
    glColor4f(0.0f, 0.0f, 0.0f, 0.18f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int seg = 32;
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.02f, 0.0f);
        for (int i = 0; i <= seg; i++) {
            float a = 2.0f * M_PI * i / seg;
            glVertex3f(2.8f * cos(a), 0.02f, 2.8f * sin(a));
        }
    glEnd();
    glDisable(GL_BLEND);
}

// ─── Display ────────────────────────────────────────────────────────
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Câmera levemente acima e afastada
    gluLookAt(
        12.0 * sin(rotationAngle), 9.0, 12.0 * cos(rotationAngle),
        0.0, 3.5, 0.0,
        0.0, 1.0, 0.0
    );

    desenhaChao();

    // Árvore com crescimento + vento 
    glPushMatrix();
        glScalef(growthScale, growthScale, growthScale);
        glTranslatef(0.0f, bounceY, 0.0f);
        glRotatef(windOffset, 0.0f, 0.0f, 1.0f); 
        desenhaTronco(3.0f, 0.26f, 2);
    glPopMatrix();

    glutSwapBuffers();
}

// ─── Update ─────────────────────────────────────────────────────────
void update(int value) {
    if (growthScale < 1.0f) {
        growthScale += 0.004f;
        // bounce: ultrapassa ligeiramente e volta
        if (growthScale > 0.85f && growthScale < 1.0f) {
            bounceY = 0.15f * sin((growthScale - 0.85f) * M_PI / 0.15f);
        }
    } else {
        growthScale = 1.0f;
        bounceY = 0.0f;
    }

    rotationAngle += 0.008f;

    // Balanço de vento suave
    windOffset = 1.2f * sin(rotationAngle * 2.0f);

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ─── Init ────────────────────────────────────────────────────────────
void init() {
    texTronco = carregaTextura("texture/log-texture-brown.jpg");
    texFolha  = carregaTextura("texture/leaf-texture.jpeg");

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // Céu azul claro estilo AC
    glClearColor(0.53f, 0.85f, 0.98f, 1.0f);

    // Iluminação simples para dar volume low-poly
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat luzPos[]     = { 5.0f, 10.0f, 5.0f, 1.0f };
    GLfloat luzDifusa[]  = { 1.0f,  0.95f, 0.85f, 1.0f };
    GLfloat luzAmbiente[]= { 0.45f, 0.50f, 0.45f, 1.0f };
    GLfloat luzEspecular[]={ 0.5f,  0.5f,  0.5f,  1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, luzPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  luzDifusa);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  luzAmbiente);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

// ─── Main ────────────────────────────────────────────────────────────
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Plant Crossing - Animal Crossing Tree");

    init();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}