
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "form.h"
#include "package.h"

u_form * nil ()
{
        static u_form *nil_sym = NULL;
        if (!nil_sym)
                nil_sym = (u_form*) sym("nil");
        return nil_sym;
}

s_cons * new_cons (u_form *car, u_form *cdr)
{
        s_cons *cons = malloc(sizeof(s_cons));
        if (cons) {
                cons->type = FORM_CONS;
                cons->car = car;
                cons->cdr = cdr;
        }
        return cons;
}

s_string * init_string (s_string *str, unsigned long length,
                        const char *chars)
{
        str->type = FORM_STRING;
        str->length = length;
        strncpy(str->str, chars, length);
        str->str[length] = 0;
        return str;
}
        
s_string * new_string (unsigned long length, const char *chars)
{
        s_string *str = malloc(sizeof(s_string) + length + 1);
        if (str)
                init_string(str, length, chars);
        return str;
}

s_symbol * new_symbol (s_string *string)
{
        s_symbol *sym = malloc(sizeof(s_symbol));
        if (sym) {
                sym->type = FORM_SYMBOL;
                sym->package = NULL;
                sym->string = string;
        }
        return sym;
}

s_package * new_package (s_symbol *name)
{
        s_package *pkg = malloc(sizeof(s_package));
        if (pkg) {
                pkg->type = FORM_PACKAGE;
                pkg->name = name;
                pkg->symbols = new_skiplist(10, M_E);
                pkg->symbols->compare = compare_symbols;
        }
        return pkg;
}
