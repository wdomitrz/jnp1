/*
 * Authors:
 *  Damian Obara
 *  Piotr Ogonowski
 *  Witalis Domitrz
 */

#include <iostream>
#include "strset.h"
#include "strsetconst.h"

namespace {

#ifdef NDEBUG
    const bool debug = false;
#else
    const bool debug = true;
#endif //NDEBUG

    using StateOfConstSet = enum enumStateOfConstSet {
        notCreated, inCreation, created
    };

    StateOfConstSet &stateOfConstSet() {
        static StateOfConstSet currentState = notCreated;
        return currentState;
    }

    namespace dbg {

        using DebudMsg = std::string;

        using DebugFuncName = const char *;

        inline void debug_msg(DebugFuncName func, DebudMsg action) {

            if (debug) {

                std::cerr << func << "(";

                std::cerr << ");\t" << action << "\n";
            }
        }
    }
}

namespace jnp1 {

    unsigned long strset42() {

        dbg::debug_msg(__func__, "start");

        if (stateOfConstSet() == notCreated) {

            dbg::debug_msg(__func__, "const set does not exist, creating");
        }

        static unsigned long set42Id = strset_new();

        if (stateOfConstSet() == created) {

            dbg::debug_msg(__func__, "const set exists with id: " + std::to_string(set42Id) + ", end");

            return set42Id;
        } else if (stateOfConstSet() == inCreation) {

            dbg::debug_msg(__func__, "const set is in creation process just now, end");

            return set42Id + 1;
        }

        stateOfConstSet() = inCreation;

        strset_insert(set42Id, "42");

        stateOfConstSet() = created;

        dbg::debug_msg(__func__, "const set created with id: " + std::to_string(set42Id) + ", end");

        return set42Id;
    }

}