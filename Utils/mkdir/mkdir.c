#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

// Função para criar diretórios recursivamente (mkdir -p)
int mkdir_p(const char *path) {
    char temp[1024];
    char *p = NULL;
    size_t len;

    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    if(temp[len - 1] == '/') {
        temp[len - 1] = '\0';
    }

    for (p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(temp, S_IRWXU) != 0) {
                if (errno != EEXIST) {
                    perror("mkdir");
                    return -1;
                }
            }
            *p = '/';
        }
    }

    if (mkdir(temp, S_IRWXU) != 0) {
        if (errno != EEXIST) {
            perror("mkdir");
            return -1;
        }
    }

    return 0;
}

// Função para remover diretório (rmdir)
int remove_dir(const char *path) {
    if (rmdir(path) == 0) {
        printf("Directory %s removed successfully\n", path);
        return 0;
    } else {
        perror("rmdir");
        return -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <operation> <path>\n", argv[0]);
        fprintf(stderr, "Operations: -p (mkdir -p), -r (rmdir)\n");
        return 1;
    }

    const char *operation = argv[1];
    const char *path = argv[2];

    if (strcmp(operation, "-p") == 0) {
        return mkdir_p(path);
    } else if (strcmp(operation, "-r") == 0) {
        return remove_dir(path);
    } else {
        fprintf(stderr, "Invalid operation.\n");
        return 1;
    }
}
