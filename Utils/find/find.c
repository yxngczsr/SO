#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void find_file(const char *dir_path, const char *file_to_find) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char path[1024];

    if (!(dir = opendir(dir_path))) {
        fprintf(stderr, "Error opening directory '%s'\n", dir_path);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;  // Skip the '.' and '..' directories
        }

        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        if (stat(path, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                find_file(path, file_to_find);  // Recursive call for directories
            } else if (strcmp(entry->d_name, file_to_find) == 0) {
                printf("Found: %s\n", path);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <directory> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    find_file(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
