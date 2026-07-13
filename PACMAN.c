#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define APAGAR "\033[0J"//apaga a tela da posição do cursor para baixo
#define VOLTAR "\033[H"//volta para o inicio do terminal

#define AMARELO "\033[33m"

#define VERMELHO "\033[31m"
#define MAGENTA "\033[35m"
#define CIANO "\033[36m"
#define VERDE  "\033[32m"
#define AZUL "\033[34m" //para quando os fantasminhas ficam com medo

#define BRANCO "\033[37m"
#define CINZA "\033[90m"
#define RESET  "\033[0m"

#define LENGTH 30 //comprimento
#define HIGH 22 //altura
#define VEL_PAC 3
#define VEL_F 3
#define VEL_F_LENTO 6
#define TEMPO_VULNERAVEL 150
#define TROCAR 250


/*
1. Movimentação/Colisão do pacman
2. Comer os pontos
3. Movimentação dos fantasmas
4. Inteligência dos fantasmas

Sobre os estados dos fantasmas:

Perseguindo, espalhando: estados que se alteram ao longo do tempo como parte da estrategia do jogo
Assustados, comidos: só existem quando pacman come a bolinha especial
Obs: eles não conseguem ir para trás.

Por ordem de partida:

1. Blinky (Vermelho)
    I. Perseguição: se dirige na direção atual do pacman
    II. Dispersão: se dirige ao topo direito
    III. Após pacman comer determinado numero de bolinhas, blinky entra apenas no estado de perseguição

2. Pinky (Magenta)
    I. Perseguição: calcula a posição do pacman duas casas à frente
    *Obs: para cima, o alvo é 2 pra cima dois pra esquerda
    II. Dispersão: se dirige ao topo esquerdo

3. Inky (Ciano)
    *Só sai depois de pacman comer 30 bolinhas.
    I. Perseguição: A partir de um ponto à frente do pacman (ou um pra cima, e um pra esquerda, se estiver
    para cima, tal qual pinky), traça uma reta deste ponto até o blinky. O alvo do ciano entá nesta mesma
    reta, espelhado em relação ao ponto de referência
    II. Dispersão: se dirige para ao canto inferior direito

4. Clyde (No caso, Verde)
    I. Perseguição: somente enquanto estiver a 8 blocos de distancia de pacman. Senao, se dirige ao
    seu canto de dispersão.
    II. Dispersão: se dirige para o canto inferior esquerdo

Quando estão vulneráveis, se movimentam mais lento e de forma aleatória


Criar função para movimentação (limita em relação às paredes e processa túneis)


A movimentação é uma grande sequência de troca de direções.
O fantasma deve verificar suas opções e calcular a melhor rota, conforme seu algoritmo.
A rota deve ser recalculada ao fim da execução de cada direção
Recalculo: em cada cruzamento

Mas o que é um cruzamento?
Um ponto onde mais de uma direção está liberada (sem contar para trás)

Seguindo o alvo:
1. Calcular distância x e y a partir do alvo
2. Seguir a direção daquele cuja distância é menor. Se estiver bloqueado, ir pelo outro
3. Enquanto isso, verificando para cada posição se os lados estão liberados.
4. Quando estiver, recalcula novamente.

se tanto x quanto y favoraveis estiverem bloqueados, se afasta no eixo menor

_______|.
 .|
  |

dY = 1
dX = 5
*/

struct termios velho_terminal, novo_terminal;
void mudar_terminal(){
    tcgetattr(STDIN_FILENO, &velho_terminal);
    novo_terminal = velho_terminal;

    novo_terminal.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &novo_terminal);
}
void restaurar_terminal(){
    tcsetattr(STDIN_FILENO, TCSANOW, &velho_terminal);
}

char inicio[2][13][40]={
    {
        "+ - * - - * - - * - - * - - * - - * - +",
        "|   ___  ___  ___  _ _ _  ___  __ _   |",
        "|  |   ||   || __|| ' ' ||   ||  | |  |",
        "|  | .-'|   |||__ | | | ||   ||    |  |",
        "|  |_|  |_|_||___||_|_|_||_|_||_|__|  |",
        "|                                     |",
        "|                .----.               |",
        "|              /    O  _\\             |",
        "|             :     .-'               |",
        "|             '     '-_               |",
        "|              \\       '.             |",
        "|                '----'               |",
        "+ - - * - - * - - * - - * - - * - - * +"
    },
    {
        "+ * - - * - - * - - * - - * - - * - - +",
        "|   ___  ___  ___  _ _ _  ___  __ _   |",
        "|  |   ||   || __|| ' ' ||   ||  | |  |",
        "|  | .-'|   |||__ | | | ||   ||    |  |",
        "|  |_|  |_|_||___||_|_|_||_|_||_|__|  |",
        "|                                     |",
        "|                .----.               |",
        "|              /    O  _\\             |",
        "|             :     .-'               |",
        "|             '     '-_               |",
        "|              \\       '.             |",
        "|                '----'               |",
        "+ * - - * - - * - - * - - * - - * - - +"
    },
};

