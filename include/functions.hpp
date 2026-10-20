#include "settings.hpp"

#ifndef FUNCTIONS
#define FUNCTIONS

class Fantasma; //indica que existe, antes de saber a definição

double distancia(Coordenada p1, Coordenada p2);
void equacao_reta(Coordenada a, Coordenada b, Coordenada *novo);
void imp(char c, int i);
void imprimir();
int esta_no_quadrado(Coordenada p);
void sair_do_quadrado(Fantasma *f);
void entrar_no_quadrado(Fantasma *f);
int colisao();
void game_config();
void pausa(int msg);
void iniciar();
void ler_teclado();
void inicializar_fantasmas();

void inicialização_mapa();

void iniciar_fase();

void info_para_desenvolvedores();

#endif