#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

void list_processes(int show_all, char *user) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    FILE *fp;
    char path[1024];
    char line[256];
    char uid[32];

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Failed to open /proc");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
            if ((fp = fopen(path, "r")) != NULL) {
                while (fgets(line, sizeof(line), fp) != NULL) {
                    if (strncmp(line, "Uid:", 4) == 0) {
                        sscanf(line, "Uid:\t%s", uid);
                        struct passwd *pwd = getpwuid(atoi(uid));
                        if (pwd != NULL && (show_all || (user != NULL && strcmp(pwd->pw_name, user) == 0))) {
                            printf("%s (%s)\n", entry->d_name, pwd->pw_name);
                        }
                    }
                }
                fclose(fp);
            }
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    int show_all = 0;
    char *user = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0 && i + 1 < argc) {
            user = argv[++i];
        } else if (strcmp(argv[i], "aux") == 0) {
            show_all = 1;
        }
    }

    list_processes(show_all, user);
    return 0;
}