char mapa[HIGH][LENGTH+1]={//22x30
    ".+============..============+.",
    "|- - - - - - -II- - - - - - -|",
    "|oIIII-IIIIII-II-IIIIII-IIIIo|",
    "|-IIII-IIIIII-II-IIIIII-IIII-|",
    "|- - - - - - - - - - - - - - |",
    "|-IIII-II-IIIIIIIIII-II-IIII-|",
    "|- - - II- - -II- - -II- - - |",
    " ''''|-IIIIII-II-IIIIII-|'''' ",
    "     |-II- - - - - - II-|     ",
    "_____|-II-.===~~===.-II-|_____",
    "- - - - - |        |- - - - - ",
    "'''''|-II-'========'-II-|'''''",
    "     |-II- - - - - - II-|     ",
    " ____|-II-IIIIIIIIII-II-|____ ",
    "|- - - - - - -II- - - - - - -|",
    "|oIIII-IIIIII-II-IIIIII-IIIIo|",
    "|- -II- - - - - - - - - II- -|",
    "+==-II-II-IIIIIIIIII-II-II-==+",
    "|- - - II- - -II- - -II- - - |",
    "|-IIIIIIIIIII-II-IIIIIIIIIII-|",
    "|- - - - - - - - - - - - - - |",
    "'+==========================+'",
};

char cores[][9]={
    BRANCO,
    CINZA,
    AMARELO,
    VERDE,
    MAGENTA,
    CIANO,
    VERMELHO,
    AZUL
};

//
//Estruturas
typedef struct{
    int x, y;
}coordenada;

typedef struct{
    coordenada p;
    coordenada alvo;
    int cor;
    int modo;//1perseguindo, 2espalhando, 3vulneravel, 4comido, 5preso
    char simb;
    int direcao;//1a, 2w, 3d, 4s
    int velocidade;
}entidade;
entidade fantasma[4];

typedef struct{
    char simb;
    int cor;
    int tipo;//0parede, 1vazio, 2ponto, 3bolinha especial
}celula;
celula cel[HIGH][LENGTH];

//
//Algoritmos Simples
int inverter(int d){//retorna o numero correspondente à direção oposta
    if(d<=2)d+=2;
    else d-=2;
    return d;
}

int esta_no_quadrado(coordenada p){//1: esta no quadrado
    if((p.y == 10 || p.y == 9) && p.x>=11 && p.x<=18) return 1;
    else return 0;
}

void sair_do_quadrado(entidade *m){//algoritmo para sair do quadrado
    if(m->p.x < 14) m->p.x++;
    else if(m->p.x > 15) m->p.x--;
    else m->p.y--;
}

void equacao_reta(coordenada a, coordenada b, coordenada *novo){//blinky, pacman, mira
    //forma f(x) = mx + n
    float ax = a.x, bx = b.x, cx;
    float ay = a.y, by = b.y, cy;

    cx = 2*bx - ax;

    if(bx-ax<0.001){
        cy = 2*by - ay;
    }
    else{
        float m = (by - ay)/(bx - ax);
        float n = ay - ax*m;
        cy = m*cx + n;
    }
    cx = round(cx); cy = round(cy);

    novo->y = cy;
    novo->x = cx;
}

//
//Movimentação
void andar(entidade *mob, int direcao){//anda e verifica paredes somente
    int x = mob->p.x;
    int y = mob->p.y;
    mob->direcao = direcao;
    switch(direcao){
        case 1://esquerda
            if(x-1 < 0) mob->p.x = LENGTH-1;
            else if(cel[y][x-1].tipo) mob->p.x--; 
            break;
        case 2: //cima
            if(cel[y-1][x].simb == '~' || cel[y-1][x].tipo) mob->p.y--; break;
        case 3: //direita
            if(x+1 > LENGTH-1) mob->p.x = 0;
            else if(cel[y][x+1].tipo) mob->p.x++; break;
        case 4: //baixo
            if(cel[y+1][x].tipo || (mob->modo==4 && cel[y+1][x].simb=='~')) mob->p.y++; break;
    }
}

