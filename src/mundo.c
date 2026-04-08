#include "../include/mundo.h"
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define M_PI    3.14159265358979323846
#define N_PART  67

// ─── Estrutura de particula 
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float vida;  // 1.0=nova  0.0=morta
    float fase;  // offset individual para o balanco
} Particula;

static Particula partsNeve[N_PART];
static Particula partsFolha[N_PART];
static Particula partsPetala[N_PART];

// vento global 
static float ventoCiclo = 0.0f;
static float ventoX     = 0.0f;
static float ventoZ     = 0.0f;

// ─── Utilitarios 
static float randf(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

static void resetaParticula(Particula* p, float alturaMax) {
    p->x    = randf(-28.0f, 28.0f);
    p->y    = randf(alturaMax * 0.5f, alturaMax);
    p->z    = randf(-28.0f, 28.0f);
    p->vx   = randf(-0.02f,  0.02f);
    p->vy   = randf(-0.05f, -0.02f);
    p->vz   = randf(-0.02f,  0.02f);
    p->vida = 1.0f;
    p->fase = randf(0.0f, 2.0f * (float)M_PI);
}

static int inicializado = 0;
static void inicializaParticulas() {
    if (inicializado) return;
    for (int i = 0; i < N_PART; i++) {
        resetaParticula(&partsNeve[i],    10.0f);
        resetaParticula(&partsFolha[i],    7.0f);
        resetaParticula(&partsPetala[i],   7.0f);
        // distribui verticalmente para nao comecar tudo junto
        partsNeve[i].y   = randf(0.0f, 10.0f);
        partsFolha[i].y  = randf(0.0f,  7.0f);
        partsPetala[i].y = randf(0.0f,  7.0f);
    }
    inicializado = 1;
}
void mundo_update(int estacao) {
    inicializaParticulas();

    // vento oscilante igual ao da arvore
    ventoCiclo += 0.012f;
    ventoX = 0.025f * sinf(ventoCiclo);
    ventoZ = 0.015f * sinf(ventoCiclo * 0.7f);

    if (estacao == 2) {
        // neve: cai quase reto com leve balanco
        for (int i = 0; i < N_PART; i++) {
            Particula* p = &partsNeve[i];
            p->x += p->vx + ventoX + 0.008f * sinf(ventoCiclo + p->fase);
            p->y += p->vy;
            p->z += p->vz + ventoZ;
            p->vida -= 0.004f;
            if (p->vida <= 0.0f || p->y < 0.0f)
                resetaParticula(p, 10.0f);
        }
    } else if (estacao == 1) {
        // folhas outono: mais pesadas, espiral com vento
        for (int i = 0; i < N_PART; i++) {
            Particula* p = &partsFolha[i];
            p->x += p->vx + ventoX * 1.5f + 0.015f * sinf(ventoCiclo * 1.2f + p->fase);
            p->y += p->vy * 0.8f;
            p->z += p->vz + ventoZ * 1.5f;
            p->vida -= 0.003f;
            if (p->vida <= 0.0f || p->y < 0.0f)
                resetaParticula(p, 7.0f);
        }
    } else if (estacao == 3) {
        // petalas primavera: flutuam com vento mais forte
        for (int i = 0; i < N_PART; i++) {
            Particula* p = &partsPetala[i];
            p->x += p->vx + ventoX * 2.0f + 0.02f * sinf(ventoCiclo * 0.8f + p->fase);
            p->y += p->vy * 0.5f + 0.005f * sinf(ventoCiclo * 1.5f + p->fase);
            p->z += p->vz + ventoZ * 2.0f;
            p->vida -= 0.003f;
            if (p->vida <= 0.0f || p->y < 0.0f)
                resetaParticula(p, 7.0f);
        }
    }
}
void definirCorCeu(int estacao) {
    switch (estacao) {
        case 0: // Verão (azul vibrante)
            glClearColor(0.2f, 0.6f, 1.0f, 1.0f);
            break;

        case 1: // Outono ( pôr do sol)
            glClearColor(0.9f, 0.6f, 0.6f, 1.0f);
            break;

        case 2: // Inverno (cinza azulado)
            glClearColor(0.6f, 0.7f, 0.8f, 1.0f);
            break;

        case 3: // Primavera (azul claro)
            glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
            break;
    }
}
static float tempoSol = 0.0f;
void desenhaCirculo(float raio, int segmentos) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f); // Centro
    for (int i = 0; i <= segmentos; i++) {
        float angulo = i * 2.0f * M_PI / segmentos;
        glVertex3f(cos(angulo) * raio, 0.0f, sin(angulo) * raio);
    }
    glEnd();
}
void desenhaEsfera(float raio) {
    glutSolidSphere(raio, 10, 10);
}

