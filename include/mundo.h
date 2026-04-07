#ifndef MUNDO_H
#define MUNDO_H

#include <GL/glut.h>

// Verifica colisao com as cercas (retorna 1 se colidiu)
int verificaColisao(float proxX, float proxZ);

// Chama UMA VEZ no init() da main — habilita GL_LIGHTING e configura GL_LIGHT0
void mundo_iluminacao_init();

// Chama no update() da main — atualiza particulas de neve/folhas/petalas
// estacao: 0=verao  1=outono  2=inverno  3=primavera
void mundo_update(int estacao);

// Chama no display() da main — desenha o cenario completo para a estacao
void mundo_desenhar(int estacao);

#endif