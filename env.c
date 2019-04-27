
#include <assert.h>
#include <stdlib.h>
#include "env.h"
#include "eval.h"
#include "lambda.h"
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

void frame_new_variable (s_symbol *sym, u_form *value, s_frame *frame)
{
        u_form *a = (u_form*) new_cons((u_form*) sym, value);
        frame->variables = (u_form*) new_cons(a, frame->variables);
}

void frame_new_function (s_symbol *sym, u_form *value, s_frame *frame)
{
        u_form *a = (u_form*) new_cons((u_form*) sym, value);
        frame->functions = (u_form*) new_cons(a, frame->functions);
}

void frame_new_macro (s_symbol *sym, u_form *value, s_frame *frame)
{
        u_form *a = (u_form*) new_cons((u_form*) sym, value);
        frame->macros = (u_form*) new_cons(a, frame->macros);
}

u_form ** frame_variable (s_symbol *sym, s_frame *frame)
{
        while (frame) {
                u_form *f;
                f = assoc((u_form*) sym, frame->variables);
                if (consp(f))
                        return &f->cons.cdr;
                frame = frame->parent;
        }
        return NULL;
}

u_form ** frame_function (s_symbol *sym, s_frame *frame)
{
        while (frame) {
                u_form *f;
                f = assoc((u_form*) sym, frame->functions);
                if (consp(f))
                        return &f->cons.cdr;
                frame = frame->parent;
        }
        return NULL;
}

u_form ** frame_macro (s_symbol *sym, s_frame *frame)
{
        while (frame) {
                u_form *f;
                f = assoc((u_form*) sym, frame->macros);
                if (consp(f))
                        return &f->cons.cdr;
                frame = frame->parent;
        }
        return NULL;
}

u_form ** symbol_variable (s_symbol *sym, s_env *env)
{
        return frame_variable(sym, env->frame);
}

u_form ** symbol_function (s_symbol *sym, s_env *env)
{
        return frame_function(sym, env->frame);
}

u_form * symbol_function_ (s_symbol *sym, s_env *env)
{
        u_form **f = frame_function(sym, env->frame);
        return f ? *f : NULL;
}

u_form ** symbol_macro (s_symbol *sym, s_env *env)
{
        return frame_macro(sym, env->frame);
}

u_form ** symbol_special (s_symbol *sym, s_env *env)
{
        u_form *f;
        f = assoc((u_form*) sym, env->specials);
        if (consp(f))
                return &f->cons.cdr;
        return NULL;
}

u_form * defvar (s_symbol *name, u_form *value, s_env *env)
{
        u_form **f = frame_variable(name, env->global_frame);
        if (!f)
                frame_new_variable(name, value, env->global_frame);
        return (u_form*) name;
}

u_form * setq (s_symbol *name, u_form *value, s_env *env)
{
        u_form **f = symbol_variable(name, env);
        if (!f)
                return error("unbound symbol %s", name->string->str);
        *f = value;
        return value;
}

u_form * defparameter (s_symbol *name, u_form *value, s_env *env)
{
        u_form **f = frame_variable(name, env->global_frame);
        if (!f)                
                frame_new_variable(name, value, env->global_frame);
        else
                *f = value;
        return (u_form*) name;
}

u_form * let_star (u_form *bindings, u_form *body, s_env *env)
{
        s_frame *frame = env->frame;
        s_frame *f = new_frame(env->frame);
        u_form *r;
        env->frame = f;
        while (consp(bindings)) {
                u_form *name;
                u_form *value = nil();
                if (consp(bindings->cons.car)) {
                        name = bindings->cons.car->cons.car;
                        if (consp(bindings->cons.car->cons.cdr))
                                value = eval(cadar(bindings), env);
                } else
                        name = bindings->cons.car;
                if (!symbolp(name)) {
                        env->frame = frame;
                        return error("invalid let* binding");
                }
                frame_new_variable(&name->symbol, value, f);
                bindings = bindings->cons.cdr;
        }
        r = cspecial_progn(body, env);
        env->frame = frame;
        return r;
}

u_form * let (u_form *bindings, u_form *body, s_env *env)
{
        s_frame *frame = env->frame;
        s_frame *f = new_frame(env->frame);
        u_form *r;
        while (consp(bindings)) {
                u_form *name;
                u_form *value = nil();
                if (consp(bindings->cons.car)) {
                        name = bindings->cons.car->cons.car;
                        if (consp(bindings->cons.car->cons.cdr))
                                value = eval(cadar(bindings), env);
                } else
                        name = bindings->cons.car;
                if (!symbolp(name))
                        return error("invalid let binding");
                frame_new_variable(&name->symbol, value, f);
                bindings = bindings->cons.cdr;
        }
        env->frame = f;
        r = cspecial_progn(body, env);
        env->frame = frame;
        return r;
}

void cfun (const char *name, f_cfun *fun)
{
        u_form *name_sym = (u_form*) sym(name);
        u_form *cf = malloc(sizeof(s_cfun));
        u_form *c;
        assert(cf);
        cf->type = FORM_CFUN;
        cf->cfun.name = sym(name);
        cf->cfun.fun = fun;
        c = (u_form*) new_cons(name_sym, cf);
        g_env.global_frame->functions = (u_form*)
                new_cons(c, g_env.global_frame->functions);
}

void cspecial (const char *name, f_cfun *fun)
{
        u_form *name_sym = (u_form*) sym(name);
        u_form *cf = malloc(sizeof(s_cfun));
        u_form *c;
        assert(cf);
        cf->type = FORM_CFUN;
        cf->cfun.name = sym(name);
        cf->cfun.fun = fun;
        c = (u_form*) new_cons(name_sym, cf);
        g_env.specials = (u_form*) new_cons(c, g_env.specials);
}

u_form * defun (s_symbol *name, u_form *lambda_list, u_form *body,
                s_env *env)
{
        s_lambda *l = new_lambda(sym("function"), name, lambda_list,
                                 body, env);
        s_closure *c = new_closure(l);
        frame_new_function(name, (u_form*) c, env->global_frame);
        return (u_form*) name;
}

u_form * function (s_symbol *name, s_env *env)
{
        return *frame_function(name, env->frame);
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
        env->specials = nil();
        cspecial("quote",        cspecial_quote);
        cfun("atom",  cfun_atom);
        cfun("eq",    cfun_eq);
        cfun("car",   cfun_car);
        cfun("cdr",   cfun_cdr);
        cfun("cons",  cfun_cons);
        cspecial("cond",         cspecial_cond);
        cspecial("progn",        cspecial_progn);
        cfun("find",  cfun_find);
        cfun("assoc", cfun_assoc);
        cspecial("let",          cspecial_let);
        cspecial("let*",         cspecial_let_star);
        cspecial("defvar",       cspecial_defvar);
        cspecial("defparameter", cspecial_defparameter);
        cspecial("setq",         cspecial_setq);
        cspecial("lambda",       cspecial_lambda);
        cspecial("defun",        cspecial_defun);
        cspecial("function",     cspecial_function);
        cfun("eval", cfun_eval);
        cfun("apply", cfun_apply);
}

s_frame * push_frame (s_env *env)
{
        return env->frame = new_frame(env->frame);
}

void pop_frame (s_env *env)
{
        env->frame = env->frame->parent;
}
