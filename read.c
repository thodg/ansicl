
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "env.h"
#include "form.h"
#include "read.h"
#include "package.h"

int refill (s_standard_input *si)
{
        while (!si->s || si->start == si->end) {
                if (si->s)
                        free(si->s);
                si->s = readline("\ncfacts> ");
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

u_form * read_error (s_standard_input *si, const char *msg)
{
        (void) si;
        fprintf(stderr, "cfacts: %s\n", msg);
        return nil();
}

u_form * read_cons (s_standard_input *si)
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
                                return head;
                        }
                        if (c == '.') {
                                if (!head)
                                        return read_error(si, "unexpect"
                                                          "ed dot");
                                else {
                                        read_char(si);
                                        if (!(*tail = read_form(si)) ||
                                            read_spaces(si) ||
                                            (c = read_char(si)) < 0)
                                                return NULL;
                                        if (c == ')')
                                                return head;
                                        return read_error(si, "malformed "
                                                          "dotted list");
                                }
                        }
                        if (!(*tail = read_form(si)))
                                return NULL;
                        *tail = new_cons(*tail, NULL);
                        tail = &(*tail)->cons.cdr;
                }
        }
        return NULL;
}

u_form * read_string (s_standard_input *si)
{
        u_form *f;
        unsigned long c;
        if (peek_char(si) == '"') {
                read_char(si);
                for (c = si->start; c < si->end && si->s[c] != '"'; c++)
                        ;
                if (si->s[c] == '"') {
                        f = new_string(c - si->start, si->s + si->start);
                        si->start = c + 1;
                        return f;
                }
                /* fix newline */
        }
        return NULL;
}



u_form * read_symbol (s_standard_input *si)
{
        u_form *f;
        unsigned long i = si->start;
        unsigned long j = i;
        while (i < si->end && si->s[i] != '(' && si->s[i] != ')'
               && si->s[i] != '.' && si->s[i] != '"' && si->s[i] != ' '
               && si->s[i] != '\t' && si->s[i] != '\r'
               && si->s[i] != '\n') {
                j = i;
                i++;
        }
        j = i;
        f = new_string(j - si->start, si->s + si->start);
        f = (u_form*) intern(&f->string, NULL);
        si->start = j;
        return f;
}

u_form * read_form (s_standard_input *si)
{
        u_form *f;
        if (read_spaces(si))
                return NULL;
        if ((f = read_cons(si)))
                return f;
        if ((f = read_string(si)))
                return f;
        if ((f = read_symbol(si)))
                return f;
        return NULL;
}