/*
    Definiçao de macros, structs e alguns vetores fundamentais
*/

#include <functional>
#include <cmath>

#ifndef SETTINGS
#define SETTINGS

#if defined(_WIN32) || defined(_WIN64)
    #define WINDOWS
#elif defined(__linux__)
    #define LINUX
#endif

#define VOLTAR "\033[H"//volta para o inicio do terminal

#define ALT 22
#define LARG 30
#define SEGUNDOS 1000 //1000 milissegundos é 1 segundo

#define FPS 4
#define VEL_PAC 2
#define VEL_F 3
#define TEMPO_ASSUSTADO 10
#define TROCAR_MODO 10

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
};

extern int pontos, modo_global, cd_assustados, cd_modo, contador, vida, vitoria;
extern char cores[][9], simb[4];
extern Coordenada direcao[4];

#endif