
#include <stdlib.h>
#include "backtrace.h"
#include "env.h"
#include "frame.h"
#include "lambda.h"

void push_backtrace_frame (u_form *fun, u_form *vars,
                           s_env *env)
{
        s_backtrace_frame *bf = malloc(sizeof(s_backtrace_frame));
        if (bf) {
                bf->fun = fun;
                bf->vars = vars;
                bf->next = env->backtrace;
                env->backtrace = bf;
        }
}

void pop_backtrace_frame (s_env *env)
{
        if (env->backtrace)
                env->backtrace = env->backtrace->next;
}
