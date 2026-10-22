/*
    Define o sistema operacional usado e contém cabeçalhos das funções que dependem do SO
*/
#ifndef TERMINAL
#define TERMINAL

#if defined(_WIN32) || defined(_WIN64)
    #define WINDOWS
#elif defined(__linux__)
    #define LINUX
#endif

#if defined(LINUX)
    void mudar_terminal();
    void restaurar_terminal();
#endif

int ler_teclado(char *comando);
void limpar_tela();

#endif
