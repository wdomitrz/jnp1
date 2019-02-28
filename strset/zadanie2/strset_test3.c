#include <assert.h>
#include <string.h>
#include "strset.h"

#define SIZE 11

char* str_dup(char const *s) {
    static char t[SIZE + 1];
    return strncpy(t, s, SIZE);
}

int main() {
    {
        unsigned long s1;

        s1 = strset_new();

        strset_insert(s1, "bar");
        assert(strset_test(s1, "bar"));
        assert(! strset_test(s1, "foo"));
        strset_insert(s1, str_dup("bar"));
        strset_insert(s1, "foo");
        strset_insert(s1, "fooranka");

        assert(strset_size(s1) == 3);
        assert(strset_test(s1, "bar"));
        assert(strset_test(s1, "foo"));
        assert(strset_test(s1, "fooranka"));
        assert(!strset_test(s1, "gizmo"));

        strset_remove(s1, "bar");
        assert(strset_size(s1) == 2);
        assert(!strset_test(s1, "bar"));

        strset_remove(s1, "foo");
        assert(strset_size(s1) == 1);
        assert(strset_test(s1, "fooranka"));
        assert(!strset_test(s1, "foo"));

        strset_remove(s1, "baz");
        assert(strset_size(s1) == 1);
        assert(strset_test(s1, "fooranka"));
        assert(!strset_test(s1, "foo"));
        assert(!strset_test(s1, "baz"));

        strset_remove(s1, "fooranka");
        assert(strset_size(s1) == 0);
        assert(!strset_test(s1, "fooranka"));
        assert(!strset_test(s1, "fooranka1"));

        strset_remove(s1, "foo");
        assert(strset_size(s1) == 0);

        strset_insert(s1, "eloziom");
        strset_delete(s1);
        assert(strset_size(s1) == 0);
        assert(!strset_test(s1, "eloziom"));
    }

    {
        unsigned long s1;

        s1 = strset_new();

        strset_insert(s1, "a");
        strset_insert(s1, "b");
        strset_insert(s1, "c");

        assert(strset_size(s1) == 3);
        assert(strset_test(s1, "a"));
        assert(strset_test(s1, "b"));
        assert(strset_test(s1, "c"));

        strset_clear(s1);
        assert(strset_size(s1) == 0);
        assert(!strset_test(s1, "a"));
    }

    {
        unsigned long s1;

        s1 = strset_new();
        strset_delete(s1);

        strset_insert(s1, "a");
        strset_insert(s1, "b");
        strset_insert(s1, "c");
        assert(strset_size(s1) == 0);
        assert(!strset_test(s1, "a"));

        strset_remove(s1, "a");
        strset_remove(s1, "x");
        strset_remove(s1, "b");
        strset_clear(s1);

        assert(!strset_test(s1, "a"));
        assert(!strset_test(s1, "c"));
    }

    return 0;
}
