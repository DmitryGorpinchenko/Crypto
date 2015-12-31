#ifndef ORACLE_H
#define ORACLE_H

#include <curl/curl.h>

extern char *host_name;
extern char *url_hdr;

char *decrypt(const char *hex_cipher);

#endif
