#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

// Função recursiva para calcular o uso de disco de um diretório
unsigned long long calculate_du(const char *path) {
    unsigned long long total_size = 0;
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char full_path[1024];

    if (!(dir = opendir(path)))
        return 0; // Não foi possível abrir o diretório

    while ((entry = readdir(dir)) != NULL) {
        // Ignora diretórios "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (stat(full_path, &statbuf) == -1)
            continue; // Falha ao obter informações do arquivo

        // Se é um diretório, faz a chamada recursiva
        if (S_ISDIR(statbuf.st_mode)) {
            total_size += calculate_du(full_path);
        } else {
            total_size += statbuf.st_size; // Soma o tamanho do arquivo
        }
    }
    closedir(dir);

    // Mostra o tamanho do diretório atual
    printf("%llu\t%s\n", total_size, path);
    return total_size;
}

int main(int argc, char *argv[]) {
    const char *path = "."; // Diretório padrão é o diretório atual

    if (argc > 1) {
        path = argv[1];
    }

    printf("Calculating disk usage for: %s\n", path);
    calculate_du(path);

    return EXIT_SUCCESS;
}
