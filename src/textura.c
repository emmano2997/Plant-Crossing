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
            // Tufos de grama com sombra embaixo e highlight em cima
            int tufoH = 6;
            int ty = y % tufoH;
            float sombra = 1.0f;
            if (ty == 0) sombra = 0.6f;           // base escura (sombra do solo)
            else if (ty == 1) sombra = 0.75f;     // transicao
            else if (ty >= tufoH - 1) sombra = 1.15f; // ponta clara (luz)
            int r = (int)((60 + var) * sombra);
            int g = (int)((140 + var) * sombra);
            int b = (int)((40 + var / 2) * sombra);
            dados[i]     = (unsigned char)(r > 255 ? 255 : r);
            dados[i + 1] = (unsigned char)(g > 255 ? 255 : g);
            dados[i + 2] = (unsigned char)(b > 255 ? 255 : b);
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
                // Argamassa — sulco escuro simulando profundidade
                dados[i] = 140; dados[i+1] = 135; dados[i+2] = 125;
            } else {
                int var = (x * 7 + y * 13) % 15;
                int baseR = 225 + var % 10;
                int baseG = 210 + var % 8;
                int baseB = 185 + var % 6;
                float sombra = 1.0f;
                // Borda superior do tijolo — sombra (luz vem de cima)
                if (by == 1) sombra = 0.7f;
                // Borda esquerda — sombra lateral
                else if (bx == 1) sombra = 0.75f;
                // Borda inferior — highlight (face virada pra luz)
                else if (by == brickH - 1) sombra = 1.1f;
                // Borda direita — highlight lateral
                else if (bx == brickW - 1) sombra = 1.05f;
                int r = (int)(baseR * sombra);
                int g = (int)(baseG * sombra);
                int b = (int)(baseB * sombra);
                dados[i]     = (unsigned char)(r > 255 ? 255 : r);
                dados[i + 1] = (unsigned char)(g > 255 ? 255 : g);
                dados[i + 2] = (unsigned char)(b > 255 ? 255 : b);
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
            int tileH = 10, tileW = 16;
            int row = y / tileH;
            int offset = (row % 2) * (tileW / 2);
            int bx = (x + offset) % tileW;
            int by = y % tileH;

            if (by == 0) {
                // Sombra da sobreposicao entre telhas (sulco profundo)
                dados[i] = 100; dados[i+1] = 50; dados[i+2] = 10;
            } else if (by == 1) {
                // Borda inferior da telha de cima — sombra projetada
                dados[i] = 130; dados[i+1] = 65; dados[i+2] = 15;
            } else {
                int var = (bx * 3 + by * 7) % 10;
                int baseR = 200 + var;
                int baseG = 120 + var;
                int baseB = 30 + var;
                float sombra = 1.0f;
                // Parte superior da telha — highlight (face exposta ao sol)
                if (by >= tileH - 2) sombra = 1.15f;
                // Curvatura central — levemente mais escura
                else if (by == tileH / 2) sombra = 0.9f;
                // Bordas laterais — leve sombra
                if (bx == 0 || bx == tileW - 1) sombra *= 0.8f;
                int r = (int)(baseR * sombra);
                int g = (int)(baseG * sombra);
                int b = (int)(baseB * sombra);
                dados[i]     = (unsigned char)(r > 255 ? 255 : r);
                dados[i + 1] = (unsigned char)(g > 255 ? 255 : g);
                dados[i + 2] = (unsigned char)(b > 255 ? 255 : b);
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
                // Fresta entre tabuas — sombra profunda
                dados[i] = 35; dados[i+1] = 20; dados[i+2] = 8;
            } else if (px == 1) {
                // Borda esquerda da tabua — sombra da fresta
                dados[i] = 90; dados[i+1] = 55; dados[i+2] = 25;
            } else if (px == plankW - 1) {
                // Borda direita — highlight (chanfro pega luz)
                dados[i] = 180; dados[i+1] = 120; dados[i+2] = 65;
            } else {
                float grain = sinf(y * 0.5f + sinf(x * 0.3f) * 2.0f) * 0.5f + 0.5f;
                int var = (int)(grain * 30.0f);
                // Curvatura da tabua: centro mais claro, bordas mais escuras
                float dist = (float)(px - plankW / 2) / (float)(plankW / 2);
                float curva = 1.0f - dist * dist * 0.2f;
                int r = (int)((140 + var) * curva);
                int g = (int)((90  + var / 2) * curva);
                int b = (int)((45  + var / 3) * curva);
                dados[i]     = (unsigned char)(r > 255 ? 255 : r);
                dados[i + 1] = (unsigned char)(g > 255 ? 255 : g);
                dados[i + 2] = (unsigned char)(b > 255 ? 255 : b);
            }
        }
    }
    return geraTexturaProcedural(dados, TEX_SZ, TEX_SZ);
}

void texturas_init() {
    texTronco  = carregaTextura("src/texture/log-texture-brown.jpg");
    texGrama   = geraTexturaGrama();
    texParede  = geraTexturaParede();
    texTelhado = geraTexturaTelhado();
    texMadeira = geraTexturaMadeira();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
