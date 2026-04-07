#include "../include/mundo.h"
#include <GL/glut.h>
#include <math.h>

#define M_PI 3.14159265358979323846

GLuint texMontanha;

int verificaColisao(float proxX, float proxZ) {
    // posicao das cercas
    if (proxX > 29.0f || proxX < -29.0f) return 1;
    if (proxZ > 29.0f || proxZ < -29.0f) return 1;
    
    return 0; 
}

void desenhaChao(int estacao) {
    // plano
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.45f, 0.78f, 0.32f);
    glBegin(GL_QUADS);
        glVertex3f(-30.0f, 0.0f, -30.0f);
        glVertex3f(-30.0f, 0.0f,  30.0f);
        glVertex3f( 30.0f, 0.0f,  30.0f);
        glVertex3f( 30.0f, 0.0f, -30.0f);
    glEnd();

    // Chao 
    glColor3f(0.42f, 0.74f, 0.29f);
    for (int i = -5; i <= 5; i++) {
        glBegin(GL_QUADS);
            glVertex3f(i * 4.0f - 0.6f, 0.01f, -30.0f);
            glVertex3f(i * 4.0f - 0.6f, 0.01f,  30.0f);
            glVertex3f(i * 4.0f + 0.6f, 0.01f,  30.0f);
            glVertex3f(i * 4.0f + 0.6f, 0.01f, -30.0f);
        glEnd();
    }
}
void desenhaCaminho(int estacao) {
    if (estacao == 2) glColor3f(0.8f, 0.8f, 0.9f); // Caminho com neve
    else glColor3f(0.6f, 0.5f, 0.4f); // Terra 

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -1.0f); 

    glBegin(GL_QUADS);
        glVertex3f(-17.0f, 0.02f, 0.0f); // Casa
        glVertex3f( 17.0f, 0.02f, 0.0f); // árvore
        glVertex3f( 17.0f, 0.02f, 2.0f);
        glVertex3f(-17.0f, 0.02f, 2.0f);
    glEnd();
    glPopMatrix();
}

void desenhaFolhasNoChao(int estacao) {
    if (estacao != 1) return;

    glColor3f(0.7f, 0.3f, 0.0f); // Folhas laranjas
    for (int i = 0; i < 15; i++) {
        glPushMatrix();
            glTranslatef(11.5f + (i % 5), 0.02f, -2.0f + (i / 3));
            glRotatef(i * 30, 0, 1, 0);
            glScalef(0.2f, 0.01f, 0.3f);
            glutSolidSphere(1.0f, 5, 5);
        glPopMatrix();
    }
}
void desenhaCercaUnidade() {
    // Poste vertical
    glColor3f(0.5f, 0.35f, 0.05f);
    glPushMatrix();
        glTranslatef(0, 0.5f, 0);
        glScalef(0.1f, 1.0f, 0.1f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Ripas horizontais
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

void desenhaNeveCaindo() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 100; i++) {
        // Usa valores fixos baseados em i para simular partículas
        float x = (float)(i % 20) - 10.0f;
        float z = (float)(i / 5) - 10.0f;
        float y = 5.0f + sin(i); 
        glVertex3f(x * 2.0f, y, z * 2.0f);
    }
    glEnd();
}

void mundo_desenhar(int estacao) {
    desenhaChao(estacao);
    desenhaCaminho(estacao);
    desenhaFolhasNoChao(estacao);
    //desenhaMontanha(-32.0f, 0.0f, 80.0f, 40.0f);

    for (float i = -30.0f; i < 30.0f; i += 1.0f) { // esquerda casa 
        glPushMatrix();
            glTranslatef(i, 0, 29.8f);
            desenhaCercaUnidade();
        glPopMatrix();
    }   
    for (float i = -29.0f; i < 31.0f; i += 1.0f) { // traz casa
        glPushMatrix();
            glTranslatef(-29.8f, 0.0f, i); 
            glRotatef(90, 0, 1, 0);       
            desenhaCercaUnidade();
        glPopMatrix();
    }    for (float i = -30.0f; i < 30.0f; i += 1.0f) { // direira casa 
        glPushMatrix();
            glTranslatef(i, 0, -29.8f);
            desenhaCercaUnidade();
        glPopMatrix();
    }   
    for (float i = -29.5f; i < 31.0f; i += 1.0f) { //frente casa
        glPushMatrix();
            glTranslatef(29.8f, 0.0f, i); 
            glRotatef(90, 0, 1, 0);      
            desenhaCercaUnidade();
        glPopMatrix();
    }
    if (estacao == 2) { // inverno
        desenhaNeveCaindo();
    }    
}