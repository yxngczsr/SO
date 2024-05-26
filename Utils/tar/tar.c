#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define BLOCK_SIZE 512

// Estrutura para o cabeçalho de cada arquivo dentro do tar
struct tar_header {
    char filename[100];
    char filemode[8];
    char userid[8];
    char groupid[8];
    char filesize[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkedfile[100];
    char padding[255];
};

// Função para preencher o cabeçalho do arquivo tar
void fill_header(struct tar_header *header, const char *filename, struct stat *info) {
    memset(header, 0, sizeof(struct tar_header));
    strncpy(header->filename, filename, 99);
    sprintf(header->filemode, "%07o", info->st_mode);
    sprintf(header->userid, "%07o", info->st_uid);
    sprintf(header->groupid, "%07o", info->st_gid);
    sprintf(header->filesize, "%011lo", (unsigned long)info->st_size);
    sprintf(header->mtime, "%011lo", (unsigned long)info->st_mtime);
    header->typeflag = '0'; // Regular file
}

// Função para escrever um único arquivo no arquivo tar
int write_file_to_tar(int tar_fd, const char *filename) {
    int file_fd;
    ssize_t read_bytes, written_bytes;
    struct stat file_stat;
    struct tar_header header;
    char buffer[BLOCK_SIZE] = {0};

    if ((file_fd = open(filename, O_RDONLY)) < 0) {
        perror("Failed to open file");
        return -1;
    }

    if (fstat(file_fd, &file_stat) < 0) {
        perror("Failed to get file statistics");
        close(file_fd);
        return -1;
    }

    fill_header(&header, filename, &file_stat);
    write(tar_fd, &header, sizeof(struct tar_header));

    while ((read_bytes = read(file_fd, buffer, BLOCK_SIZE)) > 0) {
        written_bytes = write(tar_fd, buffer, read_bytes);
        if (written_bytes != read_bytes) {
            perror("Failed to write full buffer");
            close(file_fd);
            return -1;
        }
        memset(buffer, 0, BLOCK_SIZE);
    }

    close(file_fd);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <tarfile> <file1> [file2] ...\n", argv[0]);
        return 1;
    }

    int tar_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (tar_fd < 0) {
        perror("Failed to create tar file");
        return 1;
    }

    for (int i = 2; i < argc; i++) {
        if (write_file_to_tar(tar_fd, argv[i]) != 0) {
            close(tar_fd);
            return 1;
        }
    }

    // Write two empty blocks at the end
    char block[BLOCK_SIZE] = {0};
    write(tar_fd, block, BLOCK_SIZE);
    write(tar_fd, block, BLOCK_SIZE);

    close(tar_fd);
    return 0;
}
