#ifndef FORM_H
#define FORM_H

#include <skiplist.h>

typedef struct cons    s_cons;
typedef struct string  s_string;
typedef struct symbol  s_symbol;
typedef struct package s_package;
typedef struct cfun    s_cfun;
typedef struct lambda  s_lambda;
typedef struct lng     s_long;
typedef struct dbl     s_double;

typedef union form u_form;

typedef struct env s_env;
typedef u_form * f_cfun (u_form *args, s_env *env);
typedef struct frame s_frame;

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
        s_symbol *name;
        f_cfun *fun;
};

struct lambda {
        unsigned long type;
        s_symbol *lambda_type;
        s_symbol *name;
        u_form *lambda_list;
        u_form *body;
        s_frame *frame;
};

struct lng {
        unsigned long type;
        long lng;
};

struct dbl {
        unsigned long type;
        double dbl;
};

union form {
        unsigned long type;
        s_cons cons;
        s_string string;
        s_symbol symbol;
        s_package package;
        s_cfun cfun;
        s_lambda lambda;
        s_long lng;
        s_double dbl;
};

enum e_form_type { FORM_CONS,
                   FORM_STRING,
                   FORM_SYMBOL,
                   FORM_PACKAGE,
                   FORM_CFUN,
                   FORM_LAMBDA,
                   FORM_LONG,
                   FORM_DOUBLE };

#define null(x)    ((x) == nil())
#define consp(x)   ((x) && (x)->type == FORM_CONS)
#define listp(x)   ((x) && ((x)->type == FORM_CONS || x == nil()))
#define stringp(x) ((x) && (x)->type == FORM_STRING)
#define symbolp(x) ((x) && (x)->type == FORM_SYMBOL)
#define packagep(x) ((x) && (x)->type == FORM_PACKAGE)
#define functionp(x) ((x) && ((x)->type == FORM_CFUN ||         \
                              (x)->type == FORM_LAMBDA))
#define integerp(x) ((x) && (x)->type == FORM_LONG)
#define floatp(x) ((x) && (x)->type == FORM_DOUBLE)
#define numberp(x) (integerp(x) || floatp(x))

#define push(place, x) place = cons(x, place)

s_string *  init_string (s_string *s, unsigned long length,
                         const char *str);

u_form *    nil ();
s_cons *    new_cons (u_form *car, u_form *cdr);
s_string *  new_string (unsigned long length, const char *str);
s_symbol *  new_symbol (s_string *string);
s_package * new_package (s_symbol *name);
s_lambda *  new_lambda (s_symbol *type, s_symbol *name,
                        u_form *lambda_list, u_form *body,
                        s_env *env);
s_long *    new_long (long lng);
s_double *  new_double (double dbl);

#endif
