#ifndef READ_H
#define READ_H

#include <stdio.h>
#include "form.h"

typedef struct env s_env;

typedef struct stream {
        char *s;
        size_t n;
        unsigned long start;
        unsigned long end;
        int in_cons;
        FILE *fp;
        const char *prompt;
        const char *file_name;
        unsigned long line;
} s_stream;

s_stream * stream_readline (const char *prompt);
s_stream * stream_stdin ();
s_stream * stream_open (const char *file_name, s_env *env);
void       stream_close (s_stream *stream);

u_form * read_form (s_stream *stream, s_env *env);

u_form * load_stream (s_stream *stream, s_env *env);
u_form * load_file (const char *path, s_env *env);

#endif
