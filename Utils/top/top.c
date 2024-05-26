#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    initscr();              // Inicializa a tela usando ncurses
    noecho();               // Não ecoa caracteres digitados
    cbreak();               // Desabilita buffering de linha, permitindo captura de teclas instantânea
    nodelay(stdscr, TRUE);  // Não bloqueia getch() esperando por entrada
    curs_set(0);            // Esconde o cursor

    int ch;
    while ((ch = getch()) != 'q') {  // Pressione 'q' para sair
        clear();  // Limpa a tela

        // Aqui você pode inserir as chamadas para atualizar e mostrar dados do sistema
        printw("Press 'q' to quit\n");

        // Simulação de dados dinâmicos (aqui você adicionaria a coleta de dados do sistema)
        for (int i = 0; i < 10; i++) {
            printw("Process %d running\n", i);
        }

        refresh();  // Atualiza a tela com o conteúdo do buffer de saída
        sleep(1);   // Atualiza a tela a cada 1 segundo
    }

    endwin();  // Restaura as configurações do terminal
    return 0;
}
