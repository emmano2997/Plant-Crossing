#include <GL/glut.h>
#include <math.h>

// Variáveis de controle
float growthScale = 0.1f; 
float rotationAngle = 0.0f; 

// a função gluCylinder segue a ordem: (obj, raioBase, raioTopo, altura, fatias, pilhas)
void desenhaCilindro(float raioBase, float raioTopo, float altura) {
    GLUquadric* obj = gluNewQuadric();
    glPushMatrix();
        glRotatef(-90, 1.0, 0.0, 0.0); // Alinha com o eixo Y (para cima)
        gluCylinder(obj, raioBase, raioTopo, altura, 32, 32);
    glPopMatrix();
    gluDeleteQuadric(obj);
}

// função para a árvore
void criarArvore(float altura, float raio, int nivel) {
    if (nivel == 0) {
        // Desenha as folhas (Atividade Prática: Primitivas) 
        glColor3f(0.1f, 0.7f, 0.2f); 
        
        float tamanhoCopa = raio * 6.0f; // Aumentamos o raio para esferas grandes

        glPushMatrix();
            glTranslatef(0.0f, tamanhoCopa * 0.5f, 0.0f);
            glutSolidSphere(tamanhoCopa, 20, 20);
        glPopMatrix();
        
        return;
    }

    // Desenha o segmento atual
    glColor3f(0.4f, 0.2f, 0.0f); // Marrom
    desenhaCilindro(raio, raio * 0.7f, altura);

    // criar ramificações no topo
    glTranslatef(0.0f, altura, 0.0f);

    // Ramo 1 (Direita)
    glPushMatrix();
        glRotatef(30, 1, 0, 1);
        criarArvore(altura * 0.75f, raio * 0.7f, nivel - 1);
    glPopMatrix();

    // Ramo 2 (Esquerda)
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

    // 2. Árvore com Crescimento
    glPushMatrix();
        // O crescimento afeta toda a estrutura recursiva
        glScalef(growthScale, growthScale, growthScale);
        criarArvore(4.0f, 0.3f, 3); // 3 niveis
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