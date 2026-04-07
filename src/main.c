#include <GL/glut.h>
#include "../include/camera.h"
#include "../include/arvore.h"
#include "../include/casa.h"    

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera_lookAt(); // ← toda lógica de câmera encapsulada aqui

    // Chão
    glColor3f(0.2f, 0.5f, 0.2f);
    glBegin(GL_QUADS);
        glVertex3f(-20.0f, 0.0f, -20.0f);
        glVertex3f(-20.0f, 0.0f,  20.0f);
        glVertex3f( 20.0f, 0.0f,  20.0f);
        glVertex3f( 20.0f, 0.0f, -20.0f);
    glEnd();

    // Árvore
    desenharArvore(10.0f, 0.0f, 0.0f, 0.0f);

    // Casa
    //            x       y     z       rotação em graus (eixo Y)
    desenharCasa(-10.0f, 0.0f, 0.0f, 90.0f);

    glutSwapBuffers();
}

void update(int value) {
    arvore_update();
    camera_processMovement();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0/600.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    arvore_init();
    camera_init();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Projeto CG - plant crossing");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(camera_keyDown);       // ← callbacks da câmera
    glutKeyboardUpFunc(camera_keyUp);
    glutMouseFunc(camera_mouseButton);
    glutMotionFunc(camera_mouseMotion); 
    
    glutTimerFunc(16, update, 0);
   

    glutMainLoop();
    return 0;
}