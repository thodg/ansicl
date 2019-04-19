
#include <stdlib.h>
#include "env.h"

void init_standard_input (s_standard_input *si)
{
        si->s = NULL;
        si->start = 0;
        si->end = 0;
        si->in_cons = 0;
}

void env_init (s_env *env)
{
        init_standard_input(&env->si);
        env->run = 1;
}
