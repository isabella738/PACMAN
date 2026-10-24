#include <queue>
#include <settings.hpp>
#include <iostream>
#include "classes.hpp"

typedef struct{
    Coordenada p;
    int d;
}Localizacao;

int busca_em_largura(Fantasma f, Coordenada inicio, int d_inicio, int bloquear_atras){
    int passos=0;
    std::queue<Localizacao> origem;
    std::queue<Localizacao> destino;
    bool matriz[ALT][LARG] = {0};

    bool aprox = (mapa[f.alvo.y][f.alvo.x].tipo == 1);

    origem.push({inicio, d_inicio});

    matriz[inicio.y][inicio.x] = 1;
    matriz[f.p.y][f.p.x] = 1;

    if(inicio == f.alvo) return 0;

    while(!origem.empty()){
        passos++;

        while(!origem.empty()){

            Localizacao local = origem.front();
            origem.pop();

            //acrescentar novos caminhos
            for(int i=0; i<4; i++){

                if(bloquear_atras){
                    if(local.d < 2 && i == local.d+2) continue;
                    if(local.d > 1 && i == local.d-2) continue;
                }

                Localizacao novo = local;
                novo.p = local.p + direcao[i];
                novo.d = i;

                //do tunel
                if(novo.p.x < 0) novo.p.x = LARG - 1;
                else if(novo.p.x >= LARG) novo.p.x = 0;

                //se o novo caminho nao é parede nem ja foi marcado, add
                if(mapa[novo.p.y][novo.p.x].tipo != 1){ 
                    if(matriz[novo.p.y][novo.p.x]) continue;
                    matriz[novo.p.y][novo.p.x] = 1;

                    destino.push(novo);
                    if(aprox){
                        if(distancia(destino.back().p, f.alvo) < 2) return passos; //
                    }
                    else if(destino.back().p == f.alvo) return passos; //achou
                }
            }
        }
        origem.swap(destino);
    };
    return -1;
}
