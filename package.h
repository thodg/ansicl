#ifndef PACKAGE_H
#define PACKAGE_H

#include "form.h"

s_symbol * find_symbol (s_string *s, s_package *pkg);
s_symbol * find_symbol_ (const char *s, s_package *pkg);
s_symbol * intern (s_string *s, s_package *pkg);
s_symbol * intern_ (const char *s, s_package *pkg);
s_symbol * sym (const char *s);
void unintern (s_string *s, s_package *pkg);

int compare_symbols (void *a, void *b);
s_package * cfacts_package ();
void init_packages (void);

#endif
