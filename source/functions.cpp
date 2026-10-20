#include "settings.hpp"
#include "classes.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#if defined (LINUX)
    #include <termios.h>
    #include <unistd.h>
#elif defined (WINDOWS)
    #include <conio.h>
    #include <windows.h>
#endif

void inicializar_fantasmas(){// 1 vez no main

    for(int i=0; i<4; i++){
        fantasma[i]->velocidade = VEL_F;
        fantasma[i]->spawn = {10, 11+ 2*i};
        fantasma[i]->cor = 3+i;
        fantasma[i]->simbolo = '@';
        fantasma[i]->liberado = 0;
    }

    Blinky.perseguir = []() {
        Blinky.alvo = Pacman.p;
    };
    Blinky.dispersar = []() {
        Blinky.alvo = {0, LARG};
    };

    //
    Pinky.perseguir = []() {
        Pinky.alvo = {Pacman.p.y + 2*direcao[Pacman.d].y, Pacman.p.x + 2*direcao[Pacman.d].x};
    };
    Pinky.dispersar = []() {
        Pinky.alvo = {0, 0};
    };

    //
    Inky.perseguir = []() {
        equacao_reta(Blinky.p, Pacman.p, &Inky.alvo);
    };
    Inky.dispersar = []() {
        Inky.alvo = {ALT, LARG};
    };

    //
    Clyde.perseguir = []() {
        if(distancia(Pacman.p, Clyde.p) >= 8) Clyde.alvo = {ALT, 0};
        else Clyde.alvo = Pacman.p;
    };
    Clyde.dispersar = []() {
        Clyde.alvo = {ALT, 0};
    };

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
}

void iniciar_fase(){
    contador = 0;
    Pacman.p = {12, 14};
    Pacman.d = 3;
    Pacman.velocidade = VEL_PAC;
    Pacman.simbolo = '<';

    //fantasmas começam presos e nos seus respectivos spawns
    for(int i=0; i<4; i++){
        fantasma[i]->p = fantasma[i]->spawn;
        fantasma[i]->modo = 3;
    }
    
    //blinky começa fora do quadrado
    Blinky.p = {8, 14}; Blinky.modo = 0; Blinky.liberado = 1;
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
    if(f->modo == 2 && f->p.x == 14 && f->p.y == 8){//chegou na "porta"
        f->desativar_barreiras = 1;
        f->entrando = 1;
    }
    else if(f->modo == 2 && f->p == f->spawn){ //chegou no spawn (entrou no quadrado)
        f->modo = 0;
        f->desativar_barreiras = 0;
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

void equacao_reta(Coordenada a, Coordenada b, Coordenada *novo){//blinky, pacman, mira
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


void imp(char c, int i){
    std::cout << cores[i] << c << RESET;
}

void imprimir(){
    std::cout << VOLTAR;
    std::cout << "Pontuacao: " << pontos << std::endl;
    for(int i=0; i<vida; i++){
        std::cout << AMARELO << "@ " << RESET;
    }
    std::cout <<'\n';
    
    for(int y=0; y<ALT; y++){
        for(int x=0; x<LARG; x++){
            int i=0;
            if(Pacman.p.x == x && Pacman.p.y == y){
                imp(Pacman.simbolo, Pacman.cor);
                i=1;
            }
            else for(int a=0; a<4; a++){
                if(fantasma[a]->p.x == x && fantasma[a]->p.y == y){
                    switch(fantasma[a]->modo){
                        case 1:
                            imp(fantasma[a]->simbolo, 8);
                            break;
                        case 2:
                            imp(fantasma[a]->simbolo, 1);
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
        pontos++;
        contador++;
        mapa[p.y][p.x].tipo = 0;
        mapa[p.y][p.x].c = ' ';
    }
    else if(mapa[p.y][p.x].tipo == 3){
        pontos += 10;
        cd_assustados = TEMPO_ASSUSTADO;
        mapa[p.y][p.x].tipo = 0;
        mapa[p.y][p.x].c = ' ';
    }
    for(int i=0; i<4; i++){
        if(fantasma[i]->p == p){
            if(fantasma[i]->modo == 0){
                vida--;
                return 1;
            }
            else if(fantasma[i]->modo == 1){
                fantasma[i]->modo = 2;
            }
            break;
        }
    }
    return 0;
}

void game_config(){
    vitoria = 1;

    cd_modo--;
    if(cd_modo == 0){
        modo_global = !modo_global;
        cd_modo = TROCAR_MODO;
    }

    if(cd_assustados == TEMPO_ASSUSTADO){
        for(int i=0; i<4; i++) fantasma[i]->modo = 1;
    }
    else if(cd_assustados == 1){
        for(int i=0; i<4; i++) fantasma[i]->modo = 0;
    }
    if(cd_assustados)
        cd_assustados--;

    for(int i=0; i<ALT; i++){
        for(int j=0; j<LARG; j++){
            if(mapa[i][j].tipo > 1) vitoria = 0;
        }
    }
    
    if(contador >= 7 && !Pinky.liberado){ Pinky.liberado = 1; Pinky.modo = 0;}
    if(contador >= 17 && !Inky.liberado){ Inky.liberado = 1; Inky.modo = 0;}
    if(contador >= 32 && !Clyde.liberado){ Clyde.liberado = 1; Clyde.modo = 0;}
}

void ler_teclado(){
    char comando;

    #if defined(WINDOWS)
        if(_kbhit()) {
            comando = _getch();
        }
    #elif defined(LINUX)
        char c;
        if(read(STDIN_FILENO, &c, 1) > 0) comando = c;
    #endif

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
            if(mapa[y-1][x].tipo != 1){
                Pacman.d = 2; Pacman.simbolo = 'V'; 
            }
            break;
        case 'a':
            if(mapa[y-1][x].tipo != 1){
                Pacman.d = 3; Pacman.simbolo = '<'; 
            }
            break;
    }
}

void info_para_desenvolvedores(){
    std::cout << '\n';
    std::cout << Pacman.p.y << ' ' << Pacman.p.x << "   " << std::endl;
    std::cout << "Direcao: " << Pacman.d << std::endl;
}