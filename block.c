
#include <stdlib.h>
#include <setjmp.h>
#include "block.h"
#include "env.h"
#include "eval.h"

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

u_form * block (s_symbol *name, u_form *progn, s_env *env)
{
        s_block block;
        u_form *f;
        push_block(&block, name, env);
        if (setjmp(block.buf)) {
                block_pop(name, env);
                return block.return_value;
        }
        f = cspecial_progn(progn, env);
        block_pop(name, env);
        return f;
}

u_form * block_pop (s_symbol *name, s_env *env)
{
        s_block **pb = find_block(name, env);
        s_block *b;
        if (!pb)
                return error("no block named %s", name->string->str);
        b = *pb;
        *pb = (*pb)->next;
        return b->return_value;
}

void return_from (s_symbol *name, u_form *value, s_env *env)
{
        s_block **pb = find_block(name, env);
        if (!pb) {
                error("return from unknown block %s", name->string->str);
                return;
        }
        (*pb)->return_value = value;
        longjmp((*pb)->buf, 1);
}
