
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

void print_error (s_error_handler *eh, FILE *stream, s_env *env)
{
        s_backtrace_frame *bf;
        fputs("cfacts: ", stream);
        fputs(eh->string->str, stream);
        fputs("\nBacktrace:", stream);
        for (bf = eh->backtrace; bf; bf = bf->next) {
                print((u_form*) bf->lambda, stream, env);
                prin1(bf->frame->variables, stream, env);
        }
        fputs("\n", stream);
}

int repl (s_env *env)
{
        while (env->run) {
                s_error_handler eh;
                push_error_handler(&eh, env);
                if (setjmp(eh.buf))
                        print_error(&eh, stderr, env);
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