// ─── mundo_iluminacao_init ────────────────────────────────────────────
// Habilita o sistema de iluminacao do OpenGL e configura GL_LIGHT0
// como a fonte principal (sol/lua). Deve ser chamada UMA VEZ no init()
// da main, DEPOIS de criar a janela GLUT.
//
// Por que GL_LIGHTING + GL_LIGHT0?
//   O OpenGL fixo (sem shaders) oferece ate 8 luzes (GL_LIGHT0..7).
//   GL_LIGHT0 e a mais comum para luz direcional/sol.
//   Sem glEnable(GL_LIGHTING) as cores glColor3f sao usadas diretamente
//   (flat shading); com ele, a cor final depende da normal da superficie
//   e da posicao/cor da luz — dando volume aos objetos.
//
// Por que GL_COLOR_MATERIAL?
//   Permite que glColor3f continue definindo a cor difusa/ambiente do
//   material sem precisar chamar glMaterialfv a cada objeto. Pratico
//   para projetos onde cada objeto ja tem sua propria cor definida.
void mundo_iluminacao_init() {
    // Ativa o pipeline de iluminacao do OpenGL
    glEnable(GL_LIGHTING);

    // Ativa a luz 0 (sera reposicionada a cada frame pelo sol/lua)
    glEnable(GL_LIGHT0);

    // GL_COLOR_MATERIAL: glColor3f passa a controlar o material difuso+ambiente
    // sem isso, glColor3f seria ignorado quando a iluminacao esta ativa
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Normalizacao automatica das normais apos glScalef
    // (sem isso, o glScalef distorce os calculos de iluminacao)
    glEnable(GL_NORMALIZE);
}

