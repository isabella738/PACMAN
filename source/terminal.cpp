#include "settings.hpp"

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