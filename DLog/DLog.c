#include <stdlib.h>	
#include <string.h>

#include "DLog.h"

#define TABLE_SIZE 1000000

struct nlist {
    struct nlist *next;
    mpz_t key;
    size_t value;
}; 
typedef struct nlist nlist;

struct Hashtable {
    nlist *table[TABLE_SIZE];
};

size_t hash(mpz_t big_num);
nlist *lookup_ptr(Hashtable *table, mpz_t big_num);

void put(Hashtable *table, mpz_t key, size_t value) 
{	
    nlist *np;
    size_t hashval;	
    if ((np = lookup_ptr(table, key)) == NULL) {
        np = malloc(sizeof(nlist));
        mpz_init_set(np->key, key);
        np->value = value;
        hashval = hash(key);
        np->next = table->table[hashval];
        table->table[hashval] = np;	
    } else {
        np->value = value;
    }
}

int lookup(Hashtable *table, mpz_t big_num) 
{	
    nlist *np = lookup_ptr(table, big_num);	
    return np ? np->value : -1;
}

nlist *lookup_ptr(Hashtable *table, mpz_t big_num) 
{	
    nlist *np;	
    for (np = table->table[hash(big_num)]; np != NULL; np = np->next) {
        if (mpz_cmp(np->key, big_num) == 0) {
            return np;
        }
    }   	
    return NULL;
}

size_t hash(mpz_t big_num) 
{	
    return mpz_get_ui(big_num) % TABLE_SIZE;
}

Hashtable *init_Hashtable(void) 
{	
    Hashtable *table = (Hashtable *) malloc(sizeof(Hashtable));
    memset(table->table, 0, sizeof(nlist *) * TABLE_SIZE);
    return table;
}

void free_Hashtable(Hashtable *table) 
{	
    nlist *curr;
    nlist *tmp;	
    for (int i = 0; i < TABLE_SIZE; i++) {
        curr = table->table[i];
        while (curr != NULL) {
            tmp = curr->next;
            mpz_clear(curr->key);
            free(curr);
            curr = tmp;
        }
    }
    free(table);
}
