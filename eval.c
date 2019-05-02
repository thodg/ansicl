
#include <stdlib.h>
#include "block.h"
#include "env.h"
#include "error.h"
#include "eval.h"
#include "lambda.h"
#include "package.h"
#include "print.h"
#include "tags.h"
#include "unwind_protect.h"

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
                u_form **f = symbol_variable(&form->symbol, env);
                if (!f)
                        return error(env, "symbol not bound: %s",
                                     form->symbol.string->str);
                return *f;
        }
        return NULL;
}

u_form * mapcar_eval (u_form *list, s_env *env)
{
        u_form *head = nil();
        u_form **tail = &head;
        while (consp(list)) {
                *tail = (u_form*) new_cons(eval(list->cons.car, env),
                                           nil());
                tail = &(*tail)->cons.cdr;
                list = list->cons.cdr;
        }
        return head;
}

u_form * eval_beta (u_form *form, s_env *env)
{
        u_form *lambda_sym = NULL;
        if (!lambda_sym)
                lambda_sym = (u_form*) sym("lambda");
        if (caar(form) == lambda_sym) {
                u_form *f = eval(form->cons.car, env);
                u_form *a = mapcar_eval(form->cons.cdr, env);
                return apply(f, a, env);
        }
        return NULL;
}

u_form * eval_call (u_form *form, s_env *env)
{
        if (consp(form) && symbolp(form->cons.car)) {
                s_symbol *sym = &form->cons.car->symbol;
                u_form **f;
                u_form *a;
                if ((f = symbol_special(sym, env)))
                        return (*f)->cfun.fun(form->cons.cdr, env);
                if ((f = symbol_macro(sym, env)))
                        return eval(apply(*f, form->cons.cdr, env), env);
                if (!(f = symbol_function(sym, env)))
                        return error(env, "function not bound: %s",
                                     sym->string->str);
                a = mapcar_eval(form->cons.cdr, env);
                return apply(*f, a, env);
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

u_form * cspecial_quote (u_form *args, s_env *env)
{
        (void) env;
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for quote");
        return args->cons.car;
}

u_form * backquote (u_form *x)
{
        static u_form *backquote_sym = NULL;
        if (!backquote_sym)
                backquote_sym = (u_form*) sym("backquote");
        return (u_form*) new_cons(backquote_sym,
                                  (u_form*) new_cons(x, nil()));
}

u_form * comma_at (u_form *x)
{
        static u_form *comma_at_sym = NULL;
        if (!comma_at_sym)
                comma_at_sym = (u_form*) sym("comma_at");
        return (u_form*) new_cons(comma_at_sym,
                                  (u_form*) new_cons(x, nil()));
}

u_form * comma_dot (u_form *x)
{
        static u_form *comma_dot_sym = NULL;
        if (!comma_dot_sym)
                comma_dot_sym = (u_form*) sym("comma_dot");
        return (u_form*) new_cons(comma_dot_sym,
                                  (u_form*) new_cons(x, nil()));
}

u_form * comma (u_form *x)
{
        static u_form *comma_sym = NULL;
        if (!comma_sym)
                comma_sym = (u_form*) sym("comma");
        return (u_form*) new_cons(comma_sym,
                                  (u_form*) new_cons(x, nil()));
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
        (void) env;
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for atom");
        return atom(args->cons.car);
}

u_form * eq (u_form *a, u_form *b) {
        static u_form *t = NULL;
        if (!t)
                t = (u_form*) sym("t");
        if (a == b)
                return t;
        if (integerp(a) && integerp(b) &&
            a->lng.lng == b->lng.lng)
                return t;
        if (floatp(a) && floatp(b) &&
            a->dbl.dbl == b->dbl.dbl)
                return t;
        return nil();
}

u_form * cfun_eq (u_form *args, s_env *env)
{
        (void) env;
        if (!consp(args) || !consp(args->cons.cdr) ||
            args->cons.cdr->cons.cdr != nil())
                return error(env, "invalid arguments for eq");
        return eq(args->cons.car,
                  args->cons.cdr->cons.car);
}

u_form * cons (u_form *car, u_form *cdr)
{
        return (u_form*) new_cons(car, cdr);
}

u_form * car (u_form *f)
{
        if (consp(f))
                return f->cons.car;
        return nil();
}

u_form * cfun_car (u_form *args, s_env *env)
{
        (void) env;
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for car");
        return car(args->cons.car);
}

u_form * cdr (u_form *f)
{
        if (consp(f))
                return f->cons.cdr;
        return nil();
}

u_form * cfun_cdr (u_form *args, s_env *env)
{
        (void) env;
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for cdr");
        return cdr(args->cons.car);
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

u_form * cdddr (u_form *f)
{
        if (consp(f) && consp(f->cons.cdr) &&
            consp(f->cons.cdr->cons.cdr))
                return f->cons.cdr->cons.cdr->cons.cdr;
        return nil();
}

u_form * cfun_cons (u_form *args, s_env *env)
{
        (void) env;
        if (!consp(args) || !consp(args->cons.cdr) ||
            args->cons.cdr->cons.cdr != nil())
                return error(env, "invalid arguments for cons");
        return (u_form*) new_cons(args->cons.car,
                                  args->cons.cdr->cons.car);
}

u_form * cspecial_cond (u_form *args, s_env *env)
{
        while (consp(args)) {
                u_form *f;
                if (!consp(args->cons.car))
                        return error(env, "invalid cond form");
                f = eval(args->cons.car->cons.car, env);
                if (f != nil())
                        return cspecial_progn(args->cons.car->cons.cdr,
                                              env);
                args = args->cons.cdr;
        }
        if (args && args->type != FORM_CONS)
                return error(env, "invalid cond form");
        return nil();
}

u_form * cspecial_if (u_form *args, s_env *env)
{
        u_form *test;
        if (!consp(args) || (!consp(args->cons.cdr)) ||
            cdddr(args) != nil())
                return error(env, "invalid if form");
        test = eval(args->cons.car, env);
        if (test != nil())
                return eval(args->cons.cdr->cons.car, env);
        if (consp(args->cons.cdr->cons.cdr))
                return eval(args->cons.cdr->cons.cdr->cons.car, env);
        return nil();
}

u_form * cspecial_and (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        u_form *test;
        if (!t_sym)
                t_sym = (u_form*) sym("t");
        test = t_sym;
        while (consp(args) && test != nil()) {
                test = eval(args->cons.car, env);
                args = args->cons.cdr;
        }
        return test;
}


u_form * cspecial_or (u_form *args, s_env *env)
{
        u_form *test = nil();
        while (consp(args) && test == nil()) {
                test = eval(args->cons.car, env);
                args = args->cons.cdr;
        }
        return test;
}

u_form * cfun_not (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        if (!t_sym)
                t_sym = (u_form*) sym("t");
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for not");
        if (args->cons.car == nil())
                return t_sym;
        return nil();
}

u_form * cfun_consp (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        if (!t_sym)
                t_sym = (u_form*) sym("t");
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for consp");
        if (consp(args->cons.car))
                return t_sym;
        return nil();
}

u_form * cfun_stringp (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        if (!t_sym)
                t_sym = (u_form*) sym("t");
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for stringp");
        if (stringp(args->cons.car))
                return t_sym;
        return nil();
}

u_form * cfun_symbolp (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        if (!t_sym)
                t_sym = (u_form*) sym("t");
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for symbolp");
        if (symbolp(args->cons.car))
                return t_sym;
        return nil();
}

u_form * cfun_packagep (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        if (!t_sym)
                t_sym = (u_form*) sym("t");
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for packagep");
        if (packagep(args->cons.car))
                return t_sym;
        return nil();
}

u_form * cfun_functionp (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        if (!t_sym)
                t_sym = (u_form*) sym("t");
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for functionp");
        if (functionp(args->cons.car))
                return t_sym;
        return nil();
}

u_form * cspecial_prog1 (u_form *form, s_env *env)
{
        u_form *result = NULL;
        while (consp(form)) {
                u_form *f = eval(form->cons.car, env);
                if (!result)
                        result = f;
                form = form->cons.cdr;
        }
        if (!result)
                return error(env, "malformed prog1");
        if (form != nil())
                return error(env, "malformed prog1");
        return result;
}

u_form * cspecial_progn (u_form *form, s_env *env)
{
        u_form *f = nil();
        while (consp(form)) {
                f = eval(form->cons.car, env);
                form = form->cons.cdr;
        }
        if (form != nil())
                return error(env, "malformed progn");
        return f;
}

u_form * cfun_make_symbol (u_form *args, s_env *env)
{
        (void) env;
        if (!consp(args) || !stringp(args->cons.car) ||
            args->cons.cdr != nil())
                return error(env, "invalid arguments for make-symbol");
        return (u_form*) new_symbol(&args->cons.car->string);
}

u_form * cfun_list (u_form *args, s_env *env)
{
        (void) env;
        return args;
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
        (void) env;
        if (!consp(args) || !consp(args->cons.cdr) ||
            args->cons.cdr->cons.cdr != nil())
                return error(env, "invalid arguments for find");
        return find(args->cons.car,
                    args->cons.cdr->cons.car);
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
        (void) env;
        if (!consp(args) || !consp(args->cons.cdr) ||
            args->cons.cdr->cons.cdr != nil())
                return error(env, "invalid arguments for assoc");
        return assoc(args->cons.car,
                     args->cons.cdr->cons.car);
}

u_form * last (u_form *x)
{
        if (!consp(x))
                return nil();
        while (consp(x) && consp(x->cons.cdr))
                x = x->cons.cdr;
        return x;
}

u_form * cfun_last (u_form *args, s_env *env)
{
        (void) env;
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for last");
        return (u_form*) last(args->cons.car);
}

long length (u_form *x)
{
        long len = 0;
        while (consp(x)) {
                len++;
                x = x->cons.cdr;
        }
        return len;
}

u_form * cfun_length (u_form *args, s_env *env)
{
        (void) env;
        if (!consp(args) || !listp(args->cons.car) ||
            args->cons.cdr != nil())
                return error(env, "invalid arguments for length");
        return (u_form*) new_long(length(args->cons.car));
}

u_form * cspecial_setq (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car) ||
            !consp(args->cons.cdr) ||
            args->cons.cdr->cons.cdr != nil())
                return error(env, "invalid arguments for setq");
        return setq(&args->cons.car->symbol,
                    eval(args->cons.cdr->cons.car, env),
                    env);
}

u_form * cspecial_let (u_form *args, s_env *env)
{
        if (!consp(args))
                return error(env, "invalid let form");
        return let(args->cons.car, args->cons.cdr, env);
}

u_form * cspecial_let_star (u_form *args, s_env *env)
{
        if (!consp(args))
                return error(env, "invalid let* form");
        return let_star(args->cons.car, args->cons.cdr, env);
}

u_form * cspecial_defvar (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car) ||
            !consp(args->cons.cdr) ||
            args->cons.cdr->cons.cdr != nil())
                return error(env, "invalid arguments for defvar");
        return defvar(&args->cons.car->symbol,
                      eval(args->cons.cdr->cons.car, env),
                      env);
}

