
#include <stdlib.h>
#include <string.h>
#include "form.h"
#include "package.h"

s_skiplist *g_packages;

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

s_package * cfacts_package ()
{
        static s_package *cfacts;
        if (!cfacts) {
                const char *name = "cfacts";
                s_string *str = (s_string*) new_string(strlen(name), name);
                s_symbol *sym = (s_symbol*) new_symbol(str);
                cfacts = (s_package*) new_package(sym);
                sym = intern(str, cfacts);
                cfacts->name = sym;
        }
        return cfacts;
}

s_symbol * find_symbol (s_string *s, s_package *pkg)
{
        s_symbol sym;
        if (!pkg)
                pkg = cfacts_package();
        sym.type = FORM_SYMBOL;
        sym.package = pkg;
        sym.string = s;
        s_skiplist_node *n = skiplist_find(pkg->symbols, &sym);
        if (n)
                return n->value;
        return NULL;
}

s_symbol * find_symbol_ (const char *s, s_package *pkg)
{
        unsigned long len = strlen(s);
        s_string *str = alloca(sizeof(s_string) + len + 1);
        init_string(str, len, s);
        return find_symbol(str, pkg);
}

s_symbol * intern (s_string *s, s_package *pkg)
{
        s_symbol *sym;
        if (!pkg)
                pkg = cfacts_package();
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

s_symbol * intern_ (const char *s, s_package *pkg)
{
        s_symbol *sym = find_symbol_(s, pkg);
        s_string *str;
        if (sym)
                return sym;
        str = new_string(strlen(s), s);
        return intern(str, pkg);
}

s_symbol * sym (const char *s)
{
        return intern_(s, NULL);
}

void unintern (s_string *s, s_package *pkg)
{
        s_symbol sym;
        if (!pkg)
                pkg = cfacts_package();
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

int compare_packages (void *a, void *b)
{
        s_package *pa = (s_package*) a;
        s_package *pb = (s_package*) b;
        if (!a && !b)
                return 0;
        if (!a)
                return -1;
        if (!b)
                return 1;
        return compare_symbols(pa->name, pb->name);
}

s_package * find_package (s_string *name);

void init_packages (void)
{
        g_packages = new_skiplist(10, M_E);
        g_packages->compare = compare_packages;
        skiplist_insert(g_packages, cfacts_package());
}
