
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "env.h"
#include "eval.h"
#include "lambda.h"
#include "package.h"

u_form * error (const char *msg, ...)
{
        va_list ap;
        va_start(ap, msg);
        fprintf(stderr, "cfacts: ");
        vfprintf(stderr, msg, ap);
        fprintf(stderr, "\n");
        va_end(ap);
        return nil();
}

u_form * eval_nil (u_form *form)
{
        static u_form *nil_sym = NULL;
        if (!nil_sym)
                nil_sym = (u_form*) sym("nil");
        if (form == nil_sym)
                return nil();
        return NULL;
}

u_form * eval_t (u_form *form)
{
        static u_form *t_sym = NULL;
        if (!t_sym)
                t_sym = (u_form*) sym("t");
        if (form == t_sym)
                return t_sym;
        return NULL;
}

u_form * eval_variable (u_form *form, s_env *env)
{
        if (symbolp(form)) {
                u_form *f = symbol_value(&form->symbol, env);
                if (!f)
                        return error("symbol not bound: %s",
                                     form->symbol.string->str);
                return f;
        }
        return NULL;
}

u_form * eval_macro (u_form *form, s_env *env)
{
        (void) form;
        (void) env;
        return NULL;
}

u_form * mapcar_eval (u_form *list, s_env *env)
{
        u_form *head = nil();
        u_form **tail = &head;
        if (consp(list)) {
                *tail = (u_form*) new_cons(eval(list->cons.car, env),
                                           nil());
                tail = &(*tail)->cons.cdr;
                list = list->cons.cdr;
        }
        return head;
}

u_form * beta (u_form *lambda, u_form *args, s_env *env)
{
        u_form *a = mapcar_eval(args, env);
        (void) lambda;
        return a;
}

u_form * eval_beta (u_form *form, s_env *env)
{
        u_form *lambda_sym = NULL;
        if (!lambda_sym)
                lambda_sym = (u_form*) sym("lambda");
        if (caar(form) == lambda_sym)
                return beta(form->cons.car, form->cons.cdr, env);
        return NULL;
}

u_form * eval_function (u_form *form, s_env *env)
{
        if (consp(form) && symbolp(form->cons.car)) {
                s_symbol *sym = &form->cons.car->symbol;
                u_form *f = symbol_function(sym, env);
                if (!f)
                        return error("function not bound: %s",
                                     sym->string->str);
                if (f->type == FORM_CFUN)
                        return f->cfun.fun(form->cons.cdr, env);
                if (f->type == FORM_CLOSURE) {
                        u_form *args = mapcar_eval(form->cons.cdr,
                                                   env);
                        return apply(f->closure.lambda, args, env);
                }
                return error("unknown function type");
        }
        return NULL;
}

u_form * quote (u_form *x)
{
        static u_form *quote_sym = NULL;
        if (!quote_sym)
                quote_sym = (u_form*) sym("quote");
        return (u_form*) new_cons(quote_sym,
                                  (u_form*) new_cons(x, nil()));
}

u_form * cfun_quote (u_form *args, s_env *env)
{
        (void) env;
        if (!args || args->type != FORM_CONS ||
            args->cons.cdr != nil())
                return error("invalid arguments for quote");
        return args->cons.car;
}

u_form * atom (u_form *form)
{
        static u_form *t = NULL;
        if (!t)
                t = (u_form*) sym("t");
        if (consp(form))
                return nil();
        return t;
}

u_form * cfun_atom (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            args->cons.cdr != nil())
                return error("invalid arguments for atom");
        return atom(eval(args->cons.car, env));
}

u_form * eq (u_form *a, u_form *b) {
        static u_form *t = NULL;
        if (!t)
                t = (u_form*) sym("t");
        if (a == b)
                return t;
        return nil();
}

u_form * cfun_eq (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            !args->cons.cdr || args->cons.cdr->type != FORM_CONS ||
            args->cons.cdr->cons.cdr != nil())
                return error("invalid arguments for eq");
        return eq(eval(args->cons.car, env),
                  eval(args->cons.cdr->cons.car, env));
}

u_form * car (u_form *f)
{
        if (consp(f))
                return f->cons.car;
        return nil();
}

u_form * cfun_car (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            args->cons.cdr != nil())
                return error("invalid arguments for car");
        return car(eval(args->cons.car, env));
}

u_form * cdr (u_form *f)
{
        if (consp(f))
                return f->cons.cdr;
        return nil();
}

u_form * cfun_cdr (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            args->cons.cdr != nil())
                return error("invalid arguments for cdr");
        return cdr(eval(args->cons.car, env));
}

u_form * caar (u_form *f)
{
        if (consp(f) && consp(f->cons.car))
                return f->cons.car->cons.car;
        return nil();
}

u_form * cadr (u_form *f)
{
        if (consp(f) && consp(f->cons.cdr))
                return f->cons.cdr->cons.car;
        return nil();
}

u_form * cdar (u_form *f)
{
        if (consp(f) && consp(f->cons.car))
                return f->cons.car->cons.cdr;
        return nil();
}

u_form * cddr (u_form *f)
{
        if (consp(f) && consp(f->cons.cdr))
                return f->cons.cdr->cons.cdr;
        return nil();
}

