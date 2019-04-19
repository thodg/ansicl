#ifndef FORM_H
#define FORM_H

#include <skiplist.h>

typedef struct cons    s_cons;
typedef struct string  s_string;
typedef struct symbol  s_symbol;
typedef struct package s_package;

typedef union form u_form;

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

union form {
        unsigned long type;
        s_cons cons;
        s_string string;
        s_symbol symbol;
        s_package package;
};

enum e_form_type { FORM_NULL,
                   FORM_CONS,
                   FORM_STRING,
                   FORM_SYMBOL,
                   FORM_PACKAGE };

u_form * nil ();
u_form * new_cons (u_form *car, u_form *cdr);
u_form * new_string (unsigned long length, const char *str);
u_form * new_symbol (s_string *string);
u_form * new_package (s_symbol *name);

#endif
