#ifndef ERROR_H
#define ERROR_H

#include <setjmp.h>
#include "env.h"
#include "form.h"

typedef struct error_handler
{
        jmp_buf buf;
        s_string *string;
        struct error_handler *next;
} s_error_handler;

void push_error_handler (s_error_handler *eh, s_env *env);
void  pop_error_handler (s_env *env);
u_form *  error (s_env *env, const char *msg, ...);

#endif
