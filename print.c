
#include <stdio.h>
#include "eval.h"
#include "lambda.h"
#include "package.h"
#include "print.h"

void prin1_cons (s_cons *cons)
{
        u_form *quote_sym = NULL;
        if (!quote_sym)
                quote_sym = (u_form*) sym("quote");
        if (cons->car == quote_sym && cons->cdr->type == FORM_CONS &&
            cons->cdr->cons.cdr == nil()) {
                putchar('\'');
                prin1(cons->cdr->cons.car);
                return;
        }
        putchar('(');
        prin1(cons->car);
        while (cons->cdr && cons->cdr->type == FORM_CONS) {
                putchar(' ');
                cons = &cons->cdr->cons;
                prin1(cons->car);
        }
        if (cons->cdr != nil()) {
                fputs(" . ", stdout);
                prin1(cons->cdr);
        }
        fputs(")", stdout);
}

void prin1_string (s_string *s)
{
        unsigned long len = s->length;
        char *c = s->str;
        putchar('"');
        while (len--) {
                if (*c == '"')
                        putchar('\\');
                putchar(*c++);
        }
        fputs("\"", stdout);
}

void prin1_symbol (s_symbol *sym)
{
        fputs(sym->string->str, stdout);
}

void prin1_package (s_package *pkg)
{
        fputs("#<package ", stdout);
        fputs(pkg->name->string->str, stdout);
        fputs(">", stdout);
}

void prin1_cfun (s_cfun *cf)
{
        fputs("#<cfun ", stdout);
        fputs(cf->name->string->str, stdout);
        fputs(">", stdout);
}

void prin1_closure (s_closure *c)
{
        fputs("#<", stdout);
        prin1_symbol(c->lambda->type);
        fputs(" ", stdout);
        prin1_symbol(c->lambda->name);
        fputs(">", stdout);
}

void prin1 (u_form *f)
{
        if (!f) {
                return;
        }
        switch (f->type) {
        case FORM_CONS:
                prin1_cons((s_cons*)f);
                break;
        case FORM_STRING:
                prin1_string((s_string*)f);
                break;
        case FORM_SYMBOL:
                prin1_symbol((s_symbol*)f);
                break;
        case FORM_PACKAGE:
                prin1_package((s_package*)f);
                break;
        case FORM_CFUN:
                prin1_cfun((s_cfun*)f);
                break;
        case FORM_CLOSURE:
                prin1_closure((s_closure*)f);
                break;
        default:
                error("unknown form type");
        }
}
