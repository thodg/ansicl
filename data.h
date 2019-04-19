#ifndef FORM_H
#define FORM_H

typedef struct cons s_cons;
typedef struct string s_string;
typedef union form u_form;

struct cons {
        unsigned long type;
        u_form *car;
        u_form *cdr;
};

struct string {
        unsigned long type;
        unsigned long length;
        char str[];
};

union form {
        s_cons cons;
        s_string string;
};

enum e_form_type { FORM_NULL,
                   FORM_CONS,
                   FORM_STRING };

#endif
