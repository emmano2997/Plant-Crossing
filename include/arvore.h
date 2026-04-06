#ifndef ARVORE_H
#define ARVORE_H

extern float growthScale;

void arvore_update();
void desenhaCilindro(float raioBase, float raioTopo, float altura);
void criarArvore(float altura, float raio, int nivel);

#endif