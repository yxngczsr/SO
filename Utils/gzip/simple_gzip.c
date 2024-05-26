#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define CHUNK 16384

// Função para comprimir um arquivo
int compressFile(const char *src, const char *dest) {
    FILE *source = fopen(src, "rb");
    if (source == NULL) {
        perror("Failed to open source file");
        return -1;
    }

    gzFile destFile = gzopen(dest, "wb");
    if (destFile == NULL) {
        perror("Failed to open destination file");
        fclose(source);
        return -1;
    }

    char buffer[CHUNK];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK, source)) > 0) {
        if (gzwrite(destFile, buffer, bytes_read) != bytes_read) {
            perror("Failed to write to destination file");
            gzclose(destFile);
            fclose(source);
            return -1;
        }
    }

    gzclose(destFile);
    fclose(source);
    return 0;
}

// Função para descomprimir um arquivo
int decompressFile(const char *src, const char *dest) {
    gzFile source = gzopen(src, "rb");
    if (source == NULL) {
        perror("Failed to open source file");
        return -1;
    }

    FILE *destFile = fopen(dest, "wb");
    if (destFile == NULL) {
        perror("Failed to open destination file");
        gzclose(source);
        return -1;
    }

    char buffer[CHUNK];
    int bytes_read;
    while ((bytes_read = gzread(source, buffer, CHUNK)) > 0) {
        if (fwrite(buffer, 1, bytes_read, destFile) != bytes_read) {
            perror("Failed to write to destination file");
            gzclose(source);
            fclose(destFile);
            return -1;
        }
    }

    gzclose(source);
    fclose(destFile);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <c|d> <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    int result = 0;
    if (argv[1][0] == 'c') {
        result = compressFile(argv[2], argv[3]);
    } else if (argv[1][0] == 'd') {
        result = decompressFile(argv[2], argv[3]);
    } else {
        fprintf(stderr, "Invalid operation. Use 'c' for compress and 'd' for decompress.\n");
        return 1;
    }

    return result;
}
