
#include <stdlib.h>
#include <setjmp.h>
#include "block.h"
#include "env.h"
#include "error.h"
#include "eval.h"
#include "unwind_protect.h"

void push_block (s_block *b, s_symbol *name, s_env *env)
{
        b->name = name;
        b->return_value = nil();
        b->next = env->blocks;
        env->blocks = b;
}

s_block ** find_block (s_symbol *name, s_env *env)
{
        s_block **b = &env->blocks;
        while (*b) {
                if ((*b)->name == name)
                        return b;
                b = &(*b)->next;
        }
        return NULL;
}

u_form * eval_block_body (s_symbol *name, u_form *progn, s_env *env)
{
        s_unwind_protect up;
        u_form *f;
        if (setjmp(up.buf)) {
                pop_unwind_protect(env);
                pop_block(name, env);
                longjmp(*up.jmp, 1);
        }
        push_unwind_protect(&up, env);
        f = cspecial_progn(progn, env);
        pop_unwind_protect(env);
        pop_block(name, env);
        return f;
}

u_form * block (s_symbol *name, u_form *progn, s_env *env)
{
        s_block block;
        push_block(&block, name, env);
        if (setjmp(block.buf)) {
                return block.return_value;
        }
        return eval_block_body(name, progn, env);
}

void pop_block (s_symbol *name, s_env *env)
{
        s_block **pb = find_block(name, env);
        if (!pb)
                error(env, "no block named %s", name->string->str);
        *pb = (*pb)->next;
}

void return_from (s_symbol *name, u_form *value, s_env *env)
{
        s_block **pb = find_block(name, env);
        if (!pb) {
                error(env, "return from unknown block %s",
                      name->string->str);
                return;
        }
        (*pb)->return_value = value;
        long_jump(&(*pb)->buf, env);
}
