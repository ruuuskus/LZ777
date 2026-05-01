#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lz77os.h"


long filesize(const char *filename) {
    FILE *file;
    long size;
    file = fopen(filename, "rb");
    if (!file){
        return -1;
    }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fclose(file);
    return size;
}


int compress(const char *input_name, const char *out_name){
    FILE *input_file, *out_file;          
    unsigned char *mas;
    long fsize;
    long pos;
    int start, len, best_len, best_pos, max_len, search_start;

    input_file = fopen(input_name, "rb");
    if (!input_file) return -1;
    out_file = fopen(out_name, "wb");   

    if (!out_file) {
        fclose(input_file);
        return -1;
    }

    fseek(input_file, 0, SEEK_END);
    fsize = ftell(input_file);
    
    if (fsize == 0) {
        fclose(input_file);
        fclose(out_file);
        return 0;
    }
    fseek(input_file, 0, SEEK_SET);

    mas = (unsigned char *)malloc(fsize);
    if (!mas) {
        fclose(input_file);
        fclose(out_file);
        return -1;
    }
    if (fread(mas, 1, fsize, input_file) != (size_t)fsize) {
        free(mas);
        fclose(input_file);
        fclose(out_file);
        return -1;
    }
    fclose(input_file);                  

    pos = 0;
    while (pos < fsize) {
        best_len = 0;
        best_pos = 0;

        max_len = (int)(fsize - pos);
        if (max_len > MAX_LEN) max_len = MAX_LEN;

        if (max_len >= MIN_LEN) {
            search_start = (pos > WINDOW_SIZE) ? (int)(pos - WINDOW_SIZE) : 0;
            for (start = search_start; start < pos; start++) {
                len = 0;
                while ((len < max_len) && mas[start + len] == mas[pos + len]) {
                    len++;
                }
                if (len > best_len) {
                    best_len = len;
                    best_pos = start;
                }
            }
        }

        if (best_len >= MIN_LEN) {
            unsigned int len_back;
            unsigned char flag;

            len_back = (unsigned int)(pos - best_pos);
            flag = (unsigned char)(0x80 | ((best_len - MIN_LEN) & 0x0F));
            
            fputc(flag, out_file);              
            fputc(len_back & 0xFF, out_file);
            fputc((len_back >> 8) & 0xFF, out_file);
            pos += best_len;
        } else {
            fputc(0x00, out_file);               
            fputc(mas[pos], out_file);
            pos++;
        }
    }

    free(mas);
    fclose(out_file);                            
    return 0;
}

/* Распаковка */
int decompress(const char *input_name, const char *out_name){
    FILE *input_file, *out_file;                 
    unsigned char window[WINDOW_SIZE];
    int win_pos;
    int after_size;
    int b1, b2, b3;
    int solo;
    int len_nosolo;
    unsigned int len_back;
    int i, t;

    input_file = fopen(input_name, "rb");
    if (!input_file) return -1;
    out_file = fopen(out_name, "wb");

    if (!out_file) {
        fclose(input_file);
        return -1;
    }

    memset(window, 0, WINDOW_SIZE);
    win_pos = 0;
    after_size = 0;

    while (1) {
        b1 = fgetc(input_file);
        if (b1 == EOF) break;

        if (b1 == 0x00) {
            solo = fgetc(input_file);

            if (solo == EOF) {
                fclose(input_file);
                fclose(out_file);
                return -1;
            }

            fputc(solo, out_file);              
            window[win_pos] = (unsigned char)solo;
            win_pos = (win_pos + 1) & (WINDOW_SIZE - 1);
            after_size++;

        } else {
            len_nosolo = (b1 & 0x0F) + MIN_LEN;
            b2 = fgetc(input_file);
            b3 = fgetc(input_file);

            if (b2 == EOF || b3 == EOF) {
                fclose(input_file);
                fclose(out_file);
                return -1;
            }
            len_back = ((unsigned int)b3 << 8) | (unsigned int)b2;

            for (i = 0; i < len_nosolo; i++) {
                t = (win_pos - (int)len_back + WINDOW_SIZE) & (WINDOW_SIZE - 1);

                fputc(window[t], out_file);      
                window[win_pos] = window[t];
                win_pos = (win_pos + 1) & (WINDOW_SIZE - 1);
                after_size++;
            }
        }
    }

    fclose(input_file);
    fclose(out_file);
    return after_size;
}