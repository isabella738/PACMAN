/*
    Funções que dependem do sistema operacional
*/

#include "settings.hpp"
#include "terminal.hpp"
#include <stdlib.h>

#ifdef LINUX
    #include <termios.h>
    #include <unistd.h>

    struct termios velho_terminal, novo_terminal;
    void mudar_terminal(){
        tcgetattr(STDIN_FILENO, &velho_terminal);
        novo_terminal = velho_terminal;

        novo_terminal.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &novo_terminal);
    }
    void restaurar_terminal(){
        tcsetattr(STDIN_FILENO, TCSANOW, &velho_terminal);
    }
#endif 

int ler_teclado(char *comando){
    #if defined(WINDOWS)
        if(_kbhit()) {
            (*comando) = _getch();
            return 1;
        }
    #elif defined(LINUX)
        char c;
        if(read(STDIN_FILENO, &c, 1) > 0){ 
            (*comando) = c;
            return 1;
        }
    #endif
    return 0;
}

void limpar_tela(){
    #if defined(LINUX)
        system("clear");
    #else
        system("cls");
    #endif
}
