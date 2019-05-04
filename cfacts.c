
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "env.h"
#include "error.h"
#include "form.h"
#include "package.h"
#include "read.h"
#include "eval.h"
#include "print.h"

int repl (s_env *env)
{
        while (env->run) {
                s_error_handler eh;
                push_error_handler(&eh, env);
                if (setjmp(eh.buf)) {
                        fputs("cfacts: ", stderr);
                        fputs(eh.string->str, stderr);
                        fputs("\n", stderr);
                }
                else {
                        u_form *r;
                        u_form *e;
                        if (!(r = read_form(env->si, env))) {
                                env->run = 0;
                                break;
                        }
                        e = eval(r, env);
                        prin1(e, stdout, env);
                        puts("");
                }
        }
        return 0;
}

void init_standard_input (s_standard_input *si)
{
        si->s = NULL;
        si->start = 0;
        si->end = 0;
        si->in_cons = 0;
}

int main ()
{
        s_standard_input si;
        srand(42);
        init_packages();
        init_standard_input(&si);
        env_init(&g_env, &si);
        using_history();
        return repl(&g_env);
}
