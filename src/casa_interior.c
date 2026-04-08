#include "../include/casa_interior.h"
#include <GL/glut.h>

// Macro auxiliar para reduzir repetição
#define QUAD(x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4) \
    glBegin(GL_QUADS);                               \
    glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2);      \
    glVertex3f(x3,y3,z3); glVertex3f(x4,y4,z4);      \
    glEnd();

// Função auxiliar para desenhar blocos coloridos com falso sombreamento (Low Poly)
static void desenhaCaixaCor(float cx, float cy, float cz, float sx, float sy, float sz, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glScalef(sx, sy, sz);
    glBegin(GL_QUADS);
    // Cima (Mais claro)
    glColor3f(r*1.1f, g*1.1f, b*1.1f);
    glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
    // Frente e Trás (Cor base)
    glColor3f(r, g, b);
    glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
    // Laterais (Mais escuro)
    glColor3f(r*0.8f, g*0.8f, b*0.8f);
    glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f,-0.5f,-0.5f);
    glEnd();
    glPopMatrix();
}

// ── Móveis ────────────────────────────────────────────────────

static void desenhaCama() {
    // Base/Estrutura de madeira
    desenhaCaixaCor(-3.5f, 0.3f, -3.0f, 2.6f, 0.4f, 3.8f, 0.7f, 0.4f, 0.1f);
    // Cabeceira
    desenhaCaixaCor(-3.5f, 0.8f, -4.8f, 2.8f, 1.2f, 0.2f, 0.7f, 0.4f, 0.1f);
    // Colchão (Branco)
    desenhaCaixaCor(-3.5f, 0.6f, -3.0f, 2.4f, 0.3f, 3.6f, 0.95f, 0.95f, 0.95f);
    // Coberta (Azul com bolinhas pode ser complexo, vai azul sólido para low poly)
    desenhaCaixaCor(-3.5f, 0.65f, -2.0f, 2.45f, 0.35f, 2.0f, 0.1f, 0.4f, 0.8f);
    // Travesseiro (Laranja)
    desenhaCaixaCor(-3.5f, 0.8f, -4.2f, 1.4f, 0.2f, 0.8f, 0.9f, 0.4f, 0.1f);
}

static void desenhaMesaCabeceira() {
    float mx = -1.2f, mz = -4.4f;
    // Corpo
    desenhaCaixaCor(mx, 0.6f, mz, 1.0f, 0.8f, 1.0f, 0.95f, 0.9f, 0.8f);
    // Tampo e base de madeira
    desenhaCaixaCor(mx, 1.05f, mz, 1.1f, 0.1f, 1.1f, 0.7f, 0.4f, 0.1f);
    desenhaCaixaCor(mx, 0.15f, mz, 1.1f, 0.1f, 1.1f, 0.7f, 0.4f, 0.1f);
    // Pés
    desenhaCaixaCor(mx-0.4f, 0.05f, mz-0.4f, 0.1f, 0.2f, 0.1f, 0.7f, 0.4f, 0.1f);
    desenhaCaixaCor(mx+0.4f, 0.05f, mz-0.4f, 0.1f, 0.2f, 0.1f, 0.7f, 0.4f, 0.1f);
    desenhaCaixaCor(mx-0.4f, 0.05f, mz+0.4f, 0.1f, 0.2f, 0.1f, 0.7f, 0.4f, 0.1f);
    desenhaCaixaCor(mx+0.4f, 0.05f, mz+0.4f, 0.1f, 0.2f, 0.1f, 0.7f, 0.4f, 0.1f);
    // Puxadores (Preto)
    desenhaCaixaCor(mx, 0.75f, mz+0.52f, 0.15f, 0.15f, 0.05f, 0.1f, 0.1f, 0.1f);
    desenhaCaixaCor(mx, 0.45f, mz+0.52f, 0.15f, 0.15f, 0.05f, 0.1f, 0.1f, 0.1f);
}

static void desenhaArmario() {
    // Desloquei o centro X para 3.5f para acomodar a nova largura, mantendo encostado na parede (5.0f)
    float ax = 3.5f, az = -4.0f; 
    
    // Corpo principal (Aumentei a largura em X de 1.8f para 3.0f)
    desenhaCaixaCor(ax, 1.6f, az, 3.0f, 3.2f, 1.5f, 0.4f, 0.25f, 0.15f);
    
    // Portas (Giradas 180º: agora o Z soma +0.76f, ficando de frente para a sala)
    // Além disso, aumentei a largura das portas para cobrir o novo tamanho do armário
    desenhaCaixaCor(ax-0.75f, 1.5f, az+0.76f, 1.4f, 2.8f, 0.05f, 0.45f, 0.28f, 0.18f); // Porta Esquerda
    desenhaCaixaCor(ax+0.75f, 1.5f, az+0.76f, 1.4f, 2.8f, 0.05f, 0.45f, 0.28f, 0.18f); // Porta Direita
    
    // Puxadores (Também girados para Z positivo)
    desenhaCaixaCor(ax-0.1f, 1.5f, az+0.8f, 0.05f, 0.3f, 0.05f, 0.8f, 0.7f, 0.2f);
    desenhaCaixaCor(ax+0.1f, 1.5f, az+0.8f, 0.05f, 0.3f, 0.05f, 0.8f, 0.7f, 0.2f);
}

