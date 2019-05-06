#ifndef BACKTRACE_H
#define BACKTRACE_H

#include "form.h"
#include "typedefs.h"

struct backtrace_frame {
        u_form *fun;
        u_form *vars;
        struct backtrace_frame *next;
};

void push_backtrace_frame (u_form *fun, u_form *vars, s_env *env);
void pop_backtrace_frame (s_env *env);

#endif
