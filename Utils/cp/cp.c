#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

#define BUF_SIZE 4096

void print_options() {
    printf("\nOptions:\n\n");
    printf("  <source> <dest>        : Move files or directories.\n");
    printf("  -r <source> <dest>     : Copy directories recursively.\n");
    printf("  -i <source> <dest>     : Copy files with confirmation if destination exists.\n");
    printf("  --help                 : Display this help message.\n\n");
}

void copy_file(const char *source, const char *dest, int interactive) {
    int src_fd, dest_fd, nread;
    char buf[BUF_SIZE];
    struct stat src_stat;

    src_fd = open(source, O_RDONLY);
    if (src_fd == -1) {
        perror("Failed to open source file");
        exit(EXIT_FAILURE);
    }

    fstat(src_fd, &src_stat);

    if (interactive && access(dest, F_OK) == 0) {
        char response;
        printf("Destination file '%s' already exists. Overwrite? (y/n): ", dest);
        scanf(" %c", &response);
        if (response != 'y' && response != 'Y') {
            printf("Operation canceled.\n");
            close(src_fd);
            exit(EXIT_SUCCESS);
        }
    }

    dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dest_fd == -1) {
        perror("Failed to open destination file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    while ((nread = read(src_fd, buf, BUF_SIZE)) > 0) {
        if (write(dest_fd, buf, nread) != nread) {
            perror("Failed to write to destination file");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    if (nread == -1) {
        perror("Failed to read from source file");
    }

    close(src_fd);
    close(dest_fd);
}

void copy_directory(const char *source, const char *dest) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char src_path[PATH_MAX];
    char dest_path[PATH_MAX];

    if ((dir = opendir(source)) == NULL) {
        perror("Failed to open source directory");
        exit(EXIT_FAILURE);
    }

    if (mkdir(dest, 0777) == -1) {
        perror("Failed to create destination directory");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(src_path, sizeof(src_path), "%s/%s", source, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (lstat(src_path, &st) == -1) {
            perror("Failed to get file status");
            closedir(dir);
            exit(EXIT_FAILURE);
        }

        if (S_ISDIR(st.st_mode)) {
            copy_directory(src_path, dest_path);
        } else {
            copy_file(src_path, dest_path, 0);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    int opt;
    int recursive = 0, interactive = 0;

    if (argc < 3) {
        print_options();
        return EXIT_FAILURE;
    }

    while ((opt = getopt(argc, argv, "ri")) != -1) {
        switch (opt) {
            case 'r':
                recursive = 1;
                break;
            case 'i':
                interactive = 1;
                break;
            default:
                print_options();
                return EXIT_FAILURE;
        }
    }

    if (argc - optind != 2) {
        fprintf(stderr, "Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    const char *source = argv[optind];
    const char *dest = argv[optind + 1];

    struct stat st;
    if (stat(source, &st) == -1) {
        perror("Failed to get source status");
        return EXIT_FAILURE;
    }

    if (S_ISDIR(st.st_mode)) {
        if (recursive) {
            copy_directory(source, dest);
        } else {
            fprintf(stderr, "Source is a directory, use -r option to copy recursively.\n");
            return EXIT_FAILURE;
        }
    } else {
        copy_file(source, dest, interactive);
    }

    return EXIT_SUCCESS;
}

