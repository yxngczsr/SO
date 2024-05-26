#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

// Função para criar diretório
int create_directory(const char *path, mode_t mode) {
    if (mkdir(path, mode) == -1) {
        fprintf(stderr, "Error creating directory '%s': %s\n", path, strerror(errno));
        return -1;
    }
    printf("Directory created: %s\n", path);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory> [more directories...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    mode_t mode = 0755;  // Default permissions are rwxr-xr-x

    // Process each directory name given as an argument
    for (int i = 1; i < argc; i++) {
        if (create_directory(argv[i], mode) != 0) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
