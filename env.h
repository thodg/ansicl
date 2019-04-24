#ifndef ENV_H
#define ENV_H

#include "read.h"

typedef struct env
{
        s_standard_input *si;
        int run;
        u_form *variables;
        u_form *functions;
        u_form *macros;
} s_env;

extern s_env g_env;

void env_init (s_env *env, s_standard_input *si);
s_env * clone_env (s_env *env);
u_form * symbol_value (s_symbol *sym, s_env *env);
u_form * symbol_function (s_symbol *sym, s_env *env);
u_form * symbol_macro (s_symbol *sym, s_env *env);
u_form * setq (s_symbol *sym, u_form *value, s_env *env);
u_form * set_symbol_function (s_symbol *sym, u_form *value, s_env *env);
u_form * set_symbol_macro (s_symbol *sym, u_form *value, s_env *env);
u_form * defvar (s_symbol *name, u_form *value);
u_form * defparameter (s_symbol *name, u_form *value);
u_form * let (u_form *bindings, u_form *body, s_env *env);
void cfun (const char *name, f_cfun *f);


#endif
