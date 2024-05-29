#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

void print_usage() {
    printf("Usage:\n");
    printf("  ./find -name <pattern> /path/to/search\n");
    printf("  ./find -locate <pattern>\n");
    printf("  ./find -grep <pattern> /path/to/search\n");
    printf("  ./find -fdupes /path/to/search\n");
}

void search_by_name(const char *name, const char *path) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path))) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char new_path[1024];
            snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
            search_by_name(name, new_path);
        } else {
            if (strstr(entry->d_name, name)) {
                printf("%s/%s\n", path, entry->d_name);
            }
        }
    }
    closedir(dir);
}

void locate_pattern(const char *pattern) {
    char command[256];
    snprintf(command, sizeof(command), "locate %s", pattern);
    system(command);
}

void grep_pattern(const char *pattern, const char *path) {
    char command[256];
    snprintf(command, sizeof(command), "grep -r %s %s", pattern, path);
    system(command);
}

void find_duplicates(const char *path) {
    char command[256];
    snprintf(command, sizeof(command), "fdupes -r %s", path);
    system(command);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "-name") == 0) {
        if (argc != 4) {
            print_usage();
            return 1;
        }
        search_by_name(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-locate") == 0) {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        locate_pattern(argv[2]);
    } else if (strcmp(argv[1], "-grep") == 0) {
        if (argc != 4) {
            print_usage();
            return 1;
        }
        grep_pattern(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-fdupes") == 0) {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        find_duplicates(argv[2]);
    } else {
        print_usage();
        return 1;
    }

    return 0;
}
