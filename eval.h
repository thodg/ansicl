#ifndef EVAL_H
#define EVAL_H

#include "env.h"
#include "form.h"

u_form * error (const char *msg, ...);
u_form * quote (u_form *form);
u_form * atom (u_form *form);
u_form * eq (u_form *a, u_form *b);
u_form * car (u_form *form);
u_form * cdr (u_form *form);
u_form * caar (u_form *form);
u_form * cadr (u_form *form);
u_form * cddr (u_form *form);
u_form * cadar (u_form *form);
u_form * caddr (u_form *form);
u_form * assoc (u_form *item, u_form *alist);

u_form * cspecial_quote (u_form *args, s_env *env);
u_form * cfun_atom (u_form *args, s_env *env);
u_form * cfun_eq (u_form *args, s_env *env);
u_form * cfun_car (u_form *args, s_env *env);
u_form * cfun_cdr (u_form *args, s_env *env);
u_form * cfun_cons (u_form *args, s_env *env);
u_form * cspecial_cond (u_form *args, s_env *env);

u_form * cspecial_progn (u_form *form, s_env *env);
u_form * cfun_make_symbol (u_form *args, s_env *env);
u_form * cfun_list (u_form *args, s_env *env);
u_form * cfun_find (u_form *args, s_env *env);
u_form * cfun_assoc (u_form *args, s_env *env);
u_form * cfun_last (u_form *args, s_env *env);

u_form * cspecial_let (u_form *args, s_env *env);
u_form * cspecial_let_star (u_form *args, s_env *env);
u_form * cspecial_defvar (u_form *args, s_env *env);
u_form * cspecial_defparameter (u_form *args, s_env *env);
u_form * cspecial_setq (u_form *args, s_env *env);

u_form * cspecial_lambda (u_form *args, s_env *env);
u_form * cspecial_defun (u_form *args, s_env *env);
u_form * cspecial_function (u_form *args, s_env *env);
u_form * cspecial_defmacro (u_form *args, s_env *env);
u_form * cspecial_return_from (u_form *args, s_env *env);
u_form * cspecial_return (u_form *args, s_env *env);

u_form * cfun_eval (u_form *args, s_env *env);
u_form * cfun_apply (u_form *args, s_env *env);

u_form * eval (u_form *form, s_env *env);
u_form * apply (u_form *fun, u_form *args, s_env *env);

#endif
