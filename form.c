
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "form.h"
#include "package.h"

u_form * nil ()
{
        static u_form n;
        n.type = FORM_NULL;
        return &n;
}

u_form * new_cons (u_form *car, u_form *cdr)
{
        u_form *f = malloc(sizeof(u_form));
        if (f) {
                f->type = FORM_CONS;
                f->cons.car = car;
                f->cons.cdr = cdr;
        }
        return f;
}

u_form * new_string (unsigned long length, const char *str)
{
        u_form *f = malloc(sizeof(s_string) + length + 1);
        if (f) {
                f->type = FORM_STRING;
                f->string.length = length;
                strncpy(f->string.str, str, length);
                f->string.str[length] = 0;
        }
        return f;
}

u_form * new_symbol (s_string *string)
{
        u_form *f = malloc(sizeof(u_form));
        if (f) {
                f->type = FORM_SYMBOL;
                f->symbol.package = NULL;
                f->symbol.string = string;
        }
        return f;
}

u_form * new_package (s_symbol *name)
{
        u_form *f = malloc(sizeof(u_form));
        if (f) {
                f->type = FORM_PACKAGE;
                f->package.name = name;
                f->package.symbols = new_skiplist(10, M_E);
                f->package.symbols->compare = compare_symbols;
        }
        return f;
}
