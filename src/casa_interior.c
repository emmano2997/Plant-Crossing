#include "../include/casa_interior.h"
#include <GL/glut.h>

// Macro auxiliar para reduzir repetição
#define QUAD(x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4) \
    glBegin(GL_QUADS);                               \
    glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2);      \
    glVertex3f(x3,y3,z3); glVertex3f(x4,y4,z4);      \
    glEnd();

static void desenhaPortaInterior(float zPos) {
    float zF = zPos - 0.05f; 
    
    // Corpo da porta
    glColor3f(0.82f, 0.22f, 0.12f);
    QUAD(-1.0f, 0, zF, 1.0f, 0, zF, 1.0f, 2.5f, zF, -1.0f, 2.5f, zF)

    // Vidro da porta (Uma única janela centralizada)
    glColor3f(0.08f, 0.08f, 0.12f);
    QUAD(-0.6f, 1.5f, zF-0.01f,  0.6f, 1.5f, zF-0.01f,  0.6f, 2.2f, zF-0.01f, -0.6f, 2.2f, zF-0.01f)

    // Cruz da janela da porta
    glColor3f(0.6f, 0.15f, 0.1f);
    // Linha Horizontal (Corta de lado a lado)
    QUAD(-0.6f, 1.83f, zF-0.02f,  0.6f, 1.83f, zF-0.02f,  0.6f, 1.87f, zF-0.02f, -0.6f, 1.87f, zF-0.02f)
    // Linha Vertical (Centralizada perfeitamente em X = 0)
    QUAD(-0.02f, 1.5f, zF-0.02f,  0.02f, 1.5f, zF-0.02f,  0.02f, 2.2f, zF-0.02f, -0.02f, 2.2f, zF-0.02f) 

    // Maçaneta
    glColor3f(0.75f, 0.60f, 0.1f);
    glPushMatrix(); 
        glTranslatef(0.8f, 1.2f, zF-0.03f); 
        glutSolidSphere(0.06f, 8, 8); 
    glPopMatrix();
}

static void desenhaJanelaInteriorDir(float paredeX) {
    float jx = paredeX - 0.01f; 
    float frameW = 0.15f;

    // Moldura de contorno (Interior)
    glColor3f(0.6f, 0.15f, 0.1f);
    // Superior e Inferior
    QUAD(jx, 2.5f, 1.0f+frameW, jx, 2.5f, -1.0f-frameW, jx, 2.5f+frameW, -1.0f-frameW, jx, 2.5f+frameW, 1.0f+frameW)
    QUAD(jx, 1.2f-frameW, 1.0f+frameW, jx, 1.2f-frameW, -1.0f-frameW, jx, 1.2f, -1.0f-frameW, jx, 1.2f, 1.0f+frameW)
    // Laterais
    QUAD(jx, 1.2f-frameW, 1.0f, jx, 1.2f-frameW, 1.0f+frameW, jx, 2.5f+frameW, 1.0f+frameW, jx, 2.5f+frameW, 1.0f)
    QUAD(jx, 1.2f-frameW, -1.0f-frameW, jx, 1.2f-frameW, -1.0f, jx, 2.5f+frameW, -1.0f, jx, 2.5f+frameW, -1.0f-frameW)

    // Vidro
    glColor3f(0.1f, 0.15f, 0.25f);
    QUAD(jx, 1.2f, -1.0f, jx, 1.2f, 1.0f, jx, 2.5f, 1.0f, jx, 2.5f, -1.0f)

    // Cruz de madeira (Interior)
    glColor3f(0.6f, 0.15f, 0.1f);
    QUAD(jx-0.01f, 1.8f, -1.0f, jx-0.01f, 1.8f, 1.0f, jx-0.01f, 1.9f, 1.0f, jx-0.01f, 1.9f, -1.0f)
    QUAD(jx-0.01f, 1.2f, -0.05f, jx-0.01f, 1.2f, 0.05f, jx-0.01f, 2.5f, 0.05f, jx-0.01f, 2.5f, -0.05f)
}

void desenharInteriorExpandido(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    float tamanho = 5.0f; 
    float alturaS = 4.0f;

    // Chão e Teto
    glColor3f(0.3f, 0.2f, 0.1f); // Chão
    QUAD(-tamanho, 0, -tamanho,  tamanho, 0, -tamanho,  tamanho, 0, tamanho, -tamanho, 0, tamanho)
    glColor3f(0.8f, 0.8f, 0.8f); // Teto
    QUAD(-tamanho, alturaS, -tamanho,  tamanho, alturaS, -tamanho,  tamanho, alturaS, tamanho, -tamanho, alturaS, tamanho)

    // Paredes
    glColor3f(0.6f, 0.6f, 0.7f);
    QUAD(-tamanho, 0, -tamanho,  tamanho, 0, -tamanho,  tamanho, alturaS, -tamanho, -tamanho, alturaS, -tamanho) // Traseira
    QUAD(tamanho, 0, -tamanho,  tamanho, 0, tamanho,  tamanho, alturaS, tamanho,  tamanho, alturaS, -tamanho) // Direita
    QUAD(-tamanho, 0, tamanho, -tamanho, 0, -tamanho, -tamanho, alturaS, -tamanho, -tamanho, alturaS, tamanho) // Esquerda
    
    // Parede Frontal (com recorte da porta)
    QUAD(-tamanho, 0, tamanho, -1.0f, 0, tamanho, -1.0f, alturaS, tamanho, -tamanho, alturaS, tamanho)
    QUAD( 1.0f, 0, tamanho,  tamanho, 0, tamanho,  tamanho, alturaS, tamanho,  1.0f, alturaS, tamanho)
    QUAD(-1.0f, 2.5f, tamanho, 1.0f, 2.5f, tamanho, 1.0f, alturaS, tamanho, -1.0f, alturaS, tamanho)

    // --- Inserindo as Novas Peças ---
    desenhaPortaInterior(tamanho); // Tamanho é 5.0 (A posição Z da parede frontal)
    desenhaJanelaInteriorDir(tamanho); // Tamanho é 5.0 (A posição X da parede direita)

    glPopMatrix();
}