# 🌿 Projeto Final: Simulador de Jardim 3D (OpenGL)

Este projeto consiste em um ambiente simulado 3D interativo, inspirado na estética de *Animal Crossing*, onde o usuário pode interagir com um jardim e observar a transição cíclica das quatro estações do ano. O foco principal é a simulação de uma planta que cresce ou regride conforme o cuidado do jogador (rega) e a passagem do tempo (dormir).

## 🚀 Visão Geral do Sistema
O código gerencia um mundo fechado com renderização em tempo real, integrando:
* Sistema de Estações: Alteração dinâmica de cores do céu, chão e tipos de partículas (neve, pétalas, folhas).
    * **Primavera:** Florescimento intenso.
    * **Verão:** Cores vibrantes e sol alto.
    * **Outono:** Folhas amarelas e partículas caindo.
    * **Inverno:** Acúmulo de neve e mudança na paleta de cores.
* Interior Explorável: Uma mecânica de teletransporte para uma sala subterrânea detalhada com móveis e colisões.
* Cinemática de Transição: Uma câmera automatizada que executa um movimento em espiral paramétrica para suavizar a troca de dias.
* HUD Simples: Interface 2D que exibe comandos contextuais baseados na proximidade do jogador com objetos.

## 📸 Demonstração
Ambiente 3D nas diferentes estações:
<table>
  <tr>
    <td align="center">
      <img src="https://github.com/emmano2997/Plant-Crossing/img/verao.png" width="300"/><br/>
      <sub><b>Figure 1:</b>Verão</sub>
    </td>
    <td align="center">
      <img src="https://github.com/emmano2997/Plant-Crossing/img/outono.png" width="300"/><br/>
      <sub><b>Figure 2:</b>Outono</sub>
    </td>
    <td align="center">
      <img src="https://github.com/emmano2997/Plant-Crossing/img/inverno.png" width="300"/><br/>
      <sub><b>Figure 3:</b>Inverno</sub>
    </td>
        <td align="center">
      <img src="https://github.com/emmano2997/Plant-Crossing/img/primavera.png" width="300"/><br/>
      <sub><b>Figure 3:</b>Primavera</sub>
    </td>
  </tr>
</table>

## 🛠️ Atividades Práticas Implementadas
O projeto aplica os conceitos fundamentais da disciplina:
 * **Visualização 3D:** Uso de gluPerspective para projeção e gluLookAt para o controle de câmera em primeira pessoa (FPS) e modo livre.
 * **Transformações e Visibilidade:** Emprego intensivo de pilhas de matrizes (glPushMatrix/glPopMatrix) para posicionar móveis e elementos do cenário, além de um sistema de colisão baseado em AABB (Axis-Aligned Bounding Boxes).
 * **Iluminação e Sombreamento:** Implementação do modelo de iluminação de Phong utilizando as funcionalidades do OpenGL (glLight, glMaterial), incluindo componentes ambiente, difusa e especular para aumentar o realismo da cena, além de configuração de fontes de luz posicionais.
 * **Mapeamento de Textura:** Aplicação de texturas em objetos do cenário por meio de coordenadas UV, utilizando glTexCoord e carregamento de imagens para enriquecer visualmente superfícies como paredes, chão e móveis.
* **Curvas Paramétricas:** Utilização de equações de espiral (Espiral de Arquimedes) para o movimento suave da câmera durante a animação de "dormir".

## 🛠️ Como Compilar e Executar
### Dependências
Para compilar este projeto no Ubuntu (ou distribuições baseadas em Debian), você precisará do compilador C e das bibliotecas de desenvolvimento do OpenGL/GLUT:
* **build-essential:** Inclui o compilador gcc, o make e as bibliotecas padrão (stdio.h).
**freeglut3-dev:** Fornece o cabeçalho GL/glut.h e a biblioteca GLUT.
**libglu1-mesa-dev:** Necessária para as funções de utilidade do OpenGL (GLU).

Instale tudo com o comando:
```bash
sudo apt-get update
sudo apt-get install build-essential freeglut3-dev libglu1-mesa-dev
```

### Instruções de Compilação
Como o projeto utiliza um Makefile para gerenciar as dependências de compilação e a ligação (linking) das bibliotecas matemáticas (-lm) e gráficas (-lGL -lGLU -lglut), o processo é simplificado:

