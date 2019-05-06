#ifndef TAGS_H
#define TAGS_H

#include <setjmp.h>
#include "typedefs.h"

struct tags
{
        u_form *tags;
        u_form *go_tag;
        jmp_buf buf;
        s_tags *next;
};

void     push_tags (s_tags *tags, s_env *env);
void      pop_tags (s_env *env);
s_tags * find_tag (s_symbol *name, s_tags *tags);

#endif