void andar_alvo(entidade *mob, coordenada alvo){//designa a proxima direção que o fantasma deve seguir
    
    int d[5]={0};//direções livres. 1=esquerda(a)
    int x = mob->p.x, y = mob->p.y;
    int achou=0;

    //A ponte das bordas
    if(x==0){mob->p.x = LENGTH-2; return;}
    else if(x==LENGTH-1){mob->p.x=1; return;}

    //Entrar no quadrado
    if(mob->modo == 4 && cel[y+1][x].simb=='~'){
        andar(mob, 4); return;
    }

    //checa arredores
    if(cel[y][x-1].tipo) d[1]=1;
    if(cel[y-1][x].tipo) d[2]=1;
    if(cel[y][x+1].tipo) d[3]=1;
    if(cel[y+1][x].tipo) d[4]=1;
    
    //desabilita para trás
    int k = inverter(mob->direcao);
    d[k]=0;

    //calculo das distâncias
    float distancia, menor=100000;
    int indice=2;
    for(int i=4; i>0; i--){
        if(d[i]){
            float ax=x, ay=y;
            switch(i){
                case 1: ax--; break;
                case 2: ay--; break;
                case 3: ax++; break;
                case 4: ay++; break;
            }
            float distX = ax - alvo.x;
            float distY = ay - alvo.y;

            distancia = sqrt(distX*distX + distY*distY);
            if(distancia < menor){ menor = distancia; indice = i;}
        }
    }
    andar(mob, indice);
}

void alvo(coordenada *p, entidade alvo, int tipo){//calcula as coordenadas do alvo, com base na personalidade do fantasma
    int aX = alvo.p.x;
    int aY = alvo.p.y;
    int d = alvo.direcao;

    switch(tipo){
        case 1://a mira é o próprio alvo
            p->x = aX; p->y = aY; break;
        case 2://a mira é dois espaços à frente
            switch(d){
                case 1: p->x = aX-2; p->y = aY; break;
                case 2: p->x = aX; p->y = aY-2; break;
                case 3: p->x = aX+2; p->y = aY; break;
                case 4: p->x = aX; p->y = aY+2; break;
            }
            break;
        case 3://aquele esquema estranho
            coordenada referencia;
            referencia.x = aX; referencia.y = aY;

            switch(d){
                case 1: referencia.x--; break;
                case 2: referencia.y--; break;
                case 3: referencia.x++; break;
                case 4: referencia.y++; break;
            }
            
            equacao_reta(fantasma[0].p, referencia, p);
            
            break;
        case 4://a mira é o próprio alvo
            p->x = aX; p->y = aY; break;
    }
}

//
//Inicialização
void tela_inicio(){
    int n=0;
    char c = 0;
    do{
        printf(VOLTAR APAGAR);

        for(int i=0; i<13; i++) printf(AMARELO"%s\n"RESET, inicio[n][i]);

        printf("\nPressione qualquer tecla para comecar.\n");
        
        if(read(STDIN_FILENO, &c, 1) > 0)break;

        usleep(500000);
        n = !n;
    }while(1);
}

void inicialização_mapa(){
    for(int y=0; y<HIGH; y++){
        for(int x=0; x<LENGTH; x++){

            switch(mapa[y][x]){
                case '-': //pontos
                    cel[y][x].cor = 0;
                    cel[y][x].tipo = 2;
                    break;
                case 'o': //bolinhas especiais
                    cel[y][x].cor = 0;
                    cel[y][x].tipo = 3;
                    break;
                case ' ': //vazio
                    cel[y][x].cor=0;
                    cel[y][x].tipo=1;
                    break;
                default: //parede
                    cel[y][x].cor = 1;
                    cel[y][x].tipo = 0;
                    break;
            }

            cel[y][x].simb = mapa[y][x];
        }
    }

}

void inicializacao_entidades(entidade *pacman){

    pacman->p.x = 14;
    pacman->p.y = 12;
    pacman->simb = '@';
    pacman->cor = 2;
    pacman->direcao = 1;

    for(int i=0; i<4; i++){
        fantasma[i].p.x = 10 + i*2;
        fantasma[i].p.y = 10;
        fantasma[i].modo = 5;
        fantasma[i].simb = '@';
        fantasma[i].direcao = 1;
        fantasma[i].velocidade = VEL_F;
    }
    fantasma[0].cor = 6; fantasma[0].p.x = 14; fantasma[0].p.y = 8; fantasma[0].modo = 1;
    fantasma[1].cor = 4; 
    fantasma[2].cor = 5;
    fantasma[3].cor = 3;
}

