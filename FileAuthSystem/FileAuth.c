#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

#include "FileAuth.h"

#define BLOCK_SIZE 1024

typedef struct {
    uint8_t *content;
    int size;
    int blocks_num;
    int last_block_size;
} Data;

void init_data(Data *data, char *filename, int block_size);
void free_data(Data *data);
uint32_t fsize(const char *filename);

void init_data(Data *data, char *filename, int block_size) 
{    
    FILE *in = fopen(filename, "r");

    int file_size = fsize(filename);
    data->blocks_num = file_size / block_size;
    data->last_block_size = file_size % block_size;
    data->blocks_num += (data->last_block_size ? 1 : 0);

    data->size = file_size + (data->blocks_num - 1) * SHA256_DIGEST_LENGTH;
    data->content = (uint8_t *) malloc(data->size);

    uint8_t *ptr = data->content;
    for (int i = 0; i < data->blocks_num - 1; i++) {
        fread(ptr, block_size, 1, in);
        ptr += (BLOCK_SIZE + SHA256_DIGEST_LENGTH);
    }

    fread(ptr, data->last_block_size, 1, in);

    fclose(in);
}

void free_data(Data *data) 
{   
    free(data->content);    
    free(data);
}

void sign(uint8_t *h0, char *filename) 
{
    Data *data = (Data *) malloc(sizeof(Data));
    init_data(data, filename, BLOCK_SIZE);

    //point to the begining of the very last block which may not be of size BLOCK_SIZE
    uint8_t *ptr = data->content + (data->blocks_num - 1) * (BLOCK_SIZE + SHA256_DIGEST_LENGTH);

    SHA256(ptr, data->last_block_size, ptr - SHA256_DIGEST_LENGTH);

    for (int i = 0; i < data->blocks_num - 2; i++) {
        ptr -= (BLOCK_SIZE + SHA256_DIGEST_LENGTH);
        SHA256(ptr, BLOCK_SIZE + SHA256_DIGEST_LENGTH, ptr - SHA256_DIGEST_LENGTH);
    } 
    ptr -= (BLOCK_SIZE + SHA256_DIGEST_LENGTH);
    SHA256(ptr, BLOCK_SIZE + SHA256_DIGEST_LENGTH, h0); 

    FILE *out = fopen("signed", "w");

    fwrite(data->content, data->size, 1, out);

    fclose(out);
    free_data(data);
}

int verify(uint8_t *h0, char *filename) 
{   
    Data *data = (Data *) malloc(sizeof(Data));
    init_data(data, filename, BLOCK_SIZE + SHA256_DIGEST_LENGTH);

    uint8_t *curr_hash = (uint8_t *) malloc(SHA256_DIGEST_LENGTH);

    uint8_t *hash_ptr = h0;
    uint8_t *data_ptr = data->content;

    int is_valid = 1;

    for (int i = 0; i < data->blocks_num - 1; i++) {
        if (memcmp(SHA256(data_ptr, BLOCK_SIZE + SHA256_DIGEST_LENGTH, curr_hash), hash_ptr, SHA256_DIGEST_LENGTH)) {
            is_valid = 0;
            break;    
        }
        hash_ptr = data_ptr + BLOCK_SIZE;
        data_ptr += (BLOCK_SIZE + SHA256_DIGEST_LENGTH);
    }
    if (is_valid && memcmp(SHA256(data_ptr, data->last_block_size, curr_hash), hash_ptr, SHA256_DIGEST_LENGTH)) {
        is_valid = 0;
    }

    free(curr_hash);
    free_data(data);
    return is_valid; 
}

void print_hex(uint8_t *h, int size) 
{    
    for (int i = 0; i < size; i++) {
        printf("%02x", h[i]);
    }
    printf("\n");
}

uint32_t fsize(const char *filename) 
{
    struct stat st; 
    if (stat(filename, &st) == 0) {
        return st.st_size;    
    }    
    return -1; 
}