static void desenhaMesaCadeira() {
    // --- MESA ---
    float deskX = 3.8f, deskZ = 0.0f;
    // Tampo da mesa (Altura reduzida de 1.2f para 0.95f)
    desenhaCaixaCor(deskX, 0.95f, deskZ, 1.6f, 0.1f, 2.6f, 0.45f, 0.25f, 0.15f);
    // Pernas da mesa (Altura e escala ajustadas para tocarem o chão perfeitamente)
    desenhaCaixaCor(deskX-0.7f, 0.45f, deskZ-1.2f, 0.15f, 0.9f, 0.15f, 0.45f, 0.25f, 0.15f);
    desenhaCaixaCor(deskX+0.7f, 0.45f, deskZ-1.2f, 0.15f, 0.9f, 0.15f, 0.45f, 0.25f, 0.15f);
    desenhaCaixaCor(deskX-0.7f, 0.45f, deskZ+1.2f, 0.15f, 0.9f, 0.15f, 0.45f, 0.25f, 0.15f);
    desenhaCaixaCor(deskX+0.7f, 0.45f, deskZ+1.2f, 0.15f, 0.9f, 0.15f, 0.45f, 0.25f, 0.15f);

    // --- CADEIRA ---
    glPushMatrix();
    glTranslatef(2.2f, 0.0f, -0.2f); 
    glRotatef(15.0f, 0.0f, 1.0f, 0.0f); 
    // Assento (Altura reduzida de 0.6f para 0.45f)
    desenhaCaixaCor(0, 0.45f, 0, 0.8f, 0.1f, 0.8f, 0.5f, 0.3f, 0.15f);
    // Encosto (Abaixado proporcionalmente)
    desenhaCaixaCor(-0.35f, 0.85f, 0, 0.1f, 0.8f, 0.8f, 0.5f, 0.3f, 0.15f);
    // Pernas da cadeira (Ajustadas para tocarem o chão e sustentarem o assento mais baixo)
    desenhaCaixaCor(-0.35f, 0.2f, -0.35f, 0.1f, 0.4f, 0.1f, 0.5f, 0.3f, 0.15f);
    desenhaCaixaCor( 0.35f, 0.2f, -0.35f, 0.1f, 0.4f, 0.1f, 0.5f, 0.3f, 0.15f);
    desenhaCaixaCor(-0.35f, 0.2f,  0.35f, 0.1f, 0.4f, 0.1f, 0.5f, 0.3f, 0.15f);
    desenhaCaixaCor( 0.35f, 0.2f,  0.35f, 0.1f, 0.4f, 0.1f, 0.5f, 0.3f, 0.15f);
    glPopMatrix();
}

// ── Funções Estruturais (Inalteradas) ─────────────────────────

static void desenhaPortaInterior(float zPos) {
    float zF = zPos - 0.05f; 
    glColor3f(0.82f, 0.22f, 0.12f);
    QUAD(-1.0f, 0, zF, 1.0f, 0, zF, 1.0f, 2.5f, zF, -1.0f, 2.5f, zF)
    glColor3f(0.08f, 0.08f, 0.12f);
    QUAD(-0.6f, 1.5f, zF-0.01f,  0.6f, 1.5f, zF-0.01f,  0.6f, 2.2f, zF-0.01f, -0.6f, 2.2f, zF-0.01f)
    glColor3f(0.6f, 0.15f, 0.1f);
    QUAD(-0.6f, 1.83f, zF-0.02f,  0.6f, 1.83f, zF-0.02f,  0.6f, 1.87f, zF-0.02f, -0.6f, 1.87f, zF-0.02f)
    QUAD(-0.02f, 1.5f, zF-0.02f,  0.02f, 1.5f, zF-0.02f,  0.02f, 2.2f, zF-0.02f, -0.02f, 2.2f, zF-0.02f) 
    glColor3f(0.75f, 0.60f, 0.1f);
    glPushMatrix(); glTranslatef(0.8f, 1.2f, zF-0.03f); glutSolidSphere(0.06f, 8, 8); glPopMatrix();
}

