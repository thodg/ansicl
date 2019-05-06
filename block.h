#ifndef BLOCK_H
#define BLOCK_H

#include <setjmp.h>
#include "typedefs.h"

struct block {
        s_symbol *name;
        u_form *return_value;
        jmp_buf buf;
        struct block *next;
};

void       push_block (s_block *b, s_symbol *name, s_env *env);
s_block ** find_block (s_symbol *name, s_env *env);
u_form *        block (s_symbol *name, u_form *progn, s_env *env);
void        pop_block (s_symbol *name, s_env *env);

void return_from (s_symbol *name, u_form *value, s_env *env);

#endif