// ─── desenhaSolLua ────────────────────────────────────────────────────
// Orbita o sol (ou lua no inverno) ao redor do mundo usando tempoSol.
// A posicao calculada e usada TANTO para desenhar a esfera
// QUANTO para reposicionar GL_LIGHT0 — garantindo que a sombra e o
// brilho dos objetos sempre venham da direcao visual do sol.
//
// Por que glLightfv com GL_POSITION a cada frame?
//   No OpenGL de matriz fixa, a posicao da luz e transformada pela
//   modelview no momento da chamada glLightfv. Por isso ela deve ser
//   definida APOS o glLoadIdentity/gluLookAt, e ANTES de desenhar os
//   objetos iluminados. Aqui fazemos isso dentro de mundo_desenhar().
//
// Por que w=1.0 no vetor de posicao?
//   w=1.0 → luz posicional (ponto no espaco, gera sombreamento direcional).
//   w=0.0 → luz direcional infinita (raios paralelos, sem atenuacao).
//   Usamos w=1.0 para que a luz venha exatamente da esfera do sol.
void desenhaSolLua(int estacao) {
    tempoSol += 0.0002f;

    float raioOrbita = 40.0f;
    float x = cosf(tempoSol) * raioOrbita;
    float y = sinf(tempoSol) * raioOrbita;
    float posZ = -20.0f;

    // ── Posiciona GL_LIGHT0 na posicao atual do sol/lua ──────────────
    // w=1.0: luz pontual — a luz vem do ponto exato onde o sol esta
    GLfloat luzPos[] = { x, y + 10.0f, posZ, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, luzPos);

    // ── Cor difusa: determina o "tom" de luz que incide nos objetos ──
    // Verão/Primavera: luz quente amarela
    // Outono:          luz alaranjada como por-do-sol
    // Inverno:         luz fria azulada da lua
    GLfloat difusa[4], ambiente[4];
    switch (estacao) {
        case 0: // verao: luz solar diurna quente
            difusa[0]=1.0f;  difusa[1]=0.95f; difusa[2]=0.80f; difusa[3]=1.0f;
            ambiente[0]=0.30f; ambiente[1]=0.32f; ambiente[2]=0.28f; ambiente[3]=1.0f;
            break;
        case 1: // outono: por do sol, luz alaranjada
            difusa[0]=1.0f;  difusa[1]=0.65f; difusa[2]=0.30f; difusa[3]=1.0f;
            ambiente[0]=0.28f; ambiente[1]=0.20f; ambiente[2]=0.15f; ambiente[3]=1.0f;
            break;
        case 2: // inverno: luz da lua, fria e fraca
            difusa[0]=0.55f; difusa[1]=0.60f; difusa[2]=0.75f; difusa[3]=1.0f;
            ambiente[0]=0.15f; ambiente[1]=0.18f; ambiente[2]=0.25f; ambiente[3]=1.0f;
            break;
        case 3: // primavera: luz clara e suave
            difusa[0]=0.95f; difusa[1]=1.00f; difusa[2]=0.85f; difusa[3]=1.0f;
            ambiente[0]=0.32f; ambiente[1]=0.35f; ambiente[2]=0.28f; ambiente[3]=1.0f;
            break;
        default:
            difusa[0]=difusa[1]=difusa[2]=difusa[3]=1.0f;
            ambiente[0]=ambiente[1]=ambiente[2]=0.3f; ambiente[3]=1.0f;
    }

    // GL_DIFFUSE: cor da luz que incide diretamente na superficie
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  difusa);

    // GL_AMBIENT: luz ambiente global — evita que areas sem luz direta
    // fiquem completamente pretas (simula luz indireta/reflexao difusa)
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambiente);

    // ── Quando o sol esta abaixo do horizonte (y<0): escurece tudo ──
    // Simulamos a noite/entardecer reduzindo a intensidade da luz ambiente
    if (y < 0.0f) {
        float fator = 0.1f; // escuro maximo
        GLfloat noite[] = { fator, fator, fator * 1.2f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT, noite);
        GLfloat difNoite[] = { fator*2, fator*2, fator*2.5f, 1.0f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, difNoite);
    }

    // ── Desenha a esfera visual do sol/lua ───────────────────────────
    // glDisable(GL_LIGHTING) aqui porque a esfera do sol nao deve ser
    // afetada pela propria luz que ela emite — ela sempre aparece brilhante
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(x, y + 10.0f, posZ);
        if (estacao == 2)
            glColor3f(0.85f, 0.85f, 0.90f); // lua: branco azulado
        else
            glColor3f(1.0f, 0.92f, 0.30f);  // sol: amarelo quente
        desenhaEsfera(2.5f);
    glPopMatrix();
    glEnable(GL_LIGHTING); // reativa para os objetos do cenario
}
// ─── Chao por estacao 
static void desenhaChao(int estacao) {
    glDisable(GL_TEXTURE_2D);

    switch (estacao) {
        case 0: glColor3f(0.45f, 0.78f, 0.32f); break; // verao: verde
        case 1: glColor3f(0.50f, 0.62f, 0.25f); break; // outono: amarelado
        case 2: glColor3f(0.88f, 0.92f, 0.95f); break; // inverno: neve
        case 3: glColor3f(0.50f, 0.82f, 0.40f); break; // primavera: verde claro
    }
    glBegin(GL_QUADS);
        glVertex3f(-35.0f, 0.0f, -35.0f);
        glVertex3f(-35.0f, 0.0f,  35.0f);
        glVertex3f( 35.0f, 0.0f,  35.0f);
        glVertex3f( 35.0f, 0.0f, -35.0f);
    glEnd();

    // listras decorativas
    switch (estacao) {
        case 0: glColor3f(0.42f, 0.74f, 0.29f); break;
        case 1: glColor3f(0.46f, 0.58f, 0.22f); break;
        case 2: glColor3f(0.82f, 0.88f, 0.92f); break;
        case 3: glColor3f(0.46f, 0.78f, 0.36f); break;
    }
    for (int i = -5; i <= 5; i++) {
        glBegin(GL_QUADS);
            glVertex3f(i * 4.0f - 0.6f, 0.01f, -30.0f);
            glVertex3f(i * 4.0f - 0.6f, 0.01f,  30.0f);
            glVertex3f(i * 4.0f + 0.6f, 0.01f,  30.0f);
            glVertex3f(i * 4.0f + 0.6f, 0.01f, -30.0f);
        glEnd();
    }
}

