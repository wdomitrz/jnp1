#include <assert.h>
#include "strset.h"
#include "strsetconst.h"

int main() {
    {
        unsigned long d1, d2;

        d1 = strset_new();
        d2 = strset_new();
        strset_insert(d1, NULL);
        assert(!strset_test(d1, NULL));
        assert(strset_size(d1) == 0);
        assert(strset_comp(d1, d2) == 0);
    }

    {
        unsigned long d1, d2;

        d1 = strset_new();
        d2 = strset_new();
        strset_insert(d1, "foo");
        strset_insert(d2, "foo");

        strset_insert(d1, NULL);
        assert(strset_test(d1, "foo"));
        assert(strset_size(d1) == 1);
        assert(strset_comp(d1, d2) == 0);

        strset_insert(d1, NULL);
        assert(!strset_test(d1, NULL));
        assert(strset_size(d1) == 1);
        assert(strset_comp(d1, d2) == 0);
    }

    {
        strset_insert(strset42(), NULL);
        assert(strset_test(strset42(), "42"));
        assert(!strset_test(strset42(), NULL));
        assert(strset_size(strset42()) == 1);
    }

    return 0;
}
