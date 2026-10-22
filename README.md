# PACMAN
Uma recriação com base nas mecânicas clássicas do jogo.  
Pacman é um jogo de labirinto cujo objetivo é comer todas as bolinhas espalhadas pelo mapa, sem ser morto pelos fantasmas. Cada fantasma possui uma estratégia distinta para encurralar o pacman, e se locomovem pelo mapa conforme sua própria "personalidade".  

Para executar: 
1. baixar o arquivo do projeto 
2. abrir a pasta no terminal 
3. digitar ./PACMAN no Linux ou abrir o executável no Windows  

Este código é uma versão simplificada. Diferente do clássico, este possui uma fase somente. 

## Sobre os personagens  
O Pacman possui 3 vidas. Pode se locomover para todos os lados, se limitando somente às paredes do mapa. Ganha +10 pontos ao comer as bolinhas simples. Ganha a habilidade de comer os fantasmas ao comer a pilula especial e +100 pontos. Comer um fantasma garante mais 200 pontos, e este número é dobrado a cada fantasma comido pela ação da mesma pilula.  

Fantasmas tomam decisões baseadas no seu estado vigente. Podem alternar entre 5 estados distintos:  
1. Perseguição: se dirige ao alvo de ataque
2. Dispersão: se dirige ao canto de refúgio do mapa
3. Assustados: movimentação aleatória e mais lenta
4. Comidos: voltam para o quadrado central
5. Presos: inicio do jogo, quando a condição de liberação ainda não foi cumprida.

Os estados de perseguição e dispersão se alternam ao longo do tempo. Os outros são determinados por outras condições.  

## Sobre os Fantasmas
Cada fantasma se move pelo mapa de acordo com sua própria personalidade. São eles:  

### 1. Blinky (Vermelho) 
I. Perseguição: o alvo é o próprio Pacman.  
II. Disperção: o alvo é o canto superior direito.  
III. Blinky sempre começa o jogo fora do quadrado central.  

### 2. Pinky (Rosa/Magenta)
I. Perseguição: o alvo é duas casas à frente do Pacman.  
II Dispersão: o alvo é o canto superior esquerdo.  
III. É liberado após comer 7 bolinhas.  

### 3. Inky (Ciano)
I. Perseguição: a partir de um ponto à frente do Pacman, calcula-se uma reta ligando a posição de Blinky até este ponto. O alvo de Inky está nesta reta, à mesma distância de Blinky ao ponto, mas na direção oposta.  
II Dispersão: o alvo é o canto inferior direito.  
III. É liberado após comer 17 bolinhas.  

### 4. Clyde (Verde)
I. Perseguição: o alvo é o Pacman, somente enquanto estiver a 8 unidades de  distância dele. Menor que isso, ele entra no modo de dispersão.  
II Dispersão: o alvo é o canto inferior esquerdo.  
III. É liberado após comer 32 bolinhas.

A cada reinicio de fase, o contador de bolinhas (ou dots) é reiniciado também.  

## Sobre o código

### Conceitos Fundamentais
Quaisquer ações no jogo possuem um cooldown para serem realizadas. Este cooldown se refere à uma certa quantidade de frames, ou seja, de iterações do laço principal na função main. Existem cooldowns para movimentação dos personagens, para o tempo em que os fantasmas permanecem assustados e para alternar entre os modos de perseguição e dispersão. Por exemplo, se o cooldown de velocidade do Pacman é 4, ele irá mudar de posição após 4 frames.  
Em outras palavras, cada ação é feita após FPS/COOLDOWN segundos. FPS é uma macro definida em settings.hpp assim como todos os cooldowns.  

### A Inteligência das Entidades
Os fantasmas fazem apenas perseguir um alvo o jogo inteiro. O que muda é para onde este alvo aponta, que depende do estado vigente do fantasma.
Eles mudam de estado conforme duas variáveis:
- modo_global: assume valor 0 ou 1 para indicar se os fantasmas estão perseguindo ou se dispersando. É declarada no jogo, externa aos fantasmas. Quando seu valor é 1, todos os fantasmas ao mesmo tempo estão perseguindo, se não, todos estão se dispersando. 
- modo interno: cada fantasma tem uma variavel própria que se sobrepõe ao modo_global, que diz respeito ao seu estado quando estão assustados(1), comidos(2) ou presos(3). Quando o valor é 0, são regidos pelo modo_global.

Desta forma, todos os fantasmas se mantém sincronizados.

Uma vez definido o alvo, calcula-se o caminho mais curto para ele, por teorema de pitágoras. Este calculo define uma direção (0=cima, 1=direita, 2=baixo, 3=esquerda), que, por fim, muda a posição do personagem.  
Portanto, existe o seguinte algoritmo: definir alvo -> definir direção -> mudar posição. 

O pacman segue regras semelhantes, porém mais simples. Não possui um alvo, somente a direção, que é determinada pelo comando no teclado.

### Estruturas
Os objetos vêm de uma classe base denominada Entidade que contem elementos comuns a todos os personagens, como posição própria, caractere, cor, andar, etc. É desta classe que vem o objeto Pacman. 
Os fantasmas vêm da classe derivada Fantasmas, com elementos específicos para este tipo de entidade. 

Cada caractere do mapa também é organizado numa classe. Cada célula contém:  
I. Caractere  
II. Tipo (0: parede, 1: vazio, 2: ponto, 3: bolinhas especiais)   
III. Cor (paredes tem cor cinza para destacar dos pontos e bolinhas)  