static void desenhaCaminho(int estacao) {
    if (estacao == 2) glColor3f(0.80f, 0.84f, 0.90f);
    else              glColor3f(0.60f, 0.50f, 0.40f);

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -1.0f);
        glBegin(GL_QUADS);
            glVertex3f(-17.0f, 0.02f, 0.0f);
            glVertex3f( 17.0f, 0.02f, 0.0f);
            glVertex3f( 17.0f, 0.02f, 2.0f);
            glVertex3f(-17.0f, 0.02f, 2.0f);
        glEnd();
    glPopMatrix();
}

static void desenhaFolhasNoChao(int estacao) { //outono
    if (estacao != 1) return;
    glColor3f(0.70f, 0.30f, 0.00f);
    for (int i = 0; i < 15; i++) {
        glPushMatrix();
            glTranslatef(11.5f + (i % 5), 0.02f, -2.0f + (i / 3));
            glRotatef(i * 30, 0, 1, 0);
            glScalef(0.2f, 0.01f, 0.3f);
            glutSolidSphere(1.0f, 5, 5);
        glPopMatrix();
    }
}

// --- Desenha um regador vermelho metalizado low-poly ---
void desenharRegador(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Escala menor (60% do tamanho)
    glScalef(0.6f, 0.6f, 0.6f);
    
    // Instancia o objeto Quadric do GLU
    GLUquadric* q = gluNewQuadric();
    
    // --- CORPO PRINCIPAL (Oco com Água e Meia-Tampa) ---
    glPushMatrix();
        glRotatef(-90, 1, 0, 0); // Vira o cilindro para ficar em pé
        
        // Parede do regador (Vermelho Metálico)
        glColor3f(0.7f, 0.1f, 0.15f);
        gluCylinder(q, 0.35, 0.35, 0.7, 12, 2); 
        
        // Fundo do regador
        glPushMatrix(); 
            glRotatef(180, 1, 0, 0); 
            gluDisk(q, 0.0, 0.35, 12, 1); 
        glPopMatrix();
        
        // Meia-tampa superior corrigida (cobre a metade traseira)
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.7f); 
            // Começa em 270 graus em vez de 90 para cobrir a metade oposta
            gluPartialDisk(q, 0.0, 0.35, 12, 1, 270.0, 180.0);
        glPopMatrix();
        
        // Água interna (Azul) na metade da altura
        glColor3f(0.1f, 0.4f, 0.8f);
        glPushMatrix(); 
            glTranslatef(0.0f, 0.0f, 0.35f); 
            gluDisk(q, 0.0, 0.34, 12, 1); 
        glPopMatrix();
    glPopMatrix();
    
    // Retorna a cor vermelha para os próximos objetos
    glColor3f(0.7f, 0.1f, 0.15f);
    
    // --- ALÇA VERTICAL ---
    glPushMatrix();
        // Posicionada na parede traseira (-0.35)
        glTranslatef(0.0f, 0.35f, -0.35f); 
        glRotatef(90, 0, 1, 0); 
        glutSolidTorus(0.05, 0.25, 6, 8); // Desenhada inteira, a meia-tampa esconde a parte de dentro
    glPopMatrix();
    
    // --- BICO LONGO E CRIVO ---
    glPushMatrix();
        glTranslatef(0.0f, 0.2f, 0.35f); 
        glRotatef(-45.0f, 1, 0, 0); // Aponta em diagonal para cima
        
        // Cano mais longo
        gluCylinder(q, 0.05, 0.05, 0.7, 8, 1);
        
        // Vai para a ponta do cano
        glTranslatef(0.0f, 0.0f, 0.7f); 
        
        // Ativa o corte da semi-esfera do crivo
        double clip_crivo_eq[4] = {0.0, 0.0, -1.0, 0.0}; 
        glClipPlane(GL_CLIP_PLANE0, clip_crivo_eq);
        glEnable(GL_CLIP_PLANE0);
        
        // Esfera base do crivo
        glutSolidSphere(0.12, 8, 8);
        
        // Desliga o corte do crivo
        glDisable(GL_CLIP_PLANE0); 
        
        // Tampa plana do crivo (Cinza escuro)
        glColor3f(0.2f, 0.2f, 0.2f); 
        gluDisk(q, 0.0, 0.12, 8, 1);
        
    glPopMatrix();
    
    gluDeleteQuadric(q); // Libera a memória
    glPopMatrix();
}

