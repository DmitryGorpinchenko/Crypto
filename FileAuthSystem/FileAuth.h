#ifndef FILE_AUTH_H
#define FILE_AUTH_H

#include <stdint.h>
#include <openssl/sha.h>

void sign(uint8_t *h0, char *filename);
int verify(uint8_t *h0, char *filename);
void print_hex(uint8_t *h, int size);

#endif
