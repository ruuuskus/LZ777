#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lz77os.h"



int main(int argc, char *argv[]) {
    const char *mode;
    const char *input_file;
    const char *out_file;
    long before_size, after_size;
    int res;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <-c (compress) or -d (decompress) > <input.txt> <output.txt>\n", argv[0]);
        return 1;
    }

    mode = argv[1];
    input_file = argv[2];
    out_file = argv[3];

    if (strcmp(mode, "-c") == 0) {
        before_size = filesize(input_file);
        if (before_size < 0) {
            perror("Ошибка при открытии входного файла");
            return 1;
        }

        res = compress(input_file, out_file);
        if (res < 0) {
            perror("Ошибка сжатия");
            return 1;
        }

        after_size = filesize(out_file);
        if (after_size < 0) {
            perror("Ошибка при чтении размера сжатого файла");
            return 1;
        }

        printf("Сжатие %ld -> %ld байт (%.2f%%)\n", before_size, after_size, before_size ? (100.0 * after_size / before_size) : 0.0);


    } else if (strcmp(mode, "-d") == 0) {
        res = decompress(input_file, out_file);

        if (res < 0) {
            perror("ошибка распаковки");
            return 1;
        }

        printf("разжатие в байтах: %d \n", res);

    } else {
        perror("не понял");
        return 1;
    }

    return 0;
}

