/*
    Definição de classes e referencias a objetos
*/

#ifndef CLASS
#define CLASS

#include "settings.hpp"
#include "functions.hpp"

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
            if(mapa[p.y + direcao[d].y][p.x + direcao[d].x].tipo != 1){
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
        bool desativar_barreiras;
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

                if(entrando) alvo = spawn;
                else alvo = {8, 14};
            }
            else if(modo == 3){//preso
                //isso faz ele ficar andandinho
                if(alvo.x >= spawn.x)
                    alvo = {spawn.y, spawn.x-1};
                else
                    alvo = {spawn.y, spawn.x+1};
            }
        }

        void definir_direcao(){
            int dir[4] = {1, 1, 1, 1};
            
            d < 2 ? (dir[d+2] = 0) : (dir[d-2] = 0); //invalidar andar para tras

            for(int i=0; i<4; i++){//definir posicoes possiveis
                if(dir[i]){
                    if(mapa[p.y + direcao[i].y][p.x + direcao[i].x].tipo == 1) dir[i] = 0;
                }
            }

            int menor=10000000, indice=-1;
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

/*
    modo (interno)
    modo_global (do jogo)

    foi comido:
        entrar no quadrado
            - ir até 8, 14
            - ir para o spawn
        chegou no spawn: está preso
    
    estava preso e foi liberado:
        modo=0 e esta_no_quadrado:
            - ir até x=14
            - ir até y=8
        saiu do quadrado: segue a vida

*/