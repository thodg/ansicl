#ifndef ENV_H
#define ENV_H

#include "read.h"

typedef struct frame
{
        u_form *variables;
        u_form *functions;
        u_form *macros;
        struct frame *parent;
} s_frame;

s_frame * new_frame (s_frame *parent);
void          frame_new_variable (s_symbol *sym, u_form *value,
                                  s_frame *frame);
void          frame_new_function (s_symbol *sym, u_form *value,
                                  s_frame *frame);
void          frame_new_macro (s_symbol *sym, u_form *value,
                               s_frame *frame);
u_form **     frame_variable (s_symbol *sym, s_frame *frame);
u_form **     frame_function (s_symbol *sym, s_frame *frame);
u_form **     frame_macro (s_symbol *sym, s_frame *frame);

typedef struct env
{
        s_standard_input *si;
        int run;
        s_frame *frame;
        s_frame *global_frame;
        u_form *specials;
} s_env;

extern s_env g_env;

u_form ** symbol_variable (s_symbol *sym, s_env *env);
u_form * symbol_function_ (s_symbol *sym, s_env *env);
u_form ** symbol_function (s_symbol *sym, s_env *env);
u_form ** symbol_macro (s_symbol *sym, s_env *env);
u_form ** symbol_special (s_symbol *sym, s_env *env);
u_form * setq (s_symbol *sym, u_form *value, s_env *env);
u_form * set_symbol_function (s_symbol *sym, u_form *value, s_env *env);
u_form * set_symbol_macro (s_symbol *sym, u_form *value, s_env *env);
u_form * defvar (s_symbol *name, u_form *value, s_env *env);
u_form * defparameter (s_symbol *name, u_form *value, s_env *env);
u_form * let (u_form *bindings, u_form *body, s_env *env);
u_form * let_star (u_form *bindings, u_form *body, s_env *env);
void env_init (s_env *env, s_standard_input *si);
void cfun (const char *name, f_cfun *f);
void cspecial (const char *name, f_cfun *f);
u_form * defun (s_symbol *name, u_form *lambda_list, u_form *body,
                s_env *env);
u_form * function (s_symbol *name, s_env *env);

#endif
