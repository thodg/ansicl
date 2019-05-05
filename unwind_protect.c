
#include "env.h"
#include "eval.h"
#include "unwind_protect.h"

void push_unwind_protect (s_unwind_protect *up, s_env *env)
{
        up->next = env->unwind_protect;
        env->unwind_protect = up;
}

void pop_unwind_protect (s_env *env)
{
        if (env->unwind_protect)
                env->unwind_protect = env->unwind_protect->next;
}

u_form * unwind_protect (u_form *form, u_form *body, s_env *env)
{
        u_form *f = nil();
        s_unwind_protect up;
        if (setjmp(up.buf)) {
                pop_unwind_protect(env);
                cspecial_progn(body, env);
                longjmp(*up.jmp, 1);
        }
        f = eval(form, env);
        pop_unwind_protect(env);
        cspecial_progn(body, env);
        return f;
}

void long_jump (jmp_buf *buf, s_env *env)
{
        if (env->unwind_protect && *buf > env->unwind_protect->buf) {
                s_unwind_protect *up = env->unwind_protect;
                while (up->next && *buf > up->next->buf) {
                        up->jmp = &up->next->buf;
                        up = up->next;
                }
                up->jmp = buf;
                longjmp(env->unwind_protect->buf, 1);
        }
        longjmp(*buf, 1);
}
