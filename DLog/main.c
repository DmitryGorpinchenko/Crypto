/*
 * compile with gcc main.c DLog.c -lgmp
 */

#include <stdio.h>	
#include <stdlib.h>	

#include "DLog.h"
 
int main(void) 
{              		
    char *p_str = "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084171";
    char *h_str = "3239475104050450443565264378728065788649097520952449527834792452971981976143292558073856937958553180532878928001494706097394108577585732452307673444020333";
    char *g_str = "11717829880366207009516117596335367088558084999998952205599979459063929499736583746670572176471460312928594829675428279466566527115212748467589894601965568";

    mpz_t p, h, g;
	
    mpz_init_set_str(p, p_str, 10);	
    mpz_init_set_str(g, g_str, 10);
    mpz_init_set_str(h, h_str, 10);	

    //initialize to NULL pointers			
    Hashtable *table = init_Hashtable();			

    mpz_t g_inv, tmp, acc;

    mpz_init(g_inv);
    mpz_init(tmp);
    mpz_init_set_ui(acc, 1);
    mpz_invert(g_inv, g, p);

    for (int i = 0; i <= (1 << 20); i++) {
        mpz_mul(tmp, acc, h);
        mpz_mod(tmp, tmp, p);
        put(table, tmp, i);
        mpz_mul(acc, acc, g_inv);
        mpz_mod(acc, acc, p);
    }
	
    mpz_t gB;
    mpz_init(gB);
    mpz_powm_ui(gB, g, 1 << 20, p);

    mpz_t x0, x1;

    mpz_set_ui(acc, 1);

    int value;

    for (int i = 0; i <= (1 << 20); i++) {
        if ((value = lookup(table, acc)) >= 0) {
            mpz_init_set_ui(x0, i);
            mpz_init_set_ui(x1, value);
            break;
        }
        mpz_mul(acc, acc, gB);
        mpz_mod(acc, acc, p);
    }
	
    mpz_mul_ui(tmp, x0, 1 << 20);
    mpz_add(tmp, tmp, x1);

    char *dlog = mpz_get_str(NULL, 10, tmp);
		
    printf("dlog = %s\n", dlog);

    mpz_clear(tmp);
    mpz_clear(acc);
    mpz_clear(x0);
    mpz_clear(x1);
    mpz_clear(g_inv);
    mpz_clear(gB);
    mpz_clear(p);
    mpz_clear(g);
    mpz_clear(h);
    free(dlog);
    free_Hashtable(table);
    return 0;
}

