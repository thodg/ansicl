
#include <stdlib.h>
#include "env.h"
#include "eval.h"
#include "tags.h"

void push_tags (s_tags *tags, s_env *env)
{
        tags->next = env->tags;
        env->tags = tags;
}

void pop_tags (s_env *env)
{
        env->tags = env->tags->next;
}

s_tags * find_tag (s_symbol *name, s_tags *tags)
{
        while (tags) {
                u_form *a = assoc((u_form*) name, tags->tags);
                if (a != nil()) {
                        tags->go_tag = a;
                        return tags;
                }
                tags = tags->next;
        }
        return NULL;
}
