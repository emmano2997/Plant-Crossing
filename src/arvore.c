#include "../include/arvore.h"
#include <GL/glut.h>

float growthScale = 0.1f;

void arvore_update() {
    if (growthScale < 1.5f) growthScale += 0.002f;
}

void desenhaCilindro(float raioBase, float raioTopo, float altura) {
    GLUquadric* obj = gluNewQuadric();
    glPushMatrix();
    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(obj, raioBase, raioTopo, altura, 32, 32);
    glPopMatrix();
    gluDeleteQuadric(obj);
}

void criarArvore(float altura, float raio, int nivel) {
    if (nivel == 0) {
        glColor3f(0.1f, 0.7f, 0.2f);
        float tamanhoCopa = raio * 6.0f;
        glPushMatrix();
        glTranslatef(0.0f, tamanhoCopa * 0.5f, 0.0f);
        glutSolidSphere(tamanhoCopa, 20, 20);
        glPopMatrix();
        return;
    }
    glColor3f(0.4f, 0.2f, 0.0f);
    desenhaCilindro(raio, raio * 0.7f, altura);
    glTranslatef(0.0f, altura, 0.0f);

    glPushMatrix();
    glRotatef(30, 1, 0, 1);
    criarArvore(altura * 0.75f, raio * 0.7f, nivel - 1);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-30, -1, 0, 1);
    criarArvore(altura * 0.75f, raio * 0.7f, nivel - 1);
    glPopMatrix();
}

void desenharArvore(float x, float y, float z, float rotacao) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotacao, 0.0f, 1.0f, 0.0f);
    glScalef(growthScale, growthScale, growthScale);
    criarArvore(4.0f, 0.3f, 3);
    glPopMatrix();
}