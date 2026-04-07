#ifndef ARVORE_H
#define ARVORE_H

#include <GL/glut.h>

// ─── Texturas ────────────────────────────────────────────────────────
extern GLuint texTronco;
extern GLuint texFolha;

// ─── Animacao ────────────────────────────────────────────────────────
extern float growthScale;
extern float windOffset;
extern float bounceY;

// ─── Estado da planta ────────────────────────────────────────────────
// 0=brotinho  1=jovem  2=adulta  3=madura(+fruta)  4=morta
extern int estagioPlantas;

// ─── Estado do mundo ─────────────────────────────────────────────────
// 0=verao  1=outono  2=inverno  3=primavera
extern int estacaoAtual;

// Flag: jogador regou nessa estacao (0=nao 1=sim)
extern int regouEssaEstacao;

// ─── Funcoes publicas ────────────────────────────────────────────────

// Chama no init() — carrega texturas
void arvore_init();

// Chama no update() — atualiza animacoes
void arvore_update();

// Chama no display() — desenha a arvore na posicao (x,y,z) com rotacao Y
void desenharArvore(float x, float y, float z, float rotacao);

// Jogador interagiu com a planta para regar
void arvore_regar();

// Jogador dormiu — faz a transicao de estagio + estacao
// Retorna 1 se a planta morreu nessa transicao, 0 caso contrario
int arvore_dormir();

#endif