* 1 - Limpar arquivos de compilação antigos:
```bash
make clean
```

* 2 - Compilar  e executar o programa:
```bash
make run
```
## ⚠️ Desafios e Soluções

* **Z-Fighting:** Encontrado em molduras de janelas e vigas de madeira. Resolvido aplicando pequenos offsets (deslocamentos de 0.01f) nas coordenadas de renderização.

* **Near Clipping:** A câmera atravessava móveis antes de colidir. Ajustado diminuindo o plano de corte próximo no gluPerspective para 0.1f.

* **Texturas sem iluminação:** Ao aplicar texturas, os objetos ficavam com cor flat, ignorando a luz. Resolvido configurando `glTexEnvf` com `GL_MODULATE` e setando `glColor3f(1,1,1)` nas superfícies texturizadas para que a cor do material não interferisse na textura.

* **Superfícies sem volume:** Mesmo com iluminação ativa, as superfícies desenhadas manualmente (quads) não reagiam à luz. O problema era a ausência de `glNormal3f` — sem normais definidas, o OpenGL não consegue calcular o ângulo de incidência da luz. Resolvido adicionando normais em todas as superfícies principais.

## 📈 Melhorias Futuras

* **Sombras Dinâmicas:** Implementar shadow mapping para sombras projetadas pela árvore e casa.

* **Shaders (GLSL):** Transicionar do pipeline fixo para o programável para efeitos de vento mais realistas na vegetação.

* **Sistema de Áudio:** Adicionar sons ambientes para cada estação (vento, pássaros, chuva).

* **Interface de Menu:** Criação de um menu principal e telas de pause utilizando projeção ortográfica 2D, permitindo configurar volume, sensibilidade da câmera e reiniciar o progresso da planta.

* **Gamificação e Atividades:** Implementação de novas mecânicas como colheita de frutos (no estágio maduro), remoção de ervas daninhas e um sistema de inventário básico.

* **Objetos Sazonais Responsivos:** Adição de mais elementos que reagem ao clima, como um boneco de neve que derrete na primavera, poças de água que aparecem após chuvas de verão ou um balanço que oscila mais forte durante ventanias de outono.

## 👥 Equipe e Contribuições
A divisão de tarefas foi estruturada da seguinte forma:

### Emmanuel Aprígio Cavalcanti dos Santos
Responsável pela Ambientação e Lógica Sazonal.

* Desenvolveu a modelagem do mundo exterior e a lógica de variação ambiental para cada uma das quatro estações.

* Implementou o sistema de crescimento da árvore (estágios de vida) e a rotina de transição de tempo.

* Projetou o sistema de iluminação dinâmica, onde a posição e o comportamento da luz do sol são sincronizados com o estado do mundo.

### Kaio César de Oliveira Barreto
Responsável pela Arquitetura, Cinemática e Física.

* Projetou e modelou a casa externa e todo o seu ambiente interno detalhado.

* Implementou o sistema multicâmera (FPS e modo livre) e a animação paramétrica em espiral executada ao dormir.

* Implementou as colisões do jogo e a lógica de zonas de interação, garantindo a integridade física do jogador no cenário.

* Criou a camada de interface (HUD 2D) para fornecer feedback contextual ao usuário.

### Ralf Dewrich Ferreira
Responsável pelo Realismo Visual (Sombreamento e Texturização).

* Implementou o modelo de sombreamento suave (Gouraud Shading) e configurou as propriedades ópticas dos materiais (luz ambiente, difusa e especular) para simular diferentes brilhos e superfícies.

* Aplicou a técnica de projeção de imagens 2D em modelos 3D utilizando coordenadas UV para garantir o posicionamento preciso das texturas em cada objeto.

* Utilizou Mipmapping e filtros lineares para reduzir artefatos visuais e otimizar o desempenho, além de definir os vetores normais das superfícies para o cálculo correto da reflexão da luz.

## 🎮 Controles:
* **W, A, S, D:** Movimentação.

* **C:** Alternar modo de câmera (Livre/Jogador).

* **Z:** Interagir com portas (Entrar/Sair).

* **R:** Regar a planta.

* **F:** Dormir na cama (Passar estação).

* **Q:** Reiniciar.