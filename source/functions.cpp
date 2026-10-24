/*
    Principais funções de cálculo do jogo
*/

#include "settings.hpp"
#include "classes.hpp"
#include "terminal.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <math.h>

void cutscene(){
    int i=0;
    while(1){
        std::cout << VOLTAR;

        for(int y=0; y<13; y++){
            std::cout << AMARELO << inicio[i][y] << RESET << std::endl;
        }
        pausa(0.5*SEGUNDOS);
        i = !i;

        char c;
        if(ler_teclado(&c)) break;
    }
    limpar_tela();
}

void inicializar_fantasmas(){// 1 vez no main

    for(int i=0; i<4; i++){
        fantasma[i]->velocidade = VEL_F;
        fantasma[i]->spawn = {10, 11+ 2*i};
        fantasma[i]->cor = 3+i;
        fantasma[i]->simbolo = '@';
    }

    Blinky.perseguir = []() {
        Blinky.alvo = Pacman.p;
    };
    Blinky.dispersar = []() {
        Blinky.alvo = {1, LARG-2};
    };

    //
    Pinky.perseguir = []() {
        Coordenada p = Pacman.p + direcao[Pacman.d]*2;
        if(p.x <= 0) p.x = 1;
        else if(p.x >= LARG-1) p.x = LARG-2;

        if(p.y <= 0) p.y = 1;
        else if(p.y >= ALT-1) p.y = ALT-2;
        
        Pinky.alvo = p;
    };
    Pinky.dispersar = []() {
        Pinky.alvo = {1, 1};
    };

    //
    Inky.perseguir = []() {
        equacao_reta(Blinky.p, Pacman.p + direcao[Pacman.d]*2, &Inky.alvo);
    };
    Inky.dispersar = []() {
        Inky.alvo = {ALT-2, LARG-2};
    };

    //
    Clyde.perseguir = []() {
        if(distancia(Pacman.p, Clyde.p) >= 8) Clyde.alvo = {ALT, 0};
        else Clyde.alvo = Pacman.p;
    };
    Clyde.dispersar = []() {
        Clyde.alvo = {ALT-2, 1};
    };

    Pacman.cor = 2;
    Pacman.velocidade = VEL_PAC;
}

void inicialização_mapa(){// 1 vez no main
    for(int y=0; y<ALT; y++){
        for(int x=0; x<LARG; x++){

            extern char cel[ALT][LARG+1];
            switch(cel[y][x]){
                case '-': //pontos
                    mapa[y][x].cor = 0;
                    mapa[y][x].tipo = 2;
                    break;
                case 'o': //bolinhas especiais
                    mapa[y][x].cor = 0;
                    mapa[y][x].tipo = 3;
                    break;
                case ' ': //vazio
                    mapa[y][x].cor = 0;
                    mapa[y][x].tipo = 0;
                    break;
                default: //parede
                    mapa[y][x].cor = 1;
                    mapa[y][x].tipo = 1;
                    break;
            }

            mapa[y][x].c = cel[y][x];
        }
    }
    mapa[9][14].tipo = 4;
    mapa[9][15].tipo = 4;
}

void iniciar_fase(){
    contador_dots = 0;
    contador_f = 0;
    Pacman.p = {12, 14};
    Pacman.d = 3;
    Pacman.simbolo = '<';

    //fantasmas começam presos e nos seus respectivos spawns
    for(int i=0; i<4; i++){
        fantasma[i]->p = fantasma[i]->spawn;
        fantasma[i]->modo = 3;
        fantasma[i]->liberado = 0;
    }
    
    //blinky começa fora do quadrado
    Blinky.p = {8, 14}; 
    Blinky.modo = 0; Blinky.liberado = 1;
}

void iniciar(){//antes de cada fase
    for(int i=0; i<4; i++){
        fantasma[i]->p = {-1, -1};
        fantasma[i]->d = 0;
    }
    Pacman.p = {-1, -1};
    
    imprimir();
    pausa(2*SEGUNDOS);

    iniciar_fase();

    imprimir();
    pausa(2*SEGUNDOS);
}

//Outros