//
//Main
int main(){
    mudar_terminal();
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    tela_inicio();
    system("clear");
    
    entidade pacman;
    inicialização_mapa();
    inicializacao_entidades(&pacman);

    char comando = 'a';
    int frames=0, pontos=0, vulneravel=0, count_v=TEMPO_VULNERAVEL, trocar_modos = TROCAR, modo=0;
    int cooldown_p = VEL_PAC;
    int liberta1=1, liberta2=1, liberta3=1;
    int perdeu=0, vitoria=1, vidas=3, reiniciar=1;

    /*
    Sequência lógica:
    0. Cooldowns, libertações
    1. Colisões (entre entidades, comer bolinhas)
    2. Mudar modos
    3. Definir alvos
    4. Movimentar
    5. Imprimir
    */

    do{

        //0. COOLDOWNS E LIBERAR FANTASMAS
            printf(VOLTAR); usleep(100000);
            cooldown_p--; trocar_modos--;
            vitoria=1;
            if(vulneravel) count_v--;

            //Condição para libertar fantasmas
            if(frames >= 50 && liberta1){ fantasma[1].modo=1; liberta1=0;}
            if(pontos >= 30 && liberta2){ fantasma[2].modo=1; liberta2=0;}
            if(pontos >= 80 && liberta3){ fantasma[3].modo=1; liberta3=0;}

        //1. COLISÕES

            //Comer bolinhas, iniciar Estado de Vulnerabilidade
            if(cel[pacman.p.y][pacman.p.x].tipo == 2){
                pontos++;
                cel[pacman.p.y][pacman.p.x].tipo=1;
                cel[pacman.p.y][pacman.p.x].simb=' ';
            }
            else if(cel[pacman.p.y][pacman.p.x].tipo == 3){
                vulneravel=1;
                cel[pacman.p.y][pacman.p.x].tipo=1;
                cel[pacman.p.y][pacman.p.x].simb=' ';

                //o modo deles vira assustado somente naquele momento
                for(int i=0; i<4; i++) 
                    if(fantasma[i].modo < 3) fantasma[i].modo=3;
            }

            //Colisão fantasma-pacman, Cooldown da velocidade dos fantasmas 
            //Alternar modos 1 e 2
            for(int i=0; i<4; i++){
                entidade *f = &fantasma[i];

                //Velocidade de cada fantasma
                f->velocidade--;

                //Se o pacman comeu um fantasma, ou vice versa
                if(f->p.x == pacman.p.x && f->p.y == pacman.p.y){
                    if(f->modo == 3){
                        pontos += 200;
                        f->modo = 4;
                        f->simb = '*';
                    }
                    else if(f->modo < 3){
                        perdeu=1; break;
                    }
                }

                //Alternar modos
                if(f->modo <= 2) fantasma[i].modo = modo+1;

                //Se foi comido, mas já voltou para o quadrado (no seu "alvo"), resetar o estado
                if(f->modo == 4 && f->p.x == f->alvo.x && f->p.y == f->alvo.y){ 
                    f->modo = 1; f->simb='@';
                }
            }

        //2. DEFINIR MODOS

            //Se acabou o estado de vulnerabilidade
            if(count_v<=0){
                vulneravel=0; count_v = TEMPO_VULNERAVEL;
                for(int i=0; i<4; i++){ fantasma[i].modo=1; fantasma[i].simb='@';}
            }

            //Alternar entre perseguição e espalhar
            if(!trocar_modos){ modo = !modo; trocar_modos = TROCAR;}

            //do fantasma 4-----------------------------
            if(fantasma[3].modo<3){
                float ax = (float)fantasma[3].p.x - pacman.p.x;
                float ay = (float)fantasma[3].p.y - pacman.p.y;
                float distancia = sqrt(ax*ax + ay*ay);

                if(distancia<8)fantasma[3].modo = 2;
                else fantasma[3].modo = 1;
            }

        //3. DEFINIR ALVOS
            coordenada mira;
            for(int i=0; i<4; i++) if(fantasma[i].velocidade <= 0){
                coordenada *v = &fantasma[i].alvo;

                //Definir alvos
                switch(fantasma[i].modo){
                    case 1: //a mira é o proprio pacman
                        alvo(v, pacman, i+1); 
                        break; 
                    case 2: //dispersao para os cantos
                        switch(i){
                            case 0: v->x = LENGTH; v->y = 0; break;
                            case 1: v->x = 0; v->y = 0; break;
                            case 2: v->x = LENGTH; v->y = HIGH; break;
                            case 3: v->x = 1; v->y = HIGH-1; break;
                        }
                        break;
                    case 3: //movimento aleatorio
                        v->x = rand()%LENGTH; v->y = rand()%HIGH ;
                        break; 
                    case 4: 
                        v->x = 11 + i*2; v->y = 10; break;
                        break;
                }                    
            }
        
        //4. MOVIMENTAÇÃO
            
            //Dos fantasmas
            for(int i=0; i<4; i++) if(fantasma[i].velocidade <= 0){
                entidade *f = &fantasma[i];

                if(f->modo!=5){
                    if(esta_no_quadrado(f->p) && f->modo==1) sair_do_quadrado(f);
                    else andar_alvo(f, f->alvo);
                }

                //Resetar cooldown
                 if(f->modo==3) f->velocidade = VEL_F_LENTO;
                else if(f->modo==4) f->velocidade = VEL_F/3;
                else f->velocidade = VEL_F;
            }

            //Do pacman
            if(read(STDIN_FILENO, &comando, 1) > 0){
                int x = pacman.p.x;
                int y = pacman.p.y;

                switch(comando){
                    //sempre verifica se não é parede
                    case 'a': 
                        if(cel[y][x-1].tipo){ pacman.direcao = 1; pacman.simb = '<';} break;
                    case 'w': 
                        if(cel[y-1][x].tipo){ pacman.direcao = 2; pacman.simb = 'A';} break;
                    case 'd': 
                        if(cel[y][x+1].tipo){ pacman.direcao = 3; pacman.simb = '>';} break;
                    case 's': 
                        if(cel[y+1][x].tipo){ pacman.direcao = 4; pacman.simb = 'V';} break;
                }
            }
            if(!cooldown_p){
                andar(&pacman, pacman.direcao);
                cooldown_p = VEL_PAC;
            }

        //calcular vitoria
        for(int y=0; y<HIGH; y++) {
            for(int x=0; x<LENGTH; x++)
                if(cel[y][x].tipo > 1){ vitoria=0; break;
            }
            if(!vitoria)break;
        }

        if(perdeu && vidas){
            usleep(1000000);
            reiniciar=1; vidas--;
        }

        //5. IMPRESSAO
            for(int y=0; y<HIGH; y++){
                for(int x=0; x<LENGTH; x++){
                    int imp=0;
                    
                    //os fantasmas e suas cores, de acordo com o modo
                    if(!vitoria) for(int i=0; i<4; i++){
                        if(fantasma[i].p.x == x && fantasma[i].p.y == y){
                            int a = fantasma[i].cor;
                            switch(fantasma[i].modo){
                                case 3: a=7; break;
                                case 4: a=1; break;
                            }
                            if(perdeu) a=1;
                            printf("%s%c", cores[a], fantasma[i].simb);
                            imp=1; break;
                        }
                    }
                    //outros
                    if(!imp){
                        int a = cel[y][x].cor;
                        if(perdeu) a=1;
                        if(pacman.p.x == x && pacman.p.y ==y)//pacman
                            printf("%s%c", cores[pacman.cor], pacman.simb);
                        else //resto
                            printf("%s%c"RESET, cores[a], cel[y][x].simb);
                    }
                }
                printf("\n");
            }
            printf("\nPontuacao: %d   \n", pontos);
            //printf("Frames: %d\n", frames);
            for(int i=0; i<vidas; i++) printf(AMARELO"@ "RESET);
            printf("         \n");
        
        frames++;

        if(!vidas || vitoria){ usleep(1000000); break;}

        if(reiniciar==2){
            usleep(1000000);
            pacman.simb='<';
            pacman.p.x++;
            reiniciar=0;
        }

        if(reiniciar){
            inicializacao_entidades(&pacman);
            usleep(1000000);
            reiniciar++;
            perdeu=0;
        }
        
    }while(1);

    if(!vidas) printf("Voce perdeu :(\n");
    if(vitoria) printf("+ Voce ganhou! +\n");

    return 0;
}