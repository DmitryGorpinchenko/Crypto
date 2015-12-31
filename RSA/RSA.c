#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RSA.h"

struct RSA {
    mpz_t N;
    mpz_t p;
    mpz_t q;
    mpz_t e;
    mpz_t d;
    mpz_t phi;
};

char *decrypt(RSA *rsa, char *cipher) 
{	
    mpz_t c;
    mpz_init_set_str(c, cipher, 10);	
    mpz_t m;
    mpz_init(m);

    mpz_powm(m, c, rsa->d, rsa->N);

    char *msg = mpz_get_str(NULL, 16, m);
    char *ptr = strstr(msg, "00");
    ptr += 2;
    int len = strlen(ptr) / 2;

    char *plaintext = malloc(len + 1);

    for (int i = 0; i < len; i++) {
        sscanf(ptr, "%2hhx", plaintext + i);
        ptr += 2;
    }
	
    plaintext[len] = 0;
	
    mpz_clear(c);
    mpz_clear(m);
    free(msg);
    return plaintext;
}

RSA *init_RSA(char *p, char *q, mpz_t e) 
{
    RSA *rsa = (RSA *) malloc(sizeof(RSA));

    mpz_t ONE;
    mpz_init_set_ui(ONE, 1);

    mpz_init_set(rsa->e, e);
    mpz_init_set_str(rsa->p, p, 10);
    mpz_init_set_str(rsa->q, q, 10);
    mpz_init(rsa->N);
    mpz_mul(rsa->N, rsa->p, rsa->q);
    mpz_init_set(rsa->phi, rsa->N);
    mpz_sub(rsa->phi, rsa->phi, rsa->p);
    mpz_sub(rsa->phi, rsa->phi, rsa->q);
    mpz_add(rsa->phi, rsa->phi, ONE);
    mpz_init(rsa->d);
    mpz_invert(rsa->d, rsa->e, rsa->phi);

    mpz_clear(ONE);	
    return rsa;
}

void free_RSA(RSA *rsa) 
{	
    mpz_clear(rsa->N);
    mpz_clear(rsa->p);
    mpz_clear(rsa->q);
    mpz_clear(rsa->e);
    mpz_clear(rsa->d);
    mpz_clear(rsa->phi);

    free(rsa);
}
