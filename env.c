
#include <assert.h>
#include <stdlib.h>
#include "env.h"
#include "eval.h"
#include "package.h"

s_env g_env;

s_frame * new_frame (s_frame *parent)
{
        s_frame *f = malloc(sizeof(s_frame));
        if (f) {
                f->variables = nil();
                f->functions = nil();
                f->macros = nil();
                f->parent = parent;
        }
        return f;
}

u_form * frame_value (s_symbol *sym, s_frame *frame)
{
        u_form *f;
        f = assoc((u_form*) sym, frame->variables);
        if (f && f->type == FORM_CONS)
                return f->cons.cdr;
        return NULL;
}

u_form * frame_function (s_symbol *sym, s_frame *frame)
{
        u_form *f;
        f = assoc((u_form*) sym, frame->functions);
        if (f && f->type == FORM_CONS)
                return f->cons.cdr;
        return NULL;
}

u_form * frame_macro (s_symbol *sym, s_frame *frame)
{
        u_form *f;
        f = assoc((u_form*) sym, frame->macros);
        if (f && f->type == FORM_CONS)
                return f->cons.cdr;
        return NULL;
}

u_form * symbol_value (s_symbol *sym, s_env *env)
{
        s_frame *frame = env->frame;
        while (frame) {
                u_form *f = frame_value(sym, frame);
                if (f)
                        return f;
                frame = frame->parent;
        }
        return NULL;
}

u_form * symbol_function (s_symbol *sym, s_env *env)
{
        s_frame *frame = env->frame;
        while (frame) {
                u_form *f = frame_function(sym, frame);
                if (f)
                        return f;
                frame = frame->parent;
        }
        return NULL;
}

u_form * symbol_macro (s_symbol *sym, s_env *env)
{
        s_frame *frame = env->frame;
        while (frame) {
                u_form *f = frame_macro(sym, frame);
                if (f)
                        return f;
                frame = frame->parent;
        }
        return NULL;
}

u_form * defvar (s_symbol *name, u_form *value, s_env *env)
{
        u_form *f = frame_value(name, env->global_frame);
        if (!f) {
                f = (u_form*) new_cons((u_form*) name, value);
                env->global_frame->variables = (u_form*)
                        new_cons(f, env->global_frame->variables);
        }
        return (u_form*) name;
}

u_form * setq (s_symbol *name, u_form *value, s_env *env)
{
        u_form *f;
        f = assoc((u_form*) name, env->frame->variables);
        if (!f || f->type != FORM_CONS)
                return error("unbound symbol %s", name->string->str);
        f->cons.cdr = value;
        return value;
}

u_form * defparameter (s_symbol *name, u_form *value, s_env *env)
{
        u_form *f = frame_value(name, env->global_frame);
        if (!f) {
                f = (u_form*) new_cons((u_form*) name, value);
                env->global_frame->variables = (u_form*)
                        new_cons(f, env->global_frame->variables);
        }
        else
                setq(name, value, &g_env);
        return (u_form*) name;
}

void let_ (u_form *name, u_form *value, s_frame *frame)
{
        u_form *f;
        f = (u_form*) new_cons(name, value);
        frame->variables = (u_form*)
                new_cons(f, frame->variables);
}

u_form * let_star (u_form *bindings, u_form *body, s_env *env)
{
        s_frame *f = new_frame(env->frame);
        env->frame = f;
        while (bindings && bindings->type == FORM_CONS) {
                if (bindings->cons.car->type == FORM_CONS)
                        let_(caar(bindings),
                             eval(cadar(bindings), env), f);
                else
                        let_(bindings->cons.car, nil(), f);
                bindings = bindings->cons.cdr;
        }
        return cfun_progn(body, env);
}

u_form * let (u_form *bindings, u_form *body, s_env *env)
{
        s_frame *f = new_frame(env->frame);
        while (bindings && bindings->type == FORM_CONS) {
                if (bindings->cons.car->type == FORM_CONS)
                        let_(caar(bindings),
                             eval(cadar(bindings), env), f);
                else
                        let_(bindings->cons.car, nil(), f);
                bindings = bindings->cons.cdr;
        }
        env->frame = f;
        return cfun_progn(body, env);
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
        g_env.global_frame->functions = (u_form*)
                new_cons(c, g_env.global_frame->functions);
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
        env->frame = env->global_frame = new_frame(NULL);
        cfun("quote", cfun_quote);
        cfun("atom", cfun_atom);
        cfun("eq", cfun_eq);
        cfun("car", cfun_car);
        cfun("cdr", cfun_cdr);
        cfun("cons", cfun_cons);
        cfun("cond", cfun_cond);
        cfun("progn", cfun_progn);
        cfun("assoc", cfun_assoc);
        cfun("let", cfun_let);
        cfun("defvar", cfun_defvar);
        cfun("defparameter", cfun_defparameter);
        cfun("setq", cfun_setq);
}

s_frame * push_frame (s_env *env)
{
        return env->frame = new_frame(env->frame);
}

void pop_frame (s_env *env)
{
        env->frame = env->frame->parent;
}
