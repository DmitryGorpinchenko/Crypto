#ifndef RSA_H
#define RSA_H

#include <gmp.h>

typedef struct RSA RSA;

RSA *init_RSA(char *p, char *q, mpz_t e);
void free_RSA(RSA *rsa);
char *decrypt(RSA *rsa, char *cipher);

#endif
