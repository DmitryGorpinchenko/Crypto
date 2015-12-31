/*
 * compile: gcc main.c FileAuth.c -lcrypto
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "FileAuth.h"

int main(int argc, char **argv) 
{    
    uint8_t *h0 = (uint8_t *) malloc(SHA256_DIGEST_LENGTH); 
    sign(h0, argv[1]);   
    print_hex(h0, SHA256_DIGEST_LENGTH);

    //test verification procedure
    printf("The file was %s\n", verify(h0, "signed") ? "verified" : "not verified");

    free(h0);
    return 0;
}