u_form * cadar (u_form *f)
{
        if (consp(f) && consp(f->cons.car) &&
            consp(f->cons.car->cons.cdr))
                return f->cons.car->cons.cdr->cons.car;
        return nil();
}

u_form * caddr (u_form *f)
{
        if (consp(f) && consp(f->cons.cdr) &&
            consp(f->cons.cdr->cons.cdr))
                return f->cons.cdr->cons.cdr->cons.car;
        return nil();
}

u_form * cfun_cons (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            !args->cons.cdr || args->cons.cdr->type != FORM_CONS ||
            args->cons.cdr->cons.cdr != nil())
                return error("invalid arguments for cons");
        return (u_form*) new_cons(eval(args->cons.car, env),
                                  eval(args->cons.cdr->cons.car, env));
}

u_form * cfun_cond (u_form *args, s_env *env)
{
        while (args && args->type == FORM_CONS) {
                u_form *f;
                if (!args->cons.car ||
                    args->cons.car->type != FORM_CONS)
                        return error("invalid cond form");
                f = eval(args->cons.car->cons.car, env);
                if (f != nil())
                        return cfun_progn(args->cons.car->cons.cdr,
                                          env);
                args = args->cons.cdr;
        }
        if (args && args->type != FORM_CONS)
                return error("invalid cond form");
        return nil();
}

u_form * find (u_form *item, u_form *list)
{
        while (consp(list)) {
                if (list->cons.car == item)
                        return item;
                list = list->cons.cdr;
        }
        return NULL;
}

u_form * cfun_find (u_form *args, s_env *env)
{
        if (!consp(args) ||
            !consp(args->cons.cdr) ||
            args->cons.cdr->cons.cdr != nil())
                return error("invalid arguments for find");
        return find(eval(args->cons.car, env),
                    eval(args->cons.cdr->cons.car, env));
}

u_form * assoc (u_form *item, u_form *alist)
{
        while (consp(alist) && item != caar(alist)) {
                alist = alist->cons.cdr;
        }
        if (alist && consp(alist) && consp(alist->cons.car) &&
            item == alist->cons.car->cons.car)
                return alist->cons.car;
        return nil();
}

u_form * cfun_assoc (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            !args->cons.cdr || args->cons.cdr->type != FORM_CONS ||
            args->cons.cdr->cons.cdr != nil())
                return error("invalid arguments for assoc");
        return assoc(eval(args->cons.car, env),
                     eval(args->cons.cdr->cons.car, env));
}

u_form * cfun_progn (u_form *form, s_env *env)
{
        u_form *f = nil();
        while (form && form->type == FORM_CONS) {
                f = eval(form->cons.car, env);
                form = form->cons.cdr;
        }
        if (form != nil())
                return error("malformed progn");
        return f;
}

u_form * cfun_setq (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            !args->cons.car || args->cons.car->type != FORM_SYMBOL ||
            !args->cons.cdr || args->cons.cdr->type != FORM_CONS ||
            args->cons.cdr->cons.cdr != nil())
                return error("invalid arguments for setq");
        return setq(&args->cons.car->symbol,
                    eval(args->cons.cdr->cons.car, env),
                    env);
}

u_form * cfun_let (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            !args->cons.cdr || args->cons.cdr->type != FORM_CONS)
                return error("invalid let form");
        return let(args->cons.car, args->cons.cdr, env);
}

u_form * cfun_defvar (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            !args->cons.car || args->cons.car->type != FORM_SYMBOL ||
            !args->cons.cdr || args->cons.cdr->type != FORM_CONS ||
            args->cons.cdr->cons.cdr != nil())
                return error("invalid arguments for defvar");
        return defvar(&args->cons.car->symbol,
                      eval(args->cons.cdr->cons.car, env),
                      env);
}

u_form * cfun_defparameter (u_form *args, s_env *env)
{
        if (!args || args->type != FORM_CONS ||
            !args->cons.car || args->cons.car->type != FORM_SYMBOL ||
            !args->cons.cdr || args->cons.cdr->type != FORM_CONS ||
            args->cons.cdr->cons.cdr != nil())
                return error("invalid arguments for defparameter");
        return defparameter(&args->cons.car->symbol,
                            eval(args->cons.cdr->cons.car, env),
                            env);
}

u_form * cfun_lambda (u_form *args, s_env *env)
{
        if (!consp(args) || !consp(args->cons.cdr))
                return error("invalid lambda form");
        s_lambda *l = new_lambda(sym("lambda"), &nil()->symbol,
                                 args->cons.car, args->cons.cdr,
                                 env);
        return (u_form*) new_closure(l);
}

u_form * cfun_defun (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car) ||
            !consp(args->cons.cdr))
                return error("invalid defun form");
        return defun(&args->cons.car->symbol,
                     args->cons.cdr->cons.car,
                     args->cons.cdr->cons.cdr, env);
}

u_form * cfun_function (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car) ||
            args->cons.cdr != nil())
                return error("invalid function form");
        return function(&args->cons.car->symbol, env);
}

u_form * eval (u_form *form, s_env *env)
{
        u_form *f;
        if ((f = eval_nil(form))) return f;
        if ((f = eval_t(form))) return f;
        if ((f = eval_variable(form, env))) return f;
        if ((f = eval_macro(form, env))) return f;
        if ((f = eval_function(form, env))) return f;
        if ((f = eval_beta(form, env))) return f;
        return form;
}
