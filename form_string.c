
#include <stdlib.h>
#include <string.h>
#include "form_string.h"

s_string * string_append (s_string *s, const char *str,
                          unsigned long len)
{
        s = realloc(s, sizeof(s_string) + s->length + len);
        strncpy(s->str + s->length, str, len);
        s->str[s->length + len] = 0;
        s->length += len;
        return s;
}
