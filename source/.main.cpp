/*
    Onde ocorre o jogo, na função main
*/

#include <iostream>
#include "functions.hpp"
#include "classes.hpp"
#include "settings.hpp"
#include "terminal.hpp"

#if defined(LINUX)
    #include <fcntl.h>
    #include <unistd.h>
#endif

int main(){
    #if defined(LINUX)
        mudar_terminal();
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    #endif

    limpar_tela();
    cutscene();
    inicialização_mapa();
    inicializar_fantasmas();
    iniciar();
    
    int frames=0;
    do{
        pacman_comando();
        game_config();
        if(vitoria) break;
        
        for(int i=0; i<4; i++){            
            int v = VEL_F;
            if(fantasma[i]->modo == 1) v*=2;
            else if(fantasma[i]->modo == 2) v=1;

            if(fantasma[i]->cooldown(v)){
                fantasma[i]->definir_alvo(modo_global);
                fantasma[i]->definir_direcao();
                if(fantasma[i]->modo != 3) fantasma[i]->andar();
            }
        }
        if(Pacman.cooldown(VEL_PAC)) Pacman.andar();

        if(colisao()){
            pausa(2*SEGUNDOS);
            if(vida) iniciar();
            else break;
        }
        
        imprimir();
        pausa(SEGUNDOS/FPS);
        //std::cout << frames++;
    }while(1);

    if(vida)
        std::cout << "\nVoce Ganhou!\n";
    else
        std::cout << "\nVoce perdeu :(\n";
    
    return 0;
}