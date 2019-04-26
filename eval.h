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

u_form * cfun_quote (u_form *args, s_env *env);
u_form * cfun_atom (u_form *args, s_env *env);
u_form * cfun_eq (u_form *args, s_env *env);
u_form * cfun_car (u_form *args, s_env *env);
u_form * cfun_cdr (u_form *args, s_env *env);
u_form * cfun_cons (u_form *args, s_env *env);
u_form * cfun_cond (u_form *args, s_env *env);

u_form * cfun_progn (u_form *form, s_env *env);
u_form * cfun_find (u_form *args, s_env *env);
u_form * cfun_assoc (u_form *args, s_env *env);

u_form * cfun_let (u_form *args, s_env *env);
u_form * cfun_defvar (u_form *args, s_env *env);
u_form * cfun_defparameter (u_form *args, s_env *env);
u_form * cfun_setq (u_form *args, s_env *env);

u_form * cfun_lambda (u_form *args, s_env *env);
u_form * cfun_defun (u_form *args, s_env *env);

u_form * eval (u_form *form, s_env *env);

#endif
