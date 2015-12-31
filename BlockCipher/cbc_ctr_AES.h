#ifndef CTR_CBC_AES_H
#define CTR_CBC_AES_H

#include <stdint.h>

#define BLOCK_SIZE 16
#define KEY_LENGTH 16

typedef enum {CBC, CTR} enc_mode;
typedef struct BlockCipher BlockCipher;

BlockCipher *init_BlockCipher(enc_mode mode, char *key);
void decrypt(BlockCipher *bc, uint8_t *plaintext, uint8_t *ciphertext, int size);
void set_bytes(uint8_t *dst, char *src, int size);

#endif