u_form * cspecial_defparameter (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car) ||
            !consp(args->cons.cdr) ||
            args->cons.cdr->cons.cdr != nil())
                return error(env, "invalid arguments for defparameter");
        return defparameter(&args->cons.car->symbol,
                            eval(args->cons.cdr->cons.car, env),
                            env);
}

u_form * cspecial_block (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car))
                return error(env, "invalid block form");
        return block(&args->cons.car->symbol,
                     args->cons.cdr, env);
}

u_form * cspecial_return_from (u_form *args, s_env *env)
{
        s_symbol *block_name;
        u_form *value = nil();
        if (!consp(args) || !symbolp(args->cons.car) ||
            consp(cddr(args)))
                return error(env, "invalid return_from form");
        block_name = &args->cons.car->symbol;
        if (consp(args->cons.cdr))
                value = eval(args->cons.cdr->cons.car, env);
        return_from(block_name, value, env);
        return nil();
}

u_form * cspecial_return (u_form *args, s_env *env)
{
        u_form *value = nil();
        if (consp(cdr(args)))
                return error(env, "invalid arguments for return");
        if (consp(args))
                value = eval(args->cons.car, env);
        return_from(&nil()->symbol, value, env);
        return nil();
}

u_form * copy_list (u_form *list)
{
        u_form *head = nil();
        u_form **tail = &head;
        while (consp(list)) {
                *tail = cons(list->cons.car, nil());
                tail = &(*tail)->cons.cdr;
                list = list->cons.cdr;
        }
        return head;
}