static void desenhaJanelaInteriorDir(float paredeX) {
    float jx = paredeX - 0.01f; 
    float frameW = 0.15f;
    glColor3f(0.6f, 0.15f, 0.1f);
    QUAD(jx, 2.5f, 1.0f+frameW, jx, 2.5f, -1.0f-frameW, jx, 2.5f+frameW, -1.0f-frameW, jx, 2.5f+frameW, 1.0f+frameW)
    QUAD(jx, 1.2f-frameW, 1.0f+frameW, jx, 1.2f-frameW, -1.0f-frameW, jx, 1.2f, -1.0f-frameW, jx, 1.2f, 1.0f+frameW)
    QUAD(jx, 1.2f-frameW, 1.0f, jx, 1.2f-frameW, 1.0f+frameW, jx, 2.5f+frameW, 1.0f+frameW, jx, 2.5f+frameW, 1.0f)
    QUAD(jx, 1.2f-frameW, -1.0f-frameW, jx, 1.2f-frameW, -1.0f, jx, 2.5f+frameW, -1.0f, jx, 2.5f+frameW, -1.0f-frameW)
    glColor3f(0.1f, 0.15f, 0.25f);
    QUAD(jx, 1.2f, -1.0f, jx, 1.2f, 1.0f, jx, 2.5f, 1.0f, jx, 2.5f, -1.0f)
    glColor3f(0.6f, 0.15f, 0.1f);
    QUAD(jx-0.01f, 1.8f, -1.0f, jx-0.01f, 1.8f, 1.0f, jx-0.01f, 1.9f, 1.0f, jx-0.01f, 1.9f, -1.0f)
    QUAD(jx-0.01f, 1.2f, -0.05f, jx-0.01f, 1.2f, 0.05f, jx-0.01f, 2.5f, 0.05f, jx-0.01f, 2.5f, -0.05f)
}
static void desenhaLampada() {
    float lx = 0.0f, ly = 3.8f, lz = 0.0f; // Centralizada no teto (altura 4.0)

    // bocal da lâmpada
    desenhaCaixaCor(lx, ly + 0.05f, lz, 0.4f, 0.1f, 0.4f, 0.1f, 0.1f, 0.1f);

    // lâmpada
    glPushMatrix();
        glTranslatef(lx, ly - 0.1f, lz);
        
        // Faz o material da lâmpada "emitir" luz visualmente
        float emissao[] = {1.0f, 1.0f, 0.8f, 1.0f}; // Amarelado
        glMaterialfv(GL_FRONT, GL_EMISSION, emissao);
        
        glColor3f(1.0f, 1.0f, 0.9f);
        glutSolidSphere(0.15f, 8, 8); // Low poly
        
        // Reset da emissão para não afetar outros objetos
        float semEmissao[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, semEmissao);
    glPopMatrix();
}
void configurarLuzInterna() {
    glEnable(GL_LIGHTING);   
    glEnable(GL_LIGHT0);    

    // O 4º parâmetro '1.0f' torna a luz uma point light
    float posicao[] = { 0.0f, 3.8f, 0.0f, 1.0f };
    
    // Cores da luz 
    float luzDifusa[] = { 1.0f, 0.9f, 0.7f, 1.0f }; 
    float luzAmbiente[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 

    glLightfv(GL_LIGHT0, GL_POSITION, posicao);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
}

void desenharInteriorExpandido(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    configurarLuzInterna();

    float tamanho = 5.0f; 
    float alturaS = 4.0f;

    glColor3f(0.3f, 0.2f, 0.1f); 
    QUAD(-tamanho, 0, -tamanho,  tamanho, 0, -tamanho,  tamanho, 0, tamanho, -tamanho, 0, tamanho)
    glColor3f(0.8f, 0.8f, 0.8f); 
    QUAD(-tamanho, alturaS, -tamanho,  tamanho, alturaS, -tamanho,  tamanho, alturaS, tamanho, -tamanho, alturaS, tamanho)

    glColor3f(0.6f, 0.6f, 0.7f);
    QUAD(-tamanho, 0, -tamanho,  tamanho, 0, -tamanho,  tamanho, alturaS, -tamanho, -tamanho, alturaS, -tamanho) 
    QUAD(tamanho, 0, -tamanho,  tamanho, 0, tamanho,  tamanho, alturaS, tamanho,  tamanho, alturaS, -tamanho) 
    QUAD(-tamanho, 0, tamanho, -tamanho, 0, -tamanho, -tamanho, alturaS, -tamanho, -tamanho, alturaS, tamanho) 
    
    QUAD(-tamanho, 0, tamanho, -1.0f, 0, tamanho, -1.0f, alturaS, tamanho, -tamanho, alturaS, tamanho)
    QUAD( 1.0f, 0, tamanho,  tamanho, 0, tamanho,  tamanho, alturaS, tamanho,  1.0f, alturaS, tamanho)
    QUAD(-1.0f, 2.5f, tamanho, 1.0f, 2.5f, tamanho, 1.0f, alturaS, tamanho, -1.0f, alturaS, tamanho)

    desenhaPortaInterior(tamanho); 
    desenhaJanelaInteriorDir(tamanho); 

    // --- RENDERIZAÇÃO DOS MÓVEIS ---
    desenhaLampada();
    desenhaCama();
    desenhaMesaCabeceira();
    desenhaArmario();
    desenhaMesaCadeira();

    glPopMatrix();
}