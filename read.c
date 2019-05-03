
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "env.h"
#include "error.h"
#include "eval.h"
#include "form.h"
#include "package.h"
#include "read.h"
#include "form_string.h"

int refill (s_standard_input *si)
{
        while (!si->s || si->start == si->end) {
                if (si->s)
                        free(si->s);
                si->s = readline("cfacts> ");
                if (!si->s)
                        return -1;
                add_history(si->s);
                si->start = 0;
                si->end = strlen(si->s);
        }
        return 0;
}

int read_char (s_standard_input *si)
{
        if (refill(si))
                return -1;
        return si->s[si->start++];
}

int peek_char (s_standard_input *si)
{
        if (refill(si))
                return -1;
        return si->s[si->start];
}

int read_spaces (s_standard_input *si)
{
        int c;
        while ((c = peek_char(si)) >= 0)
                switch (c) {
                case ' ': case '\t': case '\r': case '\n':
                        read_char(si);
                        continue;
                default:
                        return 0;
                }
        return -1;
}

u_form * read_cons (s_standard_input *si, s_env *env)
{
        int c = peek_char(si);
        if (c == '(') {
                u_form *head = NULL;
                u_form **tail = &head;
                read_char(si);
                si->in_cons++;
                while (!read_spaces(si) && (c = peek_char(si)) >= 0) {
                        if (c == ')') {
                                read_char(si);
                                *tail = nil();
                                return head;
                        }
                        if (c == '.') {
                                if (!head)
                                        return error(env, "unexpect"
                                                     "ed dot");
                                else {
                                        read_char(si);
                                        if (!(*tail = read_form(si, env)) ||
                                            read_spaces(si) ||
                                            (c = read_char(si)) < 0)
                                                return NULL;
                                        if (c == ')')
                                                return head;
                                        return error(env, "malformed "
                                                     "dotted list");
                                }
                        }
                        if (!(*tail = read_form(si, env)))
                                return NULL;
                        *tail = (u_form*) new_cons(*tail, NULL);
                        tail = &(*tail)->cons.cdr;
                }
        }
        return NULL;
}

u_form * read_string (s_standard_input *si)
{
        u_form *f = NULL;
        unsigned long c;
        if (peek_char(si) == '"') {
                si->start++;
                while (!refill(si)) {
                        for (c = si->start + 1; c < si->end &&
                                     si->s[c] != '"'; c++)
                                ;
                        if (f)
                                f = (u_form*)
                                        string_append((s_string*) f,
                                                      si->s + si->start,
                                                      c - si->start);
                        else
                                f = (u_form*)new_string(c - si->start,
                                                        si->s + si->start);
                        if (si->s[c] == '"') {
                                si->start = c + 1;
                                return f;
                        }
                        f = (u_form*) string_append((s_string*) f, "\n",
                                                    1);
                        si->start = c;
                }
        }
        return NULL;
}

u_form * read_sharp (s_standard_input *si, s_env *env)
{
        if (peek_char(si) == '#') {
                int c;
                read_char(si);
                c = peek_char(si);
                switch (c) {
                case '\'':
                        read_char(si);
                        return cons_function(read_form(si, env));
                        break;
                }
                return error(env, "undefined # macro character %c", c);
        }
        return NULL;
}

int endchar (int c)
{
        return c == '(' || c == ')' || c == '"' || c == ' ' ||
                c == '\t' || c == '\r' || c == '\n' || c == '\'';
}

u_form * read_number (s_standard_input *si)
{
        u_form *f = NULL;
        unsigned long i = si->start;
        char *end;
        int j = 0;
        while (i < si->end && '0' <= si->s[i] && si->s[i] <= '9')
                i++;
        if (i < si->end && si->s[i] == '.') {
                f = (u_form*) new_double(strtod(si->s + si->start,
                                                &end));
                j = end - (si->s + si->start);
        }
        else if (i > si->start) {
                f = (u_form*) new_long(strtol(si->s + si->start,
                                              &end, 10));
                j = end - (si->s + si->start);
        }
        if (j > 0) {
                si->start += j;
                return f;
        }
        return NULL;
}

u_form * read_symbol (s_standard_input *si)
{
        u_form *f;
        unsigned long i = si->start;
        unsigned long j = i;
        while (i < si->end && si->s[i] != '(' && si->s[i] != ')'
               && si->s[i] != '"' && si->s[i] != ' '
               && si->s[i] != '\t' && si->s[i] != '\r'
               && si->s[i] != '\n' && si->s[i] != '\'') {
                j = i;
                i++;
        }
        j = i;
        f = (u_form*) new_string(j - si->start, si->s + si->start);
        f = (u_form*) intern(&f->string, NULL);
        si->start = j;
        return f;
}

u_form * read_quote (s_standard_input *si, s_env *env)
{
        if (peek_char(si) == '\'') {
                read_char(si);
                u_form *f = cons_quote(read_form(si, env));
                return f;
        }
        return NULL;
}

u_form * read_backquote (s_standard_input *si, s_env *env)
{
        if (peek_char(si) == '`') {
                read_char(si);
                u_form *f = backquote(read_form(si, env));
                return f;
        }
        return NULL;
}

u_form * read_comma (s_standard_input *si, s_env *env)
{
        if (peek_char(si) == ',') {
                read_char(si);
                switch (peek_char(si)) {
                case '@':
                        read_char(si);
                        return comma_at(read_form(si, env));
                case '.':
                        read_char(si);
                        return comma_dot(read_form(si, env));
                default:
                        return comma(read_form(si, env));
                }
        }
        return NULL;
}

void read_errors (s_standard_input *si, s_env *env)
{
        if (peek_char(si) == ')') {
                read_char(si);
                error(env, "unmatched close parenthesis");
        }
}

u_form * read_form (s_standard_input *si, s_env *env)
{
        u_form *f;
        if (read_spaces(si))
                return NULL;
        if ((f = read_quote(si, env)))
                return f;
        if ((f = read_backquote(si, env)))
                return f;
        if ((f = read_comma(si, env)))
                return f;
        if ((f = read_cons(si, env)))
                return f;
        if ((f = read_string(si)))
                return f;
        if ((f = read_sharp(si, env)))
                return f;
        if ((f = read_number(si)))
                return f;
        read_errors(si, env);
        if ((f = read_symbol(si)))
                return f;
        return NULL;
}
