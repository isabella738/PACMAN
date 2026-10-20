# PACMAN
Projeto não concluído. 
Uma recriação com base nas mecânicas clássicas do jogo.  
Pacman é um jogo de labirinto cujo objetivo é comer todas as bolinhas espalhadas pelo mapa, sem ser morto pelos fantasmas. Cada fantasma possui uma estratégia distinta para encurralar o pacman, e se locomovem pelo mapa conforme sua própria "personalidade".  
Para executar: 
1. baixar o arquivo do projeto 
2. abrir a pasta no terminal (cd Documentos/PACMAN) 
3. copiar e colar: make && ./PACMAN 

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

### Lógica
Os fantasmas tomam decisões baseadas no seu estado/modo vigente.
Os fantasmas mudam de estado conforme duas variáveis:
- modo_global: assume valor 0 ou 1 para indicar se os fantasmas estão perseguindo ou se dispersando. É declarada no jogo, externa aos fantasmas. Quando seu valor é 1, todos os fantasmas ao mesmo tempo estão perseguindo, se não, todos estão se dispersando. 
- modo interno: cada fantasma tem uma variavel própria que se sobrepõe ao modo_global, que diz respeito ao seu estado quando estão assustados(1), comidos(2) ou presos(3). Quando o valor é 0, são regidos pelo modo_global.
Desta forma, todos os fantasmas se mantém sincronizados. 

Os fantasmas se movimentam segundo o seguinte algoritmo: definir alvo -> definir direção -> mudar posição. 
Eles fazem somente perseguir um alvo pelo jogo inteiro. Este alvo é determinado pelo modo vigente. A partir deste alvo, calcula-se os caminhos livres e é escolhido aquele cuja distância até o alvo é a mais curta. Com este caminho, define-se uma direção (0 = cima, 1 = direita, 2 = baixo, 3 = esquerda), e então, se movimenta para aquela direção. 
Quando os fantasmas estão assustados, o alvo é definido de forma aleatória a cada movimentação. Quando estão comidos ou presos, seguem um algoritmo para que consigam entrar/sair no quadrado central. Quando estão perseguindo ou dispersando, seguem as regras ditas no tópico anterior. 

O pacman segue regras semelhantes, porém mais simples. Não possui um alvo, somente a direção, que é determinada pelo comando no teclado.

As entidades se movimentam sempre que seu próprio cronômetro interno, o cooldown, chega a 0. Elas conseguem andar para qualquer célula que não tenha sido declarada como parede. A única exceção são os fantasmas quando estão no estado 3 (comidos) e dentro do quadrado central. Estes tem as barreiras temporariamente desativadas para conseguir atravessar a "porta". 

### Estruturas
Os objetos vêm de uma classe base denominada Entidade que contem elementos comuns a todos os personagens, como posição própria, caractere, cor, andar, etc. É desta classe que vem o objeto Pacman. 
Os fantasmas vêm da classe derivada Fantasmas, com elementos específicos para este tipo de entidade. 

Cada caractere do mapa também é organizado numa classe. Cada célula contém:  
I. Caractere  
II. Tipo (0: parede, 1: vazio, 2: ponto, 3: bolinhas especiais)   
III. Cor (paredes tem cor cinza para destacar dos pontos e bolinhas)  