void entrar_no_quadrado(Fantasma *f){
    if(f->modo == 2 && f->p == Coordenada{8, 14}){//chegou na "porta"
        f->entrando = 1;
    }
    else if(f->modo == 2 && f->p == f->spawn){ //chegou no spawn (entrou no quadrado)
        f->modo = 0;
        f->entrando = 0;
    }
}

void sair_do_quadrado(Fantasma *f){
    if(f->p.x != 14) f->alvo = {10, 14};
    else f->alvo = {8, 14};
}

int esta_no_quadrado(Coordenada p){
    if(
        (p.x >= 11 && p.x <= 18) &&
        (p.y == 10 || p.y == 9)
    ) return 1;
    return 0;
}

double distancia(Coordenada p1, Coordenada p2){
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void equacao(double a, double b, double *y, double *x, double i){
    if(i){ //achar y 
        (*y) = a*(*x) + b;
    }
    else{
        (*x) = ((*y)-b)/a;
    }
}

int ponto_valido(int y, int x){
    if(x <= 0 || x >= LARG-1 || y <= 0 || y >= ALT-1) return 0;
    if(mapa[y][x].tipo == 1) return 0;
    return 1;
}

void equacao_reta(Coordenada ponto, Coordenada centro, Coordenada *novo){//blinky, pacman+2, mira

    if(ponto.x == centro.x){
        novo->y = 2*centro.y - ponto.y;
        if(novo->y >= 0) novo->y = 1;
        else if(novo->y <= ALT-1) novo->y = ALT-2;
    }

    double a = (double)(ponto.y - centro.y)/(ponto.x - centro.x);
    double b = ponto.y - ponto.x*a;

    Coordenada d = centro*2 - ponto;
    double dx = d.x, dy = d.y;

    if(dx <= 0){
        if(dy != 10) dx = 1;
    }
    else if(dx >= LARG-1){
        if(dy != 10) dx = LARG-2;        
    }
    equacao(a, b, &dy, &dx, 1);

    if(dy <= 0){
        dy = 1;
    }
    else if(dy >= ALT-1){
        dy = ALT-2;        
    }
    equacao(a, b, &dy, &dx, 0);

    dx = round(dx); dy = round(dy);
    *(novo) = {(int)dy, (int)dx};
}


void imp(char c, int i){
    std::cout << cores[i] << c << RESET;
}

void imprimir(){
    std::cout << VOLTAR;
    
    for(int y=0; y<ALT; y++){
        for(int x=0; x<LARG; x++){
            int i=0;
            Coordenada coord = {y, x};
            if(Pacman.p == coord){
                imp(Pacman.simbolo, Pacman.cor);
                i=1;
            }
            else for(int a=0; a<4; a++){
                if(fantasma[a]->p == coord){
                    switch(fantasma[a]->modo){
                        case 1:
                            imp(fantasma[a]->simbolo, 7);
                            break;
                        case 2:
                            imp('*', 1);
                            break;
                        default:
                            imp(fantasma[a]->simbolo, fantasma[a]->cor);
                            break;
                    }
                    i=1;
                    break;
                }
            }
            if(!i) imp(mapa[y][x].c, mapa[y][x].cor); 
        }
        imp('\n', 0);
    }
    
    std::cout << "Pontuacao: " << pontos << std::endl;
    for(int i=0; i<vida; i++){
        std::cout << AMARELO << "@ " << RESET;
    }
    std::cout <<"               \n";

    info_para_desenvolvedores();
}

void pausa(int msg){
    std::this_thread::sleep_for(
        std::chrono::milliseconds(msg)
    );
}


int colisao(){
    Coordenada p = Pacman.p;

    if(mapa[p.y][p.x].tipo == 2){
        pontos += P_DOTS;
        contador_dots ++;
        mapa[p.y][p.x].tipo = 0;
        mapa[p.y][p.x].c = ' ';
    }
    else if(mapa[p.y][p.x].tipo == 3){
        pontos += P_POWER;
        cd_assustados = TEMPO_ASSUSTADO;
        mapa[p.y][p.x].tipo = 0;
        mapa[p.y][p.x].c = ' ';
    }
    for(int i=0; i<4; i++){
        if(fantasma[i]->p == p){
            if(fantasma[i]->modo == 0){//perdeu
                vida--;
                return 1;
            }
            else if(fantasma[i]->modo == 1){//comer fantasma
                pontos += P_COMER*pow(2, contador_f);
                contador_f++;
                fantasma[i]->modo = 2;
            }
            break;
        }
    }
    return 0;
}

void game_config(){ //gerencia cooldowns e codição de liberação de fantasmas
    vitoria = 1;

    cd_modo--;
    if(cd_modo == 0){
        modo_global = !modo_global;
        cd_modo = TROCAR_MODO;
    }

    if(cd_assustados == TEMPO_ASSUSTADO){
        for(int i=0; i<4; i++){ 
            if(fantasma[i]->modo == 0) fantasma[i]->modo = 1;
        }
    }
    else if(cd_assustados == 1){
        contador_f = 0;
        for(int i=0; i<4; i++) {
            if(fantasma[i]->modo == 1) fantasma[i]->modo = 0;
        }
    }
    if(cd_assustados)
        cd_assustados--;

    for(int i=0; i<ALT; i++){
        for(int j=0; j<LARG; j++){
            if(mapa[i][j].tipo > 1) vitoria = 0;
        }
    }
    
    if(contador_dots >= 7 && !Pinky.liberado){ Pinky.liberado = 1; Pinky.modo = 0;}
    if(contador_dots >= 17 && !Inky.liberado){ Inky.liberado = 1; Inky.modo = 0;}
    if(contador_dots >= 32 && !Clyde.liberado){ Clyde.liberado = 1; Clyde.modo = 0;}
}

void pacman_comando(){

    char comando;
    if(!ler_teclado(&comando)) return;

    int y = Pacman.p.y, x = Pacman.p.x;
    switch(comando){
        case 'w':
            if(mapa[y-1][x].tipo != 1){
                Pacman.d = 0; Pacman.simbolo = 'A';
            }
            break;
        case 'd':
            if(mapa[y][x+1].tipo != 1){
                Pacman.d = 1; Pacman.simbolo = '>'; 
            }
            break;
        case 's':
            if(mapa[y+1][x].tipo != 1){
                Pacman.d = 2; Pacman.simbolo = 'V'; 
            }
            break;
        case 'a':
            if(mapa[y][x-1].tipo != 1){
                Pacman.d = 3; Pacman.simbolo = '<'; 
            }
            break;
    }
}

void info_para_desenvolvedores(){

    for(int i=3; i>=0; i--){
        for(int j=i; j>=0; j--) std::cout << "----------";
        std::cout << "Fant " << i << '\n'; 
        for(int j=i; j>=0; j--) std::cout << "----------";
        std::cout << fantasma[i]->d << '\n';
        for(int j=i; j>=0; j--) std::cout << "----------";
        std::cout << fantasma[i]->velocidade << '\n';
        for(int j=i; j>=0; j--) std::cout << "----------";
        std::cout << modo_global << '\n';
        for(int j=i; j>=0; j--) std::cout << "----------";
        std::cout << "\033[4A\r";
    }
    std::cout << "\ndir\nveloc\nmodo\n";

    /*
    std::cout << '\n';
    std::cout << Pacman.p.y << ' ' << Pacman.p.x << "   " << std::endl;
    std::cout << "Direcao: " << Pacman.d << std::endl;
    std::cout << "Modo global: " << modo_global << std::endl;
    //std::cout << "Blinky.alvo: " << Blinky.alvo.y << " " << Blinky.alvo.x << "    " << std::endl;
    std::cout << "Blinky.modo: " << Blinky.modo << "    " << std::endl;
    std::cout << "Contador: " << contador_dots << "    " << std::endl;
    //std::cout << "Cooldown: " << cd_assustados << "     " << std::endl;
    //std::cout << "Blinky.p: " << Blinky.p.y << ' ' << Blinky.p.x << "   " << std::endl;
    //std::cout << "Blinky.d: " << Blinky.d << "    " << std::endl;
    */
}