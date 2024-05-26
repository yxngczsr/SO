#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

// Função para mover arquivos entre sistemas de arquivos
int move_file(const char *src, const char *dest) {
    // Tenta renomear primeiro
    if (rename(src, dest) == 0) {
        return 0; // Sucesso
    }

    // Se falhar devido a sistemas de arquivos diferentes
    if (errno == EXDEV) {
        // Implementar copiar e excluir
        char cmd[1024];
        sprintf(cmd, "cp -r %s %s && rm -r %s", src, dest, src);
        return system(cmd); // Executa os comandos de copiar e excluir
    } else {
        perror("Failed to move");
        return -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *source = argv[1];
    const char *destination = argv[2];

    if (move_file(source, destination) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
