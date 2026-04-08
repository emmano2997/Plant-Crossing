#include "../include/textura.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint texGrama   = 0;
GLuint texParede  = 0;
GLuint texTelhado = 0;
GLuint texMadeira = 0;
GLuint texTronco  = 0;

// Carrega textura de arquivo com mipmaps (padrao do professor)
GLuint carregaTextura(const char* arquivo) {
    GLuint texID;
    int largura, altura, canais;
    unsigned char* dados = stbi_load(arquivo, &largura, &altura, &canais, 0);
    if (dados) {
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum formato = (canais == 4) ? GL_RGBA : GL_RGB;
        gluBuild2DMipmaps(GL_TEXTURE_2D, formato, largura, altura,
                          formato, GL_UNSIGNED_BYTE, dados);
        stbi_image_free(dados);
    } else {
        printf("Erro ao carregar textura: %s\n", arquivo);
        texID = 0;
    }
    return texID;
}

// Gera textura procedural com mipmaps
static GLuint geraTexturaProcedural(unsigned char* dados, int largura, int altura) {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, largura, altura,
                      GL_RGB, GL_UNSIGNED_BYTE, dados);
    return texID;
}

#define TEX_SZ 64

static GLuint geraTexturaGrama() {
    unsigned char dados[TEX_SZ * TEX_SZ * 3];
    srand(42);
    for (int y = 0; y < TEX_SZ; y++) {
        for (int x = 0; x < TEX_SZ; x++) {
            int i = (y * TEX_SZ + x) * 3;
            int var = rand() % 30;
            dados[i]     = (unsigned char)(60 + var);
            dados[i + 1] = (unsigned char)(140 + var);
            dados[i + 2] = (unsigned char)(40 + var / 2);
        }
    }
    return geraTexturaProcedural(dados, TEX_SZ, TEX_SZ);
}

static GLuint geraTexturaParede() {
    unsigned char dados[TEX_SZ * TEX_SZ * 3];
    for (int y = 0; y < TEX_SZ; y++) {
        for (int x = 0; x < TEX_SZ; x++) {
            int i = (y * TEX_SZ + x) * 3;
            int brickH = 8, brickW = 16;
            int row = y / brickH;
            int offset = (row % 2) * (brickW / 2);
            int bx = (x + offset) % brickW;
            int by = y % brickH;

            if (by == 0 || bx == 0) {
                dados[i] = 180; dados[i+1] = 175; dados[i+2] = 165;
            } else {
                int var = (x * 7 + y * 13) % 15;
                dados[i]     = (unsigned char)(225 + var % 10);
                dados[i + 1] = (unsigned char)(210 + var % 8);
                dados[i + 2] = (unsigned char)(185 + var % 6);
            }
        }
    }
    return geraTexturaProcedural(dados, TEX_SZ, TEX_SZ);
}

static GLuint geraTexturaTelhado() {
    unsigned char dados[TEX_SZ * TEX_SZ * 3];
    for (int y = 0; y < TEX_SZ; y++) {
        for (int x = 0; x < TEX_SZ; x++) {
            int i = (y * TEX_SZ + x) * 3;
            int tileH = 8;
            int by = y % tileH;

            if (by < 2) {
                dados[i] = 160; dados[i+1] = 80; dados[i+2] = 20;
            } else {
                int var = (x * 3 + y * 7) % 10;
                dados[i]     = (unsigned char)(200 + var);
                dados[i + 1] = (unsigned char)(120 + var);
                dados[i + 2] = (unsigned char)(30 + var);
            }
        }
    }
    return geraTexturaProcedural(dados, TEX_SZ, TEX_SZ);
}

static GLuint geraTexturaMadeira() {
    unsigned char dados[TEX_SZ * TEX_SZ * 3];
    for (int y = 0; y < TEX_SZ; y++) {
        for (int x = 0; x < TEX_SZ; x++) {
            int i = (y * TEX_SZ + x) * 3;
            int plankW = 16;
            int px = x % plankW;

            if (px == 0) {
                dados[i] = 60; dados[i+1] = 35; dados[i+2] = 15;
            } else {
                float grain = sinf(y * 0.5f + sinf(x * 0.3f) * 2.0f) * 0.5f + 0.5f;
                int var = (int)(grain * 30.0f);
                dados[i]     = (unsigned char)(140 + var);
                dados[i + 1] = (unsigned char)(90  + var / 2);
                dados[i + 2] = (unsigned char)(45  + var / 3);
            }
        }
    }
    return geraTexturaProcedural(dados, TEX_SZ, TEX_SZ);
}

void texturas_init() {
    texTronco  = carregaTextura("texture/log-texture-brown.jpg");
    texGrama   = geraTexturaGrama();
    texParede  = geraTexturaParede();
    texTelhado = geraTexturaTelhado();
    texMadeira = geraTexturaMadeira();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
