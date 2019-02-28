#include <assert.h>
#include "strset.h"
#include "strsetconst.h"

int main() {
    {
        unsigned long d1, d2;

        d1 = strset_new();
        d2 = strset_new();

        strset_insert(d1, "a");
        strset_insert(d2, "b");

        strset_delete(d1);
        strset_delete(d2);

        assert(strset_comp(d1, d2) == 0);
        assert(strset_comp(d1, strset42()) == -1);
        assert(strset_comp(strset42(), d2) == 1);
    }

    {
        unsigned long d1, d2;

        d1 = strset_new();
        d2 = strset_new();

        strset_insert(d1, "a");
        strset_insert(d1, "b");

        strset_insert(d2, "a");
        strset_insert(d2, "b");
        strset_insert(d2, "c");

        assert(strset_comp(d1, d2) == -1);
        assert(strset_comp(d2, d1) == 1);
    }

    {
        unsigned long d1, d2;

        d1 = strset_new();
        assert(strset_comp(d1, strset42()) == -1);
        assert(strset_comp(strset42(), d1) == 1);

        strset_insert(d1, "42");
        assert(strset_comp(strset42(), d1) == 0);
        assert(strset_comp(d1, strset42()) == 0);

        d2 = strset_new();

        strset_insert(d2, "42");
        assert(strset_comp(d2, d1) == 0);
        assert(strset_comp(d1, d2) == 0);

        strset_insert(d1, "30");
        strset_insert(d2, "5");

        assert(strset_comp(d1, d2) == -1);
        assert(strset_comp(d2, d1) == 1);
    }

    return 0;
}
