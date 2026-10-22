/*
    Definiçao de macros e structs
    Incluir este arquivo em outro é suficiente para que ele reconheça todos estes elementos.
*/

#include <functional>
#include <cmath>

#ifndef SETTINGS
#define SETTINGS

#define VOLTAR "\033[H"//volta para o inicio do terminal
#define ALT 22
#define LARG 30
#define SEGUNDOS 1000 //1000 milissegundos é 1 segundo

/*
    A tela atualiza FPS vezes por segundo. 
    Os valores abaixo se referem a uma ação que é realizada a cada X frames. Em outras palavras, tal 
    ação é realizada a cada X/FPS segundos.
*/
#define FPS 10
#define VEL_PAC 3
#define VEL_F 4
#define TEMPO_ASSUSTADO 100
#define TROCAR_MODO 100

//Pontos
#define P_DOTS 10
#define P_POWER 50
#define P_COMER 200 //aumenta o dobro a cada fantasma comido pelo efeito da mesma pilula

//Cores
#define AMARELO "\033[33m"
#define VERMELHO "\033[31m"
#define MAGENTA "\033[35m"
#define CIANO "\033[36m"
#define VERDE  "\033[32m"
#define AZUL "\033[34m"
#define BRANCO "\033[37m"
#define CINZA "\033[90m"
#define RESET  "\033[0m"


struct Coordenada {
    int y, x;

    bool operator==(const Coordenada& outro) const {
        return y == outro.y && x == outro.x;
    }
    Coordenada operator+(const Coordenada& outro) const {
        return {y + outro.y, x + outro.x};
    }
    Coordenada operator*(int n) const {
        return {y * n, x * n};
    }
};

extern int pontos, modo_global, cd_assustados, cd_modo, vida, vitoria;
extern int contador_dots, contador_f;

extern char cores[][9], simb[4], inicio[2][13][40];
extern Coordenada direcao[4];

#endif