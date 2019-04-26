#ifndef LAMBDA_H
#define LAMBDA_H

#include "form.h"

typedef struct lambda {
        s_symbol *type;
        s_symbol *name;
        u_form *lambda_list;
        u_form *body;
        s_frame *frame;
} s_lambda;

s_lambda * new_lambda (s_symbol *type, s_symbol *name,
                       u_form *lambda_list, u_form *body,
                       s_env *env);
u_form * apply_lambda (s_lambda *lambda, u_form *args, s_env *env);

#endif