static void desenhaCercaUnidade() {
    glColor3f(0.50f, 0.35f, 0.05f);
    glPushMatrix();
        glTranslatef(0, 0.5f, 0);
        glScalef(0.1f, 1.0f, 0.1f);
        glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.5f, 0.4f, 0);
        glScalef(1.0f, 0.15f, 0.05f);
        glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.5f, 0.8f, 0);
        glScalef(1.0f, 0.15f, 0.05f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void desenhaLago() {
    glEnable(GL_BLEND); // Ativa transparência
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(0.1f, 0.4f, 0.8f, 0.7f); // Azul (agua)

    glPushMatrix();
        glTranslatef(-22.0f, 0.2f, -22.0f); // Posicao
        glScalef(1.5f, 1.0f, 1.0f); 
        desenhaCirculo(2.0f, 30);
    glPopMatrix();
    
    glDisable(GL_BLEND); // Desativa transparência para não afetar outros objetos
}

static void desenhaNeve() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(3.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < N_PART; i++) {
        Particula* p = &partsNeve[i];
        glColor4f(0.95f, 0.97f, 1.0f, p->vida * 0.9f);
        glVertex3f(p->x, p->y, p->z);
    }
    glEnd();
    glDisable(GL_BLEND);
    glPointSize(1.0f);
}

static void desenhaFolhasVoando() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < N_PART; i++) {
        Particula* p = &partsFolha[i];
        glPushMatrix();
            glTranslatef(p->x, p->y, p->z);
            glRotatef(ventoX * 1800.0f, 0.0f, 0.0f, 1.0f);
            glScalef(0.12f, 0.01f, 0.18f);
            glColor4f(0.80f, 0.35f + p->vida * 0.1f, 0.0f, p->vida * 0.85f);
            glutSolidSphere(1.0f, 5, 5);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
}

