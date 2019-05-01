#ifndef UNWIND_PROTECT_H
#define UNWIND_PROTECT_H

#include <setjmp.h>

typedef struct env s_env;

typedef struct unwind_protect {
        jmp_buf buf;
        jmp_buf *jmp;
        struct unwind_protect *next;
} s_unwind_protect;

void push_unwind_protect (s_unwind_protect *up, s_env *env);
void  pop_unwind_protect (s_env *env);

void long_jump (jmp_buf *buf, s_env *env);

#endif
