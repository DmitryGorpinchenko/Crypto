/* 
 * compile with gcc main.c cbc_ctr_AES.c -lssl -lcrypto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cbc_ctr_AES.h"

int main(void) 
{
    /* CBC input setup */
    char *src_key = "140b41b22a29beb4061bda66b6747e14";
    char *src_cipher = "4ca00ff4c898d61e1edbf1800618fb2828a226d160dad07883d04e008a7897ee2e4b7465d5290d0c0e6c6822236e1daafb94ffe0c5da05d9476be028ad7c1d81";
    //char *src_cipher = "5b68629feb8606f9a6667670b75b38a5b4832d0f26e1ab7da33249de7d4afc48e713ac646ace36e872ad5fb8a512428a6e21364b0c374df45503473c5242a253";
    enc_mode mode = CBC;

    /* CTR input setup */
    //char *src_key = "36f18357be4dbd77f050515c73fcf9f2";
    //char *src_cipher = "69dda8455c7dd4254bf353b773304eec0ec7702330098ce7f7520d1cbbb20fc388d1b0adb5054dbd7370849dbf0b88d393f252e764f1f5f7ad97ef79d59ce29f5f51eeca32eabedd9afa9329";
    //char *src_cipher = "770b80259ec33beb2561358a9f2dc617e46218c0a53cbeca695ae45faa8952aa0e311bde9d4e01726d3184c34451";
    //enc_mode mode = CTR;

    BlockCipher *bc = init_BlockCipher(mode, src_key);

    if (bc != NULL) { 
        int cipher_len = strlen(src_cipher) / 2;
        uint8_t *ciphertext = (uint8_t*) malloc(cipher_len);

        set_bytes(ciphertext, src_cipher, cipher_len);

        int text_size  = cipher_len - BLOCK_SIZE + 1;

        //align to the block size
        int d = BLOCK_SIZE - (text_size - 1) % BLOCK_SIZE;
        uint8_t *plaintext = (uint8_t *) malloc(text_size + (d < BLOCK_SIZE ? d : 0));

        decrypt(bc, plaintext, ciphertext, text_size);

        printf("%s\n", plaintext);
        printf("%lu\n", strlen(plaintext));

        free(ciphertext);
        free(plaintext);
    }

    free(bc);
    return 0;
}

