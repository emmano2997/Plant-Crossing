#include <GL/glut.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint texTronco, texFolha;

GLuint carregaTextura(const char* arquivo) {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Configuração de repetição e filtro
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

// Variáveis de controle
float growthScale = 0.1f; 
float rotationAngle = 0.0f; 

// a função gluCylinder segue a ordem: (obj, raioBase, raioTopo, altura, fatias, pilhas)
void desenhaCilindro(float raioBase, float raioTopo, float altura) {
    GLUquadric* obj = gluNewQuadric();
    gluQuadricTexture(obj, GL_TRUE);
    glPushMatrix();
        glRotatef(-90, 1.0, 0.0, 0.0); // Alinha com o eixo Y (para cima)
        gluCylinder(obj, raioBase, raioTopo, altura, 32, 32);
    glPopMatrix();
    gluDeleteQuadric(obj);
}

// função para a folha
void criarArvore(float altura, float raio, int nivel) {
    if (nivel < 0) {
        // --- DESENHO DAS FOLHAS --- Primitivas pratica 1 
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texFolha); 
        glColor3f(1.0f, 1.0f, 1.0f); 

        float tamanhoCopa = raio * 6.0f;
        GLUquadric* q = gluNewQuadric();
        gluQuadricTexture(q, GL_TRUE); 
        
        glPushMatrix();
            glTranslatef(0.0f, tamanhoCopa * 0.5f, 0.0f);
            gluSphere(q, tamanhoCopa, 20, 20); 
        glPopMatrix();

        gluDeleteQuadric(q);
        glDisable(GL_TEXTURE_2D); 
        return;
    }

    // --- DESENHO DO TRONCO/GALHO ---
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texTronco); 
    glColor3f(1.0f, 1.0f, 1.0f); 

    desenhaCilindro(raio, raio * 0.7f, altura);
    
    // Move para o topo para os próximos galhos
    glTranslatef(0.0f, altura, 0.0f);

    // Chamadas recursivas (elas vão reativar a textura de madeira no início da função)
    glPushMatrix();
        glRotatef(30, 1, 0, 1);
        criarArvore(altura * 0.75f, raio * 0.7f, nivel - 1);
    glPopMatrix();

    glPushMatrix();
        glRotatef(-30, -1, 0, 1);
        criarArvore(altura * 0.75f, raio * 0.7f, nivel - 1);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Câmera dinâmica (Atividade Prática: Projeção/Visualização) 
    gluLookAt(15.0 * sin(rotationAngle), 7.0, 10.0 * cos(rotationAngle), 
              0.0, 3.0, 0.0, 
              0.0, 1.0, 0.0);

    // 1. Chão (Mundo Plano)
    glColor3f(0.2f, 0.5f, 0.2f);
    glBegin(GL_QUADS);
        glVertex3f(-20.0f, 0.0f, -20.0f);
        glVertex3f(-20.0f, 0.0f,  20.0f);
        glVertex3f( 20.0f, 0.0f,  20.0f);
        glVertex3f( 20.0f, 0.0f, -20.0f);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texTronco);
    
    // chama a função da árvore
    glDisable(GL_TEXTURE_2D);

    // 2. Árvore com Crescimento
    glPushMatrix();
        // O crescimento afeta toda a estrutura recursiva
        glScalef(growthScale, growthScale, growthScale);
        criarArvore(4.0f, 0.3f, 2); // 2 niveis da arvore
    glPopMatrix();

    glutSwapBuffers();
}

void update(int value) {
    if (growthScale < 1.5f) growthScale += 0.002f;
    rotationAngle += 0.01f; 
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void init() {
    texTronco = carregaTextura("texture/log-texture-brown.jpg");
    texFolha = carregaTextura("texture/leaf-texture.jpeg");

    glEnable(GL_DEPTH_TEST); 
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // cor do ceu
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0/600.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Projeto CG - plant crossing");

    init();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    
    glutMainLoop();
    return 0;
}