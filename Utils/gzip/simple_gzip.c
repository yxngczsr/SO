#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define CHUNK 16384

// Comprime o arquivo de origem para o arquivo de destino
int compress_file(const char* src, const char* dest) {
    FILE *source = fopen(src, "rb");
    gzFile destFile = gzopen(dest, "wb");
    if (!source || !destFile) {
        if (source) fclose(source);
        if (destFile) gzclose(destFile);
        return -1;
    }

    char buffer[CHUNK];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK, source)) > 0) {
        gzwrite(destFile, buffer, bytes_read);
    }

    fclose(source);
    gzclose(destFile);
    return 0;
}

// Descomprime o arquivo de origem para o arquivo de destino
int decompress_file(const char* src, const char* dest) {
    char buffer[CHUNK];
    FILE *destFile = fopen(dest, "wb");
    gzFile source = gzopen(src, "rb");
    if (!destFile || !source) {
        if (destFile) fclose(destFile);
        if (source) gzclose(source);
        return -1;
    }

    int bytes_read;
    while ((bytes_read = gzread(source, buffer, CHUNK)) > 0) {
        fwrite(buffer, 1, bytes_read, destFile);
    }

    fclose(destFile);
    gzclose(source);
    return 0;
}

// Lê e exibe o conteúdo do arquivo gz comprimido
void zcat_file(const char* src) {
    char buffer[CHUNK];
    gzFile source = gzopen(src, "rb");
    if (!source) {
        perror("gzopen");
        return;
    }

    int bytes_read;
    while ((bytes_read = gzread(source, buffer, CHUNK)) > 0) {
        fwrite(buffer, 1, bytes_read, stdout);
    }

    gzclose(source);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <operation> <source> [destination]\n", argv[0]);
        fprintf(stderr, "Operations: -c (compress), -d (decompress), -z (zcat)\n");
        return 1;
    }

    char* operation = argv[1];
    if (strcmp(operation, "-c") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s -c <source> <destination>\n", argv[0]);
            return 1;
        }
        return compress_file(argv[2], argv[3]);
    } else if (strcmp(operation, "-d") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s -d <source> <destination>\n", argv[0]);
            return 1;
        }
        return decompress_file(argv[2], argv[3]);
    } else if (strcmp(operation, "-z") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s -z <source>\n", argv[0]);
            return 1;
        }
        zcat_file(argv[2]);
        return 0;
    } else {
        fprintf(stderr, "Invalid operation.\n");
        return 1;
    }
}
