#ifndef READ_H
#define READ_H

#include "form.h"

typedef struct env s_env;

typedef struct standard_input {
        char *s;
        unsigned long start;
        unsigned long end;
        int in_cons;
} s_standard_input;

u_form * read_form (s_standard_input *si, s_env *env);

#endif
