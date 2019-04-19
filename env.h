#ifndef ENV_H
#define ENV_H

#include "read.h"

typedef struct env
{
        s_standard_input si;
        int run;
} s_env;

extern s_env g_env;

void env_init (s_env *env);

#endif
