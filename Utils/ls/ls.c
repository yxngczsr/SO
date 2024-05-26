#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

// Função para imprimir permissões de arquivo
void print_permissions(mode_t mode) {
    printf( (S_ISDIR(mode)) ? "d" : "-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

// Função para imprimir informações detalhadas do arquivo
void print_file_info(const char *name, struct stat *st) {
    print_permissions(st->st_mode);
    printf(" %ld", st->st_nlink);
    printf(" %s", getpwuid(st->st_uid)->pw_name);
    printf(" %s", getgrgid(st->st_gid)->gr_name);
    printf(" %5ld", st->st_size);

    char date[20];
    strftime(date, sizeof(date), "%b %d %H:%M", localtime(&st->st_mtime));
    printf(" %s %s\n", date, name);
}

// Função para listar diretório
void list_directory(const char *path, int detailed) {
    struct dirent *entry;
    struct stat statbuf;
    char fullpath[1024];

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Failed to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        if (stat(fullpath, &statbuf) == -1) {
            perror("Failed to get file status");
            continue;
        }

        if (detailed) {
            print_file_info(entry->d_name, &statbuf);
        } else {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    int detailed = 0;

    // Verifica flags para detalhes
    if (argc > 1 && strcmp(argv[1], "-l") == 0) {
        detailed = 1;
    }

    const char *path = (argc > 2) ? argv[2] : ".";

    list_directory(path, detailed);
    return EXIT_SUCCESS;
}
