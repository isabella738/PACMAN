/*
    Declarações das variaveis referenciadas em settings.hpp e classes.hpp
*/

#include "settings.hpp"
#include "classes.hpp"

Fantasma Blinky = {0}, Inky = {0}, Pinky = {0}, Clyde = {0};
Entidade Pacman = {0};
Mapa mapa[ALT][LARG];

int pontos=0, modo_global = 1, cd_assustados=0, cd_modo = TROCAR_MODO, vida=3, vitoria=0;//def
int contador_dots=0, contador_f=0;

Fantasma *fantasma[4] = {
    &Blinky, &Inky, &Pinky, &Clyde
};

char cel[ALT][LARG+1]={//22x30
    ".+============..============+.",
    "|- - - - - - -II- - - - - - -|",
    "|oIIII-IIIIII-II-IIIIII-IIIIo|",
    "|-IIII-IIIIII-II-IIIIII-IIII-|",
    "|- - - - - - - - - - - - - - |",
    "|-IIII-II-IIIIIIIIII-II-IIII-|",
    "|- - - II- - -II- - -II- - - |",
    " ''''|-IIIIII-II-IIIIII-|'''' ",
    "     |-II- - - - - - II-|     ", //8, 14
    "_____|-II-.===~~===.-II-|_____",
    "- - - - - |        |- - - - - ", //10, 11
    "'''''|-II-'========'-II-|'''''",
    "     |-II- - - - - - II-|     ",//12
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

char cores[][9]={
    BRANCO,
    CINZA,
    AMARELO,
    VERMELHO,
    MAGENTA,
    CIANO,
    VERDE,
    AZUL
};

char simb[4] = {
    'A',
    '>',
    'V',
    '<'
};

Coordenada direcao[4] = {//0cima 1direita 2baixo 3esquerda
    {-1, 0},
    {0, 1},
    {1, 0},
    {0, -1},
};