u_form * cspecial_tagbody (u_form *args, s_env *env)
{
        u_form *body = copy_list(args);
        s_unwind_protect up;
        s_tags tags;
        u_form **b = &body;
        u_form *f;
        while (consp(*b)) {
                if (symbolp((*b)->cons.car)) {
                        tags.tags = cons(*b, tags.tags);
                        *b = (*b)->cons.cdr;
                }
                else
                        b = &(*b)->cons.cdr;
        }
        tags.go_tag = NULL;
        if (setjmp(tags.buf)) {
                f = cspecial_progn(tags.go_tag->cons.cdr, env);
                pop_unwind_protect(env);
                pop_tags(env);
                return f;
        }
        push_tags(&tags, env);
        if (setjmp(up.buf)) {
                pop_unwind_protect(env);
                pop_tags(env);
                longjmp(*up.jmp, 1);
        }
        push_unwind_protect(&up, env);
        f = cspecial_progn(body, env);
        pop_unwind_protect(env);
        pop_tags(env);
        return f;
}

u_form * cspecial_go (u_form *args, s_env *env)
{
        s_symbol *name;
        s_tags *tags;
        if (!consp(args) || !symbolp(args->cons.car) ||
            args->cons.cdr != nil())
                return error(env, "invalid go form");
        name = &args->cons.car->symbol;
        if (!(tags = find_tag(name, env->tags)))
                return error(env, "go to nonexistent label %s",
                             name->string->str);
        long_jump(&tags->buf, env);
        return nil();
}

