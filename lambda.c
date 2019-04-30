
#include <stdlib.h>
#include "block.h"
#include "env.h"
#include "error.h"
#include "eval.h"
#include "lambda.h"

int check_lambda_list (u_form *lambda_list, s_env *env)
{
        while (consp(lambda_list)) {
                if (!symbolp(lambda_list->cons.car)) {
                        error(env, "malformed lambda list");
                        return 1;
                }
                lambda_list = lambda_list->cons.cdr;
        }
        return 0;
}

s_lambda * new_lambda (s_symbol *type, s_symbol *name,
                       u_form *lambda_list, u_form *body,
                       s_env *env)
{
        s_lambda *l;
        if (check_lambda_list(lambda_list, env))
                return NULL;
        if ((l = malloc(sizeof(s_lambda)))) {
                l->type = type;
                l->name = name;
                l->lambda_list = lambda_list;
                l->body = body;
                l->frame = env->frame;
        }
        return l;
}

u_form * apply_lambda (s_lambda *lambda, u_form *args, s_env *env)
{
        s_frame *frame = env->frame;
        u_form *f = lambda->lambda_list;
        u_form *a = args;
        s_block block;
        env->frame = new_frame(lambda->frame);
        while (consp(f) && consp(a)) {
                s_symbol *sym = &f->cons.car->symbol;
                if (!symbolp(sym))
                        return error(env, "invalid lambda list");
                frame_new_variable(sym, a->cons.car, env->frame);
                f = f->cons.cdr;
                a = a->cons.cdr;
        }
        if (consp(f) || consp(a))
                return error(env, "invalid number of arguments");
        push_block(&block, &nil()->symbol, env);
        if (setjmp(block.buf)) {
                block_pop(&nil()->symbol, env);
                env->frame = frame;
                return block.return_value;
        }
        f = cspecial_progn(lambda->body, env);
        block_pop(&nil()->symbol, env);
        env->frame = frame;
        return f;
}
