#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>

#define HUMAN_READABLE_SIZE 1024
#define MAX_PATH_LENGTH 4096

int human_readable = 0;
int summarize = 0;
int all_files = 0;
int display_time = 0;

void print_usage(const char *prog_name) {
    fprintf(stderr, "Uso: %s [-h] [-s] [-a] [--time] [diretório]\n", prog_name);
    fprintf(stderr, "Opções:\n");
    fprintf(stderr, "  -h          Exibe os tamanhos em formato legível para humanos (KB, MB, GB, etc).\n");
    fprintf(stderr, "  -s          Mostra apenas o total de um diretório.\n");
    fprintf(stderr, "  -a          Inclui todos os arquivos na saída, não apenas os diretórios.\n");
    fprintf(stderr, "  --time      Exibe o tempo de execução do comando.\n");
    fprintf(stderr, "  --help      Exibe esta mensagem de ajuda e sai.\n");
}

void print_help(const char *prog_name) {
    print_usage(prog_name);
    fprintf(stderr, "\nExibe o tamanho de uso do disco de arquivos e diretórios.\n");
    fprintf(stderr, "Se nenhum ARQUIVO for especificado, será utilizado o diretório atual.\n");
}

void print_human_readable(off_t size) {
    char *units[] = {"B", "KB", "MB", "GB"};
    int unit_index = 0;
    double human_size = size;

    while (human_size >= HUMAN_READABLE_SIZE && unit_index < 3) {
        human_size /= HUMAN_READABLE_SIZE;
        unit_index++;
    }

    printf("%.1f%s", human_size, units[unit_index]);
}

off_t calculate_directory_size(const char *dir_name, int depth) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char path[MAX_PATH_LENGTH];
    off_t total_size = 0;

    if ((dir = opendir(dir_name)) == NULL) {
        perror(dir_name);
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

        if (lstat(path, &file_stat) == -1) {
            perror(path);
            continue;
        }

        if (S_ISDIR(file_stat.st_mode)) {
            off_t dir_size = calculate_directory_size(path, depth + 1);
            total_size += dir_size;
            if (!summarize) {
                if (human_readable) {
                    print_human_readable(dir_size);
                    printf("\t%s\n", path);
                } else {
                    printf("%ld\t%s\n", dir_size, path);
                }
            }
        } else {
            total_size += file_stat.st_size;
            if (all_files) {
                if (human_readable) {
                    print_human_readable(file_stat.st_size);
                    printf("\t%s\n", path);
                } else {
                    printf("%ld\t%s\n", file_stat.st_size, path);
                }
            }
        }
    }

    closedir(dir);
    return total_size;
}

int main(int argc, char *argv[]) {
    int opt;
    int option_index = 0;
    char *dir_name = ".";
    clock_t start_time, end_time;

    struct option long_options[] = {
        {"help", no_argument, 0, 0},
        {"time", no_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "hsa", long_options, &option_index)) != -1) {
        if (opt == 0) {
            if (strcmp(long_options[option_index].name, "help") == 0) {
                print_help(argv[0]);
                return EXIT_SUCCESS;
            } else if (strcmp(long_options[option_index].name, "time") == 0) {
                display_time = 1;
            }
        } else {
            switch (opt) {
                case 'h':
                    human_readable = 1;
                    break;
                case 's':
                    summarize = 1;
                    break;
                case 'a':
                    all_files = 1;
                    break;
                case 't':
                    display_time = 1;
                    break;
                default:
                    print_usage(argv[0]);
                    return EXIT_FAILURE;
            }
        }
    }

    if (optind < argc) {
        dir_name = argv[optind];
    }

    if (display_time) {
        start_time = clock();
    }

    off_t total_size = calculate_directory_size(dir_name, 0);
    if (summarize) {
        if (human_readable) {
            print_human_readable(total_size);
            printf("\t%s\n", dir_name);
        } else {
            printf("%ld\t%s\n", total_size, dir_name);
        }
    }

    if (display_time) {
        end_time = clock();
        double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
        printf("Tempo de execução: %.2f segundos\n", elapsed_time);
    }

    return EXIT_SUCCESS;
}