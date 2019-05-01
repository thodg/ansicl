#ifndef TAGS_H
#define TAGS_H

#include <setjmp.h>
#include "form.h"

typedef struct env s_env;

typedef struct tags
{
        u_form *tags;
        u_form *go_tag;
        jmp_buf buf;
        struct tags *next;
} s_tags;

void     push_tags (s_tags *tags, s_env *env);
void      pop_tags (s_env *env);
s_tags * find_tag (s_symbol *name, s_tags *tags);

#endif