static void desenhaPetalas() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < N_PART; i++) {
        Particula* p = &partsPetala[i];
        glPushMatrix();
            glTranslatef(p->x, p->y, p->z);
            glRotatef(ventoX * 1200.0f, 0.0f, 1.0f, 0.0f);
            glScalef(0.10f, 0.01f, 0.14f);
            glColor4f(1.0f, 0.65f + p->vida * 0.1f, 0.80f, p->vida * 0.80f);
            glutSolidSphere(1.0f, 5, 5);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
}
void desenhaPedras() {
    // Usamos uma semente diferente da vegetação para não sobrepor tudo
    srand(76); 

    for (int i = 0; i < 20; i++) { // Sorteia posição X e Z
        float x = (float)(rand() % 58) - 29.0f; 
        float z = (float)(rand() % 58) - 29.0f; 

        // EVITAR ÁREAS ESPECÍFICAS:
        // Se a pedra cair dentro do caminho, pulamos para a próxima iteração
        if (z > -0.5f && z < 2.5f && x > -16.0f && x < 16.0f) continue;
        // Se cair dentro do lago
        if (x < -16.0f && z < -16.0f) continue;

        glPushMatrix();
            glTranslatef(x, 0.05f, z);
            
            // Variar o tamanho de cada pedra para não ficarem iguais
            float escala = 0.2f + (float)(rand() % 5) / 10.0f; 
            glScalef(escala * 1.5f, escala * 0.5f, escala); 
            
            // Variar levemente a cor cinza
            float tom = 0.4f + (float)(rand() % 3) / 10.0f;
            glColor3f(tom, tom, tom);
            
            glutSolidSphere(1.0f, 8, 8);
        glPopMatrix();
    }
}
void desenhaVegetacao(int estacao) {
    // Usamos semente fixa para que as flores não "pulem" de lugar a cada frame
    srand(67); 

    for (int i = 0; i < 50; i++) {
        float x = (float)(rand() % 56) - 28.0f; // Espalha pelo mapa
        float z = (float)(rand() % 56) - 28.0f;
        
        // Evita desenhar em cima do caminho (aproximadamente)
        if (z > -1.0f && z < 3.0f && x > -16.0f && x < 16.0f) continue;
        // Evita desenhar dentro do lago
        if (x < -16.0f && z < -16.0f) continue;

        // Desenha Grama 
        if (estacao == 2) // Inverno: Grama coberta de neve
            glColor3f(0.8f, 0.8f, 0.85f);
        else if (estacao == 1) // Outono: Grama seca
            glColor3f(0.5f, 0.5f, 0.2f);
        else // Primavera/Verão: Verde vivo
            glColor3f(0.3f, 0.6f, 0.2f);

        glBegin(GL_TRIANGLES);
            glVertex3f(x - 0.1f, 0.01f, z);
            glVertex3f(x + 0.1f, 0.01f, z);
            glVertex3f(x, 0.3f, z); // Ponta da grama
        glEnd();

        // --- Desenha Flores (Pequenas Esferas) ---
        int desenharFlor = (rand() % 5 == 0); // 20% de chance de ter flor

        if (desenharFlor) {
            if (estacao == 0) { // Primavera: Flores Coloridas
                float r = (float)(rand() % 100) / 100.0f;
                float g = (float)(rand() % 100) / 100.0f;
                glColor3f(r, g, 1.0f); // Predominância de azul/roxo
            } else if (estacao == 3) { // Verão: Flores Amarelas/Vermelhas
                if (rand() % 2 == 0) glColor3f(1.0f, 0.9f, 0.0f); // Amarelo
                else glColor3f(1.0f, 0.2f, 0.2f); // Vermelho
            } else {
                continue; // Outono/Inverno: Sem flores
            }

            glPushMatrix();
                glTranslatef(x, 0.2f, z); // No topo da grama
                glutSolidSphere(0.1f, 5, 5);
            glPopMatrix();
        }
    }
}
void configurarFog(int estacao) {
    glEnable(GL_FOG);
    
    GLfloat fogColor[4];
    switch (estacao) {
        case 0: fogColor[0]=0.2f; fogColor[1]=0.6f; fogColor[2]=1.0f; break; // Verão
        case 1: fogColor[0]=0.9f; fogColor[1]=0.6f; fogColor[2]=0.6f; break; // Outono
        case 2: fogColor[0]=0.6f; fogColor[1]=0.7f; fogColor[2]=0.8f; break; // Inverno
        case 3: fogColor[0]=0.4f; fogColor[1]=0.7f; fogColor[2]=1.0f; break; // Primavera
    }
    fogColor[3] = 1.0f;

    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_EXP2);      // neblina 
    glFogf(GL_FOG_DENSITY, 0.002f);    // densidade
    glHint(GL_FOG_HINT, GL_NICEST);
}
static void desenhaMontanha(int estacao) {
    glPushMatrix();
        // Posiciona bem ao fundo, como uma tela de teatro
        glTranslatef(0.0f, 0.0f, -33.0f); 
        
        // Cor da montanha: um tom que mude com a estação
        if (estacao == 2) glColor3f(0.7f, 0.75f, 0.85f); // Inverno (azul frio)
        else if (estacao == 1) glColor3f(0.5f, 0.3f, 0.2f); // Outono (marrom terra)
        else glColor3f(0.2f, 0.4f, 0.2f); // Primavera/Verão (verde escuro)

        glBegin(GL_POLYGON);
            // Desenha a silhueta (x, y, z)
            glVertex3f(-30.0f,  0.0f, 0.1f);  // Base esquerda
            glVertex3f(-20.0f, 15.0f, 0.2f);  // Pico 1
            glVertex3f(-10.0f,  8.0f, 0.3f);  // Vale
            glVertex3f(  0.0f, 20.0f, 0.4f);  // Pico central (mais alto)
            glVertex3f( 12.5f, 10.0f, 0.5f);  // Vale
            glVertex3f( 25.0f, 18.0f, 0.6f);  // Pico 2
            glVertex3f( 35.0f,  0.0f, 0.7f);  // Base direita
        glEnd();
        
    glPopMatrix();
}
void mundo_desenhar(int estacao) {
    //inicializaParticulas();

    desenhaSolLua(estacao);
    definirCorCeu(estacao);   
    configurarFog(estacao);
    desenhaChao(estacao);
    desenhaLago();
    desenhaMontanha(estacao);
    desenhaCaminho(estacao);

    desenhaPedras(); 
    desenhaVegetacao(estacao); 
    desenhaFolhasNoChao(estacao);

    // cercas
    for (float i = -30.0f; i < 30.0f; i += 1.0f) {
        glPushMatrix(); glTranslatef(i, 0, 29.8f);
            desenhaCercaUnidade();
        glPopMatrix();
    }
    for (float i = -29.0f; i < 31.0f; i += 1.0f) {
        glPushMatrix(); glTranslatef(-29.8f, 0.0f, i); glRotatef(90, 0, 1, 0);
            desenhaCercaUnidade();
        glPopMatrix();
    }
    for (float i = -30.0f; i < 30.0f; i += 1.0f) {
        glPushMatrix(); glTranslatef(i, 0, -29.8f);
            desenhaCercaUnidade();
        glPopMatrix();
    }
    for (float i = -29.5f; i < 31.0f; i += 1.0f) {
        glPushMatrix(); glTranslatef(29.8f, 0.0f, i); glRotatef(90, 0, 1, 0);
            desenhaCercaUnidade();
        glPopMatrix();
    }

    // particulas por estacao
    switch (estacao) {
        case 1: desenhaFolhasVoando(); break;
        case 2: desenhaNeve();         break;
        case 3: desenhaPetalas();      break;
    }
}
int verificaColisao(float proxX, float proxZ) {
    if (proxX > 29.0f || proxX < -29.0f) return 1;
    if (proxZ > 29.0f || proxZ < -29.0f) return 1;
    return 0;
}