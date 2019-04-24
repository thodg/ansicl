
#include <assert.h>
#include <stdlib.h>
#include "env.h"
#include "eval.h"
#include "package.h"

s_env g_env;

s_env * clone_env (s_env *env)
{
        s_env *e = malloc(sizeof(s_env));
        if (e) {
                e->si = env->si;
                e->run = env->run;
                e->variables = env->variables;
                e->functions = env->functions;
                e->macros = env->macros;
        }
        return e;
}

u_form * symbol_value (s_symbol *sym, s_env *env)
{
        u_form *f;
        if (!env)
                env = &g_env;
        f = assoc((u_form*) sym, env->variables);
        if (f && f->type == FORM_CONS)
                return f->cons.cdr;
        return NULL;
}

u_form * symbol_function (s_symbol *sym, s_env *env)
{
        u_form *f;
        if (!env)
                env = &g_env;
        f = assoc((u_form*) sym, env->functions);
        if (f && f->type == FORM_CONS)
                return f->cons.cdr;
        return NULL;
}

u_form * symbol_macro (s_symbol *sym, s_env *env)
{
        u_form *f;
        if (!env)
                env = &g_env;
        f = assoc((u_form*) sym, env->macros);
        if (f && f->type == FORM_CONS)
                return f->cons.cdr;
        return NULL;
}

u_form * defvar (s_symbol *name, u_form *value)
{
        u_form *f = symbol_value(name, &g_env);
        if (!f) {
                f = (u_form*) new_cons((u_form*) name, value);
                g_env.variables = (u_form*) new_cons(f,
                                                     g_env.variables);
        }
        return (u_form*) name;
}

u_form * setq (s_symbol *name, u_form *value, s_env *env)
{
        u_form *f;
        if (!env)
                env = &g_env;
        f = assoc((u_form*) name, env->variables);
        if (!f || f->type != FORM_CONS)
                return error("unbound symbol %s", name->string->str);
        f->cons.cdr = value;
        return value;
}

u_form * defparameter (s_symbol *name, u_form *value)
{
        u_form *f = symbol_value(name, &g_env);
        if (!f) {
                f = (u_form*) new_cons((u_form*) name, value);
                g_env.variables = (u_form*) new_cons(f,
                                                     g_env.variables);
        }
        else
                setq(name, value, &g_env);
        return (u_form*) name;
}

void let_ (u_form *name, u_form *value, s_env *env)
{
        u_form *f;
        f = (u_form*) new_cons(name, value);
        env->variables = (u_form*) new_cons(f,
                                            env->variables);
}

u_form * let (u_form *bindings, u_form *body, s_env *env)
{
        s_env *e = clone_env(env);
        while (bindings && bindings->type == FORM_CONS) {
                if (bindings->cons.car->type == FORM_CONS)
                        let_(caar(bindings),
                             eval(cadar(bindings), env), e);
                else
                        let_(bindings->cons.car, nil(), e);
                bindings = bindings->cons.cdr;
        }
        return eval_progn(body, e);
}

void cfun (const char *name, f_cfun *fun)
{
        u_form *name_sym = (u_form*) sym(name);
        u_form *cf = malloc(sizeof(s_cfun));
        u_form *c;
        assert(cf);
        cf->type = FORM_CFUN;
        cf->cfun.fun = fun;
        c = (u_form*) new_cons(name_sym, cf);
        g_env.functions = (u_form*) new_cons(c, g_env.functions);
}

u_form * defun (s_symbol *name, u_form *lambda_list, u_form *body)
{
        (void) lambda_list;
        (void) body;
        return (u_form*) name;
}

u_form * defmacro (s_symbol *name, u_form *lambda_list, u_form *body)
{
        (void) lambda_list;
        (void) body;
        return (u_form*) name;
}

void env_init (s_env *env, s_standard_input *si)
{
        env->si = si;
        env->run = 1;
        env->variables = nil();
        env->functions = nil();
        env->macros = nil();
        cfun("quote", cfun_quote);
        cfun("atom", cfun_atom);
        cfun("eq", cfun_eq);
        cfun("car", cfun_car);
        cfun("cdr", cfun_cdr);
        cfun("cons", cfun_cons);
        cfun("cond", cfun_cond);
        cfun("assoc", cfun_assoc);
        cfun("let", cfun_let);
        cfun("defvar", cfun_defvar);
        cfun("defparameter", cfun_defparameter);
        cfun("setq", cfun_setq);
}
