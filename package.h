#ifndef PACKAGE_H
#define PACKAGE_H

s_symbol * find_symbol (s_string *s, s_package *pkg);
s_symbol * intern (s_string *s, s_package *pkg);
void unintern (s_string *s, s_package *pkg);

int compare_symbols (void *a, void *b);
void init_packages (void);

#endif
