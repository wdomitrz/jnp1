#include <assert.h>
#include "strset.h"
#include "strsetconst.h"

int main() {
    {
        strset_delete(strset42());
        assert(strset_new() != strset42());
    }

    {
        assert(strset_size(strset42()) == 1);
        strset_clear(strset42());
        assert(strset_size(strset42()) == 1);
    }

    {
        strset_insert(strset42(), "33");
        assert(strset_size(strset42()) == 1);
        assert(strset_test(strset42(), "42"));
        assert(!strset_test(strset42(), "33"));

        strset_remove(strset42(), "42");
        assert(strset_size(strset42()) == 1);
        assert(strset_test(strset42(), "42"));
    }

    return 0;
}
