#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_permissions(mode_t mode) {
    printf("%c", (S_ISDIR(mode)) ? 'd' : '-');
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c ", (mode & S_IXOTH) ? 'x' : '-');
}

void print_long_format(const char *file_name, struct stat *info) {
    print_permissions(info->st_mode);
    printf("%4u ", info->st_nlink); // Corrigido para usar %u para unsigned int
    struct passwd *pwd = getpwuid(info->st_uid);
    struct group *grp = getgrgid(info->st_gid);
    if (pwd) printf("%-8s ", pwd->pw_name);
    else printf("%-8s ", "Unknown");
    if (grp) printf("%-8s ", grp->gr_name);
    else printf("%-8s ", "Unknown");
    printf("%8ld ", info->st_size);

    char date_str[20];
    strftime(date_str, sizeof(date_str), "%b %d %H:%M", localtime(&info->st_mtime));
    printf("%s %s\n", date_str, file_name);
}

void list_directory(const char *path, int show_all, int long_format) {
    struct dirent *entry;
    struct stat statbuf;
    char full_path[1024];

    DIR *dir = opendir(path);
    if (!dir) {
        perror("Failed to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;  // Skip hidden files unless -a is specified
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (stat(full_path, &statbuf) == 0) {
            if (long_format) {
                print_long_format(entry->d_name, &statbuf);
            } else {
                printf("%s\n", entry->d_name);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    int show_all = 0, long_format = 0;
    char *path = ".";  // Default to the current directory

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            long_format = 1;
        } else if (strcmp(argv[i], "-a") == 0) {
            show_all = 1;
        } else {
            path = argv[i];  // Assume any non-flag argument is the path
        }
    }

    list_directory(path, show_all, long_format);
    return 0;
}
