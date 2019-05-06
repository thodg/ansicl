#ifndef BACKTRACE_H
#define BACKTRACE_H

#include "form.h"

typedef struct env s_env;

typedef struct backtrace_frame {
        u_form *fun;
        u_form *vars;
        struct backtrace_frame *next;
} s_backtrace_frame;

void push_backtrace_frame (u_form *fun, u_form *vars, s_env *env);
void pop_backtrace_frame (s_env *env);

#endif
