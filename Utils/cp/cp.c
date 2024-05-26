#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <limits.h> // Make sure to include this for PATH_MAX

#define BUF_SIZE 4096  // Buffer size for file copy

// Function to copy a single file
int copy_file(const char *src, const char *dest) {
    int src_fd, dest_fd;
    ssize_t num_read, num_written;
    char buf[BUF_SIZE];
    struct stat src_stat;

    src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        perror("Failed to open source file");
        return -1;
    }

    if (fstat(src_fd, &src_stat) == -1) {
        perror("Failed to get source file status");
        close(src_fd);
        return -1;
    }

    dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dest_fd == -1) {
        perror("Failed to open/create destination file");
        close(src_fd);
        return -1;
    }

    while ((num_read = read(src_fd, buf, BUF_SIZE)) > 0) {
        char *out_ptr = buf;
        do {
            num_written = write(dest_fd, out_ptr, num_read);
            if (num_written >= 0) {
                num_read -= num_written;
                out_ptr += num_written;
            } else if (errno != EINTR) {
                perror("Failed to write to destination file");
                close(src_fd);
                close(dest_fd);
                return -1;
            }
        } while (num_read > 0);
    }

    if (num_read == -1) {
        perror("Failed to read source file");
        close(src_fd);
        close(dest_fd);
        return -1;
    }

    close(src_fd);
    close(dest_fd);
    return 0;
}

// Function to recursively copy directories
int copy_directory(const char *src, const char *dest) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char src_path[PATH_MAX];
    char dest_path[PATH_MAX];

    if (mkdir(dest, 0777) != 0 && errno != EEXIST) {
        perror("Failed to create destination directory");
        return -1;
    }

    dir = opendir(src);
    if (!dir) {
        perror("Failed to open source directory");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (stat(src_path, &statbuf) == -1) {
            perror("Failed to get file status");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (copy_directory(src_path, dest_path) != 0) {
                closedir(dir);
                return -1;
            }
        } else {
            if (copy_file(src_path, dest_path) != 0) {
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);
    return 0;
}

int main(int argc, char *argv[]) {
    struct stat statbuf;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (stat(argv[1], &statbuf) == -1) {
        perror("Failed to get source status");
        return EXIT_FAILURE;
    }

    if (S_ISDIR(statbuf.st_mode)) {
        return copy_directory(argv[1], argv[2]);
    } else {
        return copy_file(argv[1], argv[2]);
    }
}
