#ifndef UNWIND_PROTECT_H
#define UNWIND_PROTECT_H

#include <setjmp.h>
#include "typedefs.h"

struct unwind_protect {
        jmp_buf buf;
        jmp_buf *jmp;
        s_unwind_protect *next;
};

void push_unwind_protect (s_unwind_protect *up, s_env *env);
void  pop_unwind_protect (s_env *env);
u_form *  unwind_protect (u_form *form, u_form *body, s_env *env);

void long_jump (jmp_buf *buf, s_env *env);

#endif
