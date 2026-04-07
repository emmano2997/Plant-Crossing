#ifndef ARVORE_H
#define ARVORE_H

#include <GL/glut.h>

// ─── Texturas ────────────────────────────────────────────────────────
extern GLuint texTronco;
extern GLuint texFolha;

// ─── Variáveis de animação ───────────────────────────────────────────
extern float growthScale;
extern float windOffset;
extern float bounceY;

// Estagio da planta:
// 0 = brotinho   1 = jovem   2 = adulta (sem fruto)
// 3 = madura (com fruto)    4 = morta
extern int estagioPlantas;

// ─── Funções públicas ────────────────────────────────────────────────
void arvore_init();   // carrega texturas — chama no init() da main
void arvore_update(); // atualiza growthScale/bounce/vento — chama no update() da main

// mesma assinatura que a main do colega usa:
void desenharArvore(float x, float y, float z, float rotacao);

#endif