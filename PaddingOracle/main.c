/*
 * compile: gcc -fopenmp main.c PaddingOracle.c -lcurl
 */

#include <stdio.h>
#include <stdlib.h>

#include "PaddingOracle.h"
 
const char *hex_cipher = "f20bdba6ff29eed7b046d1df9fb7000058b1ffb4210a580f748b4ac714c001bd4a61044426fb515dad3f21f18aa577c0bdf302936266926ff37dbf7035d5eeb4";  
 
int main(void) 
{
    curl_global_init(CURL_GLOBAL_ALL);
    host_name = "http://crypto-class.appspot.com";
    url_hdr   = "http://crypto-class.appspot.com/po?er=";

    struct timeval tim;
    gettimeofday(&tim, NULL);
    double t1 = tim.tv_sec + (tim.tv_usec / 1000000.0);

    char *msg = decrypt(hex_cipher);

    gettimeofday(&tim, NULL);
    double t2 = tim.tv_sec + (tim.tv_usec / 1000000.0);

    printf("\nTiming: %.6lf seconds\n", t2 - t1);	
    printf("\n[%s]\n\n", msg);

    free(msg);
    curl_global_cleanup();
    return 0;
}

