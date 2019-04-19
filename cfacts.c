
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "env.h"
#include "form.h"
#include "package.h"
#include "read.h"
#include "eval.h"
#include "print.h"

int repl (s_env *env)
{
        while (env->run) {
                u_form *r = read_form(&env->si);
                if (!r) {
                        env->run = 0;
                        break;
                }
                u_form *e = eval(r, env);
                print(e);
        }
        return 0;
}

int main ()
{
        s_env env;
        srand(42);
        init_packages();
        env_init(&env);
        using_history();
        return repl(&env);
}
