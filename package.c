
#include <stdlib.h>
#include <string.h>
#include "form.h"
#include "package.h"

s_package *g_package = NULL;

int compare_symbols (void *a, void *b)
{
        s_symbol *sa = (s_symbol*) a;
        s_symbol *sb = (s_symbol*) b;
        int c;
        if (sa == sb)
                return 0;
        if (!sa)
                return -1;
        if (!sb)
                return 1;
        c = skiplist_compare_ptr(sa->package, sb->package);
        if (c != 0)
                return c;
        return strcmp(sa->string->str, sb->string->str);
}

s_symbol * find_symbol (s_string *s, s_package *pkg)
{
        s_symbol sym;
        if (!pkg)
                pkg = g_package;
        sym.type = FORM_SYMBOL;
        sym.package = pkg;
        sym.string = s;
        s_skiplist_node *n = skiplist_find(pkg->symbols, &sym);
        if (n)
                return n->value;
        return NULL;
}

s_symbol * intern (s_string *s, s_package *pkg)
{
        s_symbol *sym;
        if (!pkg)
                pkg = g_package;
        if (pkg) {
                sym = find_symbol(s, pkg);
                if (sym)
                        return sym;
        }
        sym = (s_symbol*) new_symbol(s);
        sym->package = pkg;
        if (pkg)
                skiplist_insert(pkg->symbols, sym);
        return sym;
}

void unintern (s_string *s, s_package *pkg)
{
        s_symbol sym;
        if (!pkg)
                pkg = g_package;
        sym.type = FORM_SYMBOL;
        sym.package = pkg;
        sym.string = s;
        skiplist_delete(pkg->symbols, &sym);
}

void delete_package (s_package *pkg)
{
        s_skiplist_node *n = skiplist_node_next(pkg->symbols->head, 0);
        while (n) {
                free(n->value);
                n = skiplist_node_next(n, 0);
        }
        delete_skiplist(pkg->symbols);
        free(pkg);
}

void init_packages (void)
{
        const char *s = "cfacts";
        s_string *str = (s_string*) new_string(strlen(s), s);
        s_symbol *sym = intern(str, NULL);
        g_package = (s_package*) new_package(sym);
        sym = intern(str, g_package);
        g_package->name = sym;
}