u_form * cspecial_lambda (u_form *args, s_env *env)
{
        if (!consp(args) || !consp(args->cons.cdr))
                return error(env, "invalid lambda form");
        s_lambda *l = new_lambda(sym("lambda"), &nil()->symbol,
                                 args->cons.car, args->cons.cdr,
                                 env);
        return (u_form*) new_closure(l);
}

u_form * cspecial_defun (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car) ||
            !consp(args->cons.cdr))
                return error(env, "invalid defun form");
        return defun(&args->cons.car->symbol,
                     args->cons.cdr->cons.car,
                     args->cons.cdr->cons.cdr, env);
}

u_form * cspecial_function (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car) ||
            args->cons.cdr != nil())
                return error(env, "invalid function form");
        return function(&args->cons.car->symbol, env);
}

u_form * cspecial_defmacro (u_form *args, s_env *env)
{
        if (!consp(args) || !symbolp(args->cons.car) ||
            !consp(args->cons.cdr))
                return error(env, "invalid defmacro form");
        return defmacro(&args->cons.car->symbol,
                        args->cons.cdr->cons.car,
                        args->cons.cdr->cons.cdr, env);
}

u_form * cspecial_labels (u_form *args, s_env *env)
{
        if (!consp(args))
                return error(env, "invalid labels form");
        return labels(args->cons.car, args->cons.cdr, env);
}

u_form * cspecial_flet (u_form *args, s_env *env)
{
        if (!consp(args))
                return error(env, "invalid flet form");
        return flet(args->cons.car, args->cons.cdr, env);
}

u_form * cfun_error (u_form *args, s_env *env)
{
        if (!consp(args) || !stringp(args->cons.car) ||
            args->cons.cdr != nil())
                return error(env, "invalid arguments for error");
        return error_(&args->cons.car->string, env);
}

u_form * apply (u_form *fun, u_form *args, s_env *env)
{
        if (fun->type == FORM_SYMBOL)
                fun = symbol_function_(&fun->symbol, env);
        if (fun->type == FORM_CFUN)
                return fun->cfun.fun(args, env);
        if (car(fun) == (u_form*) sym("lambda"))
                fun = eval(fun, env);
        if (fun->type == FORM_CLOSURE) {
                return apply_lambda(fun->closure.lambda, args, env);
        }
        return error(env, "apply argument is not a function");
}

u_form * cfun_apply (u_form *args, s_env *env)
{
        u_form *l;
        if (!consp(args))
                return error(env, "invalid apply call");
        l = last(args);
        if (consp(l) && consp(l->cons.car)) {
                l->cons.cdr = l->cons.car->cons.cdr;
                l->cons.car = l->cons.car->cons.car;
        }
        return apply(args->cons.car, args->cons.cdr, env);
}

u_form * eval (u_form *form, s_env *env)
{
        u_form *f;
        if ((f = eval_nil(form))) return f;
        if ((f = eval_t(form))) return f;
        if ((f = eval_variable(form, env))) return f;
        if ((f = eval_call(form, env))) return f;
        if ((f = eval_beta(form, env))) return f;
        return form;
}

u_form * cfun_eval (u_form *args, s_env *env)
{
        if (!consp(args) ||
            args->cons.cdr != nil())
                return error(env, "invalid arguments for eval");
        return eval(args->cons.car, env);
}

