#ifndef DLOG_H
#define DLOG_H

#include <gmp.h>

typedef struct Hashtable Hashtable; 

Hashtable *init_Hashtable(void);
void free_Hashtable(Hashtable *table);
int lookup(Hashtable *table, mpz_t big_num);
void put(Hashtable *table, mpz_t key, size_t value);

#endif
