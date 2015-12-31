#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "openssl/aes.h"
#include "cbc_ctr_AES.h"

struct BlockCipher {
    enc_mode mode;
    uint8_t key[KEY_LENGTH];
};

void cbc_decrypt(BlockCipher *bc, uint8_t *plaintext, uint8_t *ciphertext, int size);
void ctr_decrypt(BlockCipher *bc, uint8_t *plaintext, uint8_t *ciphertext, int size);
void increment_IV(uint8_t *iv);
void _xor(uint8_t *a, uint8_t *b, int len);

BlockCipher *init_BlockCipher(enc_mode mode, char *key) 
{
    int key_size = strlen(key) / 2;

    if (key_size != KEY_LENGTH) {
        return NULL;
    }

    BlockCipher *bc = (BlockCipher *) malloc(sizeof(BlockCipher));

    if (bc != NULL) {
        bc->mode = mode;
        set_bytes(bc->key, key, key_size);
    }
    return bc;
}

void decrypt(BlockCipher *bc, uint8_t *plaintext, uint8_t *ciphertext, int size) 
{    
    if (bc->mode == CBC) {
        cbc_decrypt(bc, plaintext, ciphertext, size);   
    } else if (bc->mode == CTR) {
        ctr_decrypt(bc, plaintext, ciphertext, size);  
    }
}

void cbc_decrypt(BlockCipher *bc, uint8_t *plaintext, uint8_t *ciphertext, int size) 
{    
    AES_KEY dec_key;
    AES_set_decrypt_key(bc->key, KEY_LENGTH * 8, &dec_key);

    int blocks_num = (size - 1) / BLOCK_SIZE;
    uint8_t *c_ptr = ciphertext + BLOCK_SIZE;
    uint8_t *p_ptr = plaintext;

    for (int i = 0; i < blocks_num; i++) {
        AES_decrypt(c_ptr, p_ptr, &dec_key);
        _xor(p_ptr, c_ptr - BLOCK_SIZE, BLOCK_SIZE);
        c_ptr += BLOCK_SIZE;
        p_ptr += BLOCK_SIZE;
    }

    // plaintext[size-1] is reserved for null terminator
    int padding = plaintext[size - 2];
    // set null terminator according to the padding
    plaintext[size - 1 - padding] = '\0';
}

void ctr_decrypt(BlockCipher *bc, uint8_t *plaintext, uint8_t *ciphertext, int size) 
{     
    AES_KEY enc_key;
    AES_set_encrypt_key(bc->key, KEY_LENGTH * 8, &enc_key);

    uint8_t *iv = (uint8_t*) malloc(BLOCK_SIZE);
    memcpy(iv, ciphertext, BLOCK_SIZE);

    // align to block size
    int d = BLOCK_SIZE - (size - 1) % BLOCK_SIZE;
    int blocks_num = (size - 1) / BLOCK_SIZE + (d < BLOCK_SIZE ? 1 : 0);

    uint8_t *c_ptr = ciphertext + BLOCK_SIZE;
    uint8_t *p_ptr = plaintext;
    int i;

    for (i = 0; i < blocks_num; i++) {
        AES_encrypt(iv, p_ptr, &enc_key);
        increment_IV(iv);
        p_ptr += BLOCK_SIZE;
    }

    // size-1 because of null terminator
    _xor(plaintext, c_ptr, size - 1);
    plaintext[size - 1] = '\0';

    free(iv);
}

void increment_IV(uint8_t *iv) 
{
    // due to little endian we need to work with separate bytes instead of uint64_t values
    int i;
    for (i = BLOCK_SIZE - 1; i >= 0; i--) {
        (*(iv + i))++;
        if (*(iv + i)) {
            break; 
        }
    }
}

void _xor(uint8_t *a, uint8_t *b, int len) 
{	
    for (int i = 0; i < len; i++) {
        a[i] = a[i] ^ b[i];
    }
}

void set_bytes(uint8_t *dst, char *src, int size) 
{    
    char *ptr = src;
	
    for (int i = 0; i < size; i++) {
        sscanf(ptr, "%2hhx", dst + i);
        ptr += 2;
    }
}
