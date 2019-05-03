
#include <assert.h>
#include <stdlib.h>
#include "env.h"
#include "error.h"
#include "eval.h"
#include "lambda.h"
#include "package.h"
#include "unwind_protect.h"

s_env g_env;

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
                return error(env, "unbound symbol %s", name->string->str);
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
        s_unwind_protect up;
        env->frame = f;
        if (setjmp(up.buf)) {
                pop_unwind_protect(env);
                env->frame = frame;
                longjmp(*up.jmp, 1);
        }
        push_unwind_protect(&up, env);
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
                        return error(env, "invalid let* binding");
                }
                frame_new_variable(&name->symbol, value, f);
                bindings = bindings->cons.cdr;
        }
        r = cspecial_progn(body, env);
        pop_unwind_protect(env);
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
                        return error(env, "invalid let binding");
                frame_new_variable(&name->symbol, value, f);
                bindings = bindings->cons.cdr;
        }
        env->frame = f;
        {
                s_unwind_protect up;
                if (setjmp(up.buf)) {
                        pop_unwind_protect(env);
                        env->frame = frame;
                        longjmp(*up.jmp, 1);
                }
                push_unwind_protect(&up, env);
                r = cspecial_progn(body, env);
                pop_unwind_protect(env);
        }
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
        frame_new_function(name, c, env->global_frame);
        return (u_form*) name;
}

u_form * function (s_symbol *name, s_env *env)
{
        u_form **f = frame_function(name, env->frame);
        if (f)
                return *f;
        return nil();
}

u_form * defmacro (s_symbol *name, u_form *lambda_list, u_form *body,
                   s_env *env)
{
        s_lambda *l = new_lambda(sym("macro"), name, lambda_list,
                                 body, env);
        s_closure *c = new_closure(l);
        frame_new_macro(name, c, env->global_frame);
        return (u_form*) name;
}

u_form * labels (u_form *bindings, u_form *body, s_env *env)
{
        s_frame *frame = env->frame;
        s_frame *f = new_frame(env->frame);
        u_form *r;
        env->frame = f;
        while (consp(bindings)) {
                u_form *first = bindings->cons.car;
                u_form *name;
                u_form *lambda_list;
                u_form *body;
                if (!consp(first) || !consp(first->cons.cdr))
                        return error(env, "invalid binding for labels");
                name = first->cons.car;
                lambda_list = first->cons.cdr->cons.car;
                body = first->cons.cdr->cons.cdr;
                if (!symbolp(name))
                        return error(env, "invalid binding for labels");
                s_lambda *l = new_lambda(sym("labels"), &name->symbol,
                                         lambda_list, body, env);
                s_closure *c = new_closure(l);
                frame_new_function(&name->symbol, c, f);
                bindings = bindings->cons.cdr;
        }
        r = cspecial_progn(body, env);
        env->frame = frame;
        return r;
}

u_form * flet (u_form *bindings, u_form *body, s_env *env)
{
        s_frame *frame = env->frame;
        s_frame *f = new_frame(env->frame);
        u_form *r;
        while (consp(bindings)) {
                u_form *first = bindings->cons.car;
                u_form *name;
                u_form *lambda_list;
                u_form *body;
                if (!consp(first) || !consp(first->cons.cdr))
                        return error(env, "invalid binding for labels");
                name = first->cons.car;
                lambda_list = first->cons.cdr->cons.car;
                body = first->cons.cdr->cons.cdr;
                if (!symbolp(name))
                        return error(env, "invalid binding for labels");
                s_lambda *l = new_lambda(sym("labels"), &name->symbol,
                                         lambda_list, body, env);
                s_closure *c = new_closure(l);
                frame_new_function(&name->symbol, c, f);
                bindings = bindings->cons.cdr;
        }
        env->frame = f;
        r = cspecial_progn(body, env);
        env->frame = frame;
        return r;
}

void env_init (s_env *env, s_standard_input *si)
{
        env->si = si;
        env->run = 1;
        env->frame = env->global_frame = new_frame(NULL);
        env->specials = nil();
        env->tags = NULL;
        cspecial("quote",        cspecial_quote);
        cfun("atom",        cfun_atom);
        cfun("eq",          cfun_eq);
        cfun("car",         cfun_car);
        cfun("cdr",         cfun_cdr);
        cfun("cons",        cfun_cons);
        cspecial("cond",         cspecial_cond);
        cspecial("case",         cspecial_case);
        cspecial("do",           cspecial_do);
        cspecial("if",           cspecial_if);
        cspecial("and",          cspecial_and);
        cspecial("or",           cspecial_or);
        cfun("not",         cfun_not);
        cspecial("prog1",        cspecial_prog1);
        cspecial("progn",        cspecial_progn);
        cfun("make-symbol", cfun_make_symbol);
        cfun("list",        cfun_list);
        cfun("find",        cfun_find);
        cfun("assoc",       cfun_assoc);
        cfun("last",        cfun_last);
        cfun("length",      cfun_length);
        cspecial("let",          cspecial_let);
        cspecial("let*",         cspecial_let_star);
        cspecial("defvar",       cspecial_defvar);
        cspecial("defparameter", cspecial_defparameter);
        cspecial("block",        cspecial_block);
        cspecial("return-from",  cspecial_return_from);
        cspecial("return",       cspecial_return);
        cspecial("tagbody",      cspecial_tagbody);
        cspecial("go",           cspecial_go);
        cspecial("setq",         cspecial_setq);
        cspecial("lambda",       cspecial_lambda);
        cspecial("defun",        cspecial_defun);
        cspecial("function",     cspecial_function);
        cspecial("defmacro",     cspecial_defmacro);
        cspecial("labels",       cspecial_labels);
        cspecial("flet",         cspecial_flet);
        cfun("error",       cfun_error);
        cfun("eval",        cfun_eval);
        cfun("apply",       cfun_apply);
        cfun("prin1",       cfun_prin1);
        cfun("print",       cfun_print);
        cfun("+",           cfun_plus);
        cfun("-",           cfun_minus);
        cfun("*",           cfun_mul);
        cfun("/",           cfun_div);
}

s_frame * push_frame (s_env *env)
{
        return env->frame = new_frame(env->frame);
}

void pop_frame (s_env *env)
{
        env->frame = env->frame->parent;
}