u_form * cfun_prin1 (u_form *args, s_env *env)
{
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for prin1");
        prin1(args->cons.car, env);
        return nil();
}

u_form * cfun_print (u_form *args, s_env *env)
{
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for print");
        print(args->cons.car, env);
        return nil();
}

u_form * cfun_plus (u_form *args, s_env *env)
{
        int d = 0;
        u_form *a = args;
        while (consp(a)) {
                if (floatp(a->cons.car))
                        d = 1;
                else if (!integerp(a->cons.car))
                        return error(env, "invalid arguments for +");
                a = a->cons.cdr;
        }
        if (d) {
                a = (u_form*) new_double(0.0);
                while (consp(args)) {
                        if (floatp(args->cons.car))
                                a->dbl.dbl += args->cons.car->dbl.dbl;
                        else
                                a->dbl.dbl += args->cons.car->lng.lng;
                        args = args->cons.cdr;
                }
                return a;
        }
        a = (u_form*) new_long(0);
        while (consp(args)) {
                a->lng.lng += args->cons.car->lng.lng;
                args = args->cons.cdr;
        }
        return a;
}

u_form * cfun_minus (u_form *args, s_env *env)
{
        int d = 0;
        u_form *a = args;
        while (consp(a)) {
                if (floatp(a->cons.car))
                        d = 1;
                else if (!integerp(a->cons.car))
                        return error(env, "invalid arguments for -");
                a = a->cons.cdr;
        }
        if (!consp(args))
                return error(env, "invalid arguments for -");
        if (d) {
                a = (u_form*) new_double(floatp(args->cons.car) ?
                                         args->cons.car->dbl.dbl :
                                         args->cons.car->lng.lng);
                args = args->cons.cdr;
                while (consp(args)) {
                        if (floatp(args->cons.car))
                                a->dbl.dbl -= args->cons.car->dbl.dbl;
                        else
                                a->dbl.dbl -= args->cons.car->lng.lng;
                        args = args->cons.cdr;
                }
                return a;
        }
        a = (u_form*) new_long(floatp(args->cons.car) ?
                               args->cons.car->dbl.dbl :
                               args->cons.car->lng.lng);
        args = args->cons.cdr;
        while (consp(args)) {
                a->lng.lng -= args->cons.car->lng.lng;
                args = args->cons.cdr;
        }
        return a;
}

u_form * cfun_mul (u_form *args, s_env *env)
{
        int d = 0;
        u_form *a = args;
        while (consp(a)) {
                if (floatp(a->cons.car))
                        d = 1;
                else if (!integerp(a->cons.car))
                        return error(env, "invalid arguments for *");
                a = a->cons.cdr;
        }
        if (d) {
                a = (u_form*) new_double(1.0);
                while (consp(args)) {
                        if (floatp(args->cons.car))
                                a->dbl.dbl *= args->cons.car->dbl.dbl;
                        else
                                a->dbl.dbl *= args->cons.car->lng.lng;
                        args = args->cons.cdr;
                }
                return a;
        }
        a = (u_form*) new_long(1);
        while (consp(args)) {
                a->lng.lng *= args->cons.car->lng.lng;
                args = args->cons.cdr;
        }
        return a;
}

u_form * cfun_div (u_form *args, s_env *env)
{
        int d = 0;
        u_form *a = args;
        while (consp(a)) {
                if (floatp(a->cons.car))
                        d = 1;
                else if (!integerp(a->cons.car))
                        return error(env, "invalid arguments for -");
                a = a->cons.cdr;
        }
        if (!consp(args))
                return error(env, "invalid arguments for -");
        if (d) {
                a = (u_form*) new_double(floatp(args->cons.car) ?
                                         args->cons.car->dbl.dbl :
                                         args->cons.car->lng.lng);
                args = args->cons.cdr;
                while (consp(args)) {
                        if (floatp(args->cons.car))
                                a->dbl.dbl /= args->cons.car->dbl.dbl;
                        else
                                a->dbl.dbl /= args->cons.car->lng.lng;
                        args = args->cons.cdr;
                }
                return a;
        }
        a = (u_form*) new_long(floatp(args->cons.car) ?
                               args->cons.car->dbl.dbl :
                               args->cons.car->lng.lng);
        args = args->cons.cdr;
        while (consp(args)) {
                a->lng.lng /= args->cons.car->lng.lng;
                args = args->cons.cdr;
        }
        return a;
}
