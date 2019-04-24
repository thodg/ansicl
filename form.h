#ifndef FORM_H
#define FORM_H

#include <skiplist.h>

typedef struct cons    s_cons;
typedef struct string  s_string;
typedef struct symbol  s_symbol;
typedef struct package s_package;
typedef struct cfun    s_cfun;

typedef union form u_form;

typedef struct env s_env;
typedef u_form * f_cfun (u_form *args, s_env *env);

struct cons {
        unsigned long type;
        u_form *car;
        u_form *cdr;
};

struct string {
        unsigned long type;
        unsigned long length;
        char str[];
};

struct symbol {
        unsigned long type;
        s_package *package;
        s_string *string;
};

struct package {
        unsigned long type;
        s_symbol *name;
        s_skiplist *symbols;
};

struct cfun {
        unsigned long type;
        f_cfun *fun;
};

union form {
        unsigned long type;
        s_cons cons;
        s_string string;
        s_symbol symbol;
        s_package package;
        s_cfun cfun;
};

enum e_form_type { FORM_NULL,
                   FORM_CONS,
                   FORM_STRING,
                   FORM_SYMBOL,
                   FORM_PACKAGE,
                   FORM_CFUN };

s_string *  init_string (s_string *s, unsigned long length,
                         const char *str);

u_form *    nil ();
s_cons *    new_cons (u_form *car, u_form *cdr);
s_string *  new_string (unsigned long length, const char *str);
s_symbol *  new_symbol (s_string *string);
s_package * new_package (s_symbol *name);

#endif
