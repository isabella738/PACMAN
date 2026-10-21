/*
    Definição de classes e referencias a objetos
    Incluir este arquivo em outro é suficiente para que ele reconheça todos estes elementos.
*/

#ifndef CLASS
#define CLASS

#include "settings.hpp"
#include "functions.hpp"
#include <iostream>

class Mapa {
    public:
        int tipo;//0vazio, 1parede, 2bolinha, 3especial
        char c;
        int cor;
};
extern Mapa mapa[ALT][LARG];

class Entidade {
    public:
        char simbolo;
        bool desativar_barreiras;
        int d;
        int cor;
        int velocidade;
        Coordenada spawn;
        Coordenada p;

        int cooldown(int vel){
            if(velocidade == 0){
                velocidade = vel;
                return 1;
            }
            else velocidade--;
            return 0;
        }

        int andar(){
            if(
                mapa[p.y + direcao[d].y][p.x + direcao[d].x].tipo != 1 || 
                p.x + direcao[d].x == -1 || p.x + direcao[d].x == LARG ||
                desativar_barreiras
            ){
                p.y += direcao[d].y;
                p.x += direcao[d].x;
                
                if(p.x == LARG)p.x = 0;
                else if(p.x < 0)p.x = LARG-1;
            }
            return 0;
        }
};

class Fantasma : public Entidade {
    public:
        int modo;//(0=dispersar/perseguir, 1=vulneravel, 2=comido, 3=preso)
        bool entrando;
        bool liberado;
        Coordenada alvo;

        std::function<void()> perseguir;
        std::function<void()> dispersar;

        void definir_alvo(int i){
            if(modo == 0){
                if(esta_no_quadrado(p)){
                    desativar_barreiras = 1;
                    sair_do_quadrado(this);
                    return;
                }
                else{
                    desativar_barreiras = 0;
                }

                if(i) perseguir();
                else dispersar();
            }
            else if(modo == 1){//assustado
                int x = std::rand()%LARG;
                int y = std::rand()%ALT;
                alvo = {y, x};
            }
            else if(modo == 2){//comido
                entrar_no_quadrado(this);

                //if(entrando) alvo = spawn;
                if(entrando){
                    if(p.y != 10) alvo = {10, p.x};
                    else alvo = spawn;
                }
                else alvo = {8, 14};
            }
            else if(modo == 3){//preso
                //isso faz ele ficar andandinho
                if(p.x == spawn.x)
                    p.x++;
                else if(p.x == spawn.x+1)
                    p.x--;
            }
        }

        void definir_direcao(){
            //para surgir do outro lado do mapa
            if((p.x == 0 && d==3) || (p.x == LARG-1 && d==1)) return;

            double menor=10000000;
            int indice=-1;
            int dir[4] = {1, 1, 1, 1};

            if(!desativar_barreiras){// bloquear caminhos invalidos. se nao, todos os caminhos sao validos
                d < 2 ? (dir[d+2] = 0) : (dir[d-2] = 0); //invalidar andar para tras

                for(int i=0; i<4; i++){//definir posicoes possiveis
                    if(dir[i]){
                        if(mapa[p.y + direcao[i].y][p.x + direcao[i].x].tipo == 1) dir[i] = 0;
                    }
                }
            }

            for(int i=0; i<4; i++){//definir a menor distancia dentre as posições possiveis
                if(dir[i]){
                    double n = distancia(alvo, {(p.y + direcao[i].y), (p.x + direcao[i].x)});
                    if(n < menor){
                        menor = n; indice = i;
                    }
                }
            }

            d = indice;
        }

};

extern Fantasma Blinky, Inky, Pinky, Clyde;
extern Fantasma *fantasma[4];
extern Entidade Pacman;

#endif