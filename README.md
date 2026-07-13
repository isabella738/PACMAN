# PACMAN
Uma recriação com base nas mecânicas clássicas do jogo.  
Pacman é um jogo de labirinto cujo objetivo é comer todas as bolinhas espalhadas pelo mapa, sem ser morto pelos fantasmas. Cada fantasma possui uma estratégia distinta para encurralar o pacman, e se locomovem pelo mapa conforme sua própria "personalidade".  

Este código é uma versão simplificada. Diferente do clássico, este possui uma fase somente. Sujeito a bugs.  
## Sobre os personagens  
O Pacman possui 3 vidas. Pode se locomover para todos os lados, se limitando somente às paredes do mapa. Ganha +1 ponto ao comer as bolinhas simples. Ganha a habilidade de comer os fantasmas ao comer a bolinha especial. Comer um fantasma garante mais 100 pontos.  

Os fantasmas não podem andar para trás. Eles percorrem o mapa em direção a um alvo determinado por seu estado atual. Os fantasmas podem alternar entre 5 estados distintos:  
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
III. É liberado logo nos primeiros segundos do jogo.  

### 3. Inky (Ciano)
I. Perseguição: a partir de um ponto à frente do Pacman, calcula-se uma reta ligando a posição de Blinky até este ponto. O alvo de Inky está nesta reta, à mesma distância de Blinky ao ponto, mas na direção oposta.  
II Dispersão: o alvo é o canto inferior direito.  
III. É liberado após certa quantidade de pontos coletados.  

### 4. Clyde (Verde)
I. Perseguição: o alvo é o Pacman, somente enquanto estiver a 8 unidades de  distância dele. Menor que isso, ele entra no modo de dispersão.  
II Dispersão: o alvo é o canto inferior esquerdo.  
III. É liberado após uma quantidade grande de pontos.  

## Sobre o código

### Estruturas
Os personagens são formados por uma estrutura chamada "entidade". Uma entidade contém:  
I. Coordenada própria  
II. Coordenada do alvo  
III. Cor  
IV. Modo/Estado  
V. Caractere  
VI. Direcionamento (1: esquerda, 2: cima, 3: direita, 4: baixo)  
VII. Velocidade (representa o cooldown que permite a movimentação)    
O Pacman utiliza esta mesma estrutura, apesar de não usar todos os elementos.   

Cada caractere do mapa também é organizado numa estrutura, denominada "célula". Cada célula contém:  
I. Caractere  
II. Tipo (0: parede, 1: vazio, 2: ponto, 3: bolinhas especiais)   
III. Cor (paredes tem cor cinza para destacar dos pontos e bolinhas)  

### Organização  

O código é organizado em uma sequência de passos lógicos:  
1. Inicialização de variáveis, redução de cooldowns e checagem da condição de liberação dos fantasmas.  
2. Verificação de colisões entre entidades ou a ação de comer as bolinhas  
3. Definição do estado dos fantasmas
4. Definição do alvo dos fantasmas
5. Movimentação das entidades
6. Impressão

### Movimentação
As entidades se movimentam sempre que seu próprio cronômetro interno, o cooldown, chega a 0. Elas conseguem andar para qualquer célula que não tenha sido declarada como parede. A única exceção são os fantasmas quando estão no estado 4 (comidos) e dentro do quadrado central. Estes conseguem atravessar o caractere '~' para entrar e sair do centro.  

A cada movimentação, os fantasmas calculam a quantidade de caminhos que estão disponíveis ao redor dele, desconsiderando a direção para trás. Se existe somente 1, este é escolhido. Se existem mais, é escolhido aquele cuja distância do alvo é menor (por teorema de pitágoras). 
