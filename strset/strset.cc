/*
 * Authors:
 *  Damian Obara
 *  Piotr Ogonowski
 *  Witalis Domitrz
 */

#include <set>
#include <unordered_map>
#include <string>
#include <list>
#include <iostream>
#include <cassert>
#include <climits>
#include "strset.h"
#include "strsetconst.h"


namespace {


#ifdef NDEBUG
    const bool debug = false;
#else
    const bool debug = true;
#endif //NDEBUG


    using Id = unsigned long;

    using SetSize = size_t;

    using CStr = const char *;

    using Str = std::string;

    using SetOfStrs = std::set<Str>;

    using MapOfSets = std::unordered_map<Id, SetOfStrs>;


    using SetOfIds = std::set<Id>;


    SetOfIds &idsToReuse() {
        static SetOfIds *idsToReuseSet = new SetOfIds();
        return *idsToReuseSet;
    }


    MapOfSets &mapOfSets() {
        static MapOfSets *mapOfSets = new MapOfSets();
        return *mapOfSets;
    }


    Id getNewID() {
        static Id newID = 0;

        assert(newID != ULONG_MAX);

        return newID++;
    }

    int setCompare(SetOfStrs &set1, SetOfStrs &set2) {
        if (set1 < set2)
            return -1;
        if (set1 == set2)
            return 0;
        return 1;
    }

    SetOfStrs &emptySet() {
        static SetOfStrs *newSet = new SetOfStrs();
        return *newSet;
    }

    namespace dbg {

        using DebudArg = std::string;
        using DebudMsg = std::string;
        using DebudArgs = std::list<dbg::DebudArg>;
        using DebugFuncName = const char *;

        template<class T>
        inline DebudMsg toDebudMsg(T msgElem) {
            return std::to_string(msgElem);
        }

        void writeListWithColon(std::ostream &outStream, DebudArgs elements) {

            bool isFirstElement = true;

            for (auto const &element : elements) {

                if (!isFirstElement)
                    outStream << ", ";
                else
                    isFirstElement = false;

                outStream << element;
            }
        }

        inline void debug_msg(DebugFuncName func, DebudArgs args, DebudMsg action) {

            if (debug) {

                std::cerr << func << "(";

                writeListWithColon(std::cerr, args);

                std::cerr << ");\t" << action << "\n";
            }
        }

    }

}


namespace jnp1 {

    Id strset_new() {

        dbg::debug_msg(__func__, dbg::DebudArgs({}), "start");

        Id newId;

        if (!idsToReuse().empty()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({}), "reusing Id");

            auto newIdIt = idsToReuse().begin();

            newId = *newIdIt;

            idsToReuse().erase(newIdIt);

        } else {

            dbg::debug_msg(__func__, dbg::DebudArgs({}), "creating new Id");

            newId = getNewID();

        }

        mapOfSets()[newId] = SetOfStrs();

        dbg::debug_msg(__func__, dbg::DebudArgs({}), "success, the new Id: " + dbg::toDebudMsg(newId) + ", end");

        return newId;
    }

    void strset_delete(Id id) {

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}), "start");

        if (id == strset42()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                           "set " + dbg::toDebudMsg(id) + " is const set (42), end");

            return;
        }

        auto idAndStringSet = mapOfSets().find(id);

        if (idAndStringSet == mapOfSets().end()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                           "set " + dbg::toDebudMsg(id) + " does not exist, end");

            return;
        }

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                       "set " + dbg::toDebudMsg(id) + " exists, removing");

        mapOfSets().erase(idAndStringSet);

        idsToReuse().insert(id);

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}), "end");
    }

    SetSize strset_size(Id id) {

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}), "start");

        auto idAndStringSet = mapOfSets().find(id);

        if (idAndStringSet == mapOfSets().end()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                           "set " + dbg::toDebudMsg(id) + " does not exist, end");

            return 0;
        }

        auto &strSet = idAndStringSet->second;

        SetSize size = strSet.size();

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                       "set " + dbg::toDebudMsg(id) + " exists, size: " + dbg::toDebudMsg(size) + ", end");

        return size;
    }

    void strset_insert(Id id, CStr value) {

        if (value == nullptr) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), "NULL"}), "start");

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), "NULL"}), "invalid value - NULL, end");

            return;
        }

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}), "start");

        if (id == strset42()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                           "set " + dbg::toDebudMsg(id) + " is const set (42), end");

            return;
        }

        auto idAndStringSet = mapOfSets().find(id);

        if (idAndStringSet == mapOfSets().end()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                           "invalid value - set " + dbg::toDebudMsg(id) + " does not exist, end");

            return;
        }

        auto &strSet = idAndStringSet->second;

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                       "attempt to insert value: " + dbg::DebudMsg(value) + " into set: " + dbg::toDebudMsg(id));

        auto inserted = strSet.insert(Str(value)).second;

        if (!inserted) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                           "value: " + dbg::DebudMsg(value) + " existed earlier in set: " + dbg::toDebudMsg(id) +
                           ", end");

            return;
        }

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}), "inserted, end");
    }

    void strset_remove(Id id, CStr value) {

        if (value == nullptr) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), "NULL"}), "start");

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), "NULL"}), "invalid value - NULL, end");

            return;
        }

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}), "start");

        if (id == strset42()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                           "set " + dbg::toDebudMsg(id) + " is const set (42), end");

            return;
        }

        auto idAndStringSet = mapOfSets().find(id);

        if (idAndStringSet == mapOfSets().end()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                           "invalid value - set " + dbg::toDebudMsg(id) + " does not exist, end");

            return;
        }

        auto &strSet = idAndStringSet->second;

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                       "attempt to remove value: " + dbg::DebudMsg(value) + " from set: " + dbg::toDebudMsg(id));

        auto removedCount = strSet.erase(Str(value));

        if (removedCount == 0) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                           "value: " + dbg::DebudMsg(value) + " did not exist in set: " + dbg::toDebudMsg(id) +
                           ", end");

            return;
        }

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}), "removed, end");
    }

    int strset_test(Id id, CStr value) {

        if (value == nullptr) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), "NULL"}), "start");

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), "NULL"}), "invalid value - NULL, end");

            return 0;
        }

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}), "start");

        auto idAndStringSet = mapOfSets().find(id);

        if (idAndStringSet == mapOfSets().end()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                           "invalid value - set " + dbg::toDebudMsg(id) + " does not exist, end");

            return 0;
        }

        auto &strSet = idAndStringSet->second;

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                       "attempt to find value: " + dbg::DebudMsg(value) + " in set: " + dbg::toDebudMsg(id));

        bool elementFound = !(strSet.find(Str(value)) == strSet.end());

        if (!elementFound) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                           "value: " + dbg::DebudMsg(value) + " not found in set: " + dbg::toDebudMsg(id) + ", end");
            return 0;
        }

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id), dbg::DebudArg(value)}),
                       "value: " + dbg::DebudMsg(value) + " found in set: " + dbg::toDebudMsg(id) + ", end");
        return 1;
    }

    void strset_clear(Id id) {

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}), "start");

        if (id == strset42()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                           "set " + dbg::toDebudMsg(id) + " is const set (42), end");

            return;
        }

        auto idAndStringSet = mapOfSets().find(id);

        if (idAndStringSet == mapOfSets().end()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                           "invalid value - set " + dbg::toDebudMsg(id) + " does not exist, end");

            return;
        }

        auto &strSet = idAndStringSet->second;

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                       "clearing set " + dbg::toDebudMsg(id));

        strSet.clear();

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id)}),
                       "clearing succeded, end");
    }

    int strset_comp(Id id1, Id id2) {

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id1), dbg::toDebudMsg(id2)}), "start");

        auto idAndStringSet1 = mapOfSets().find(id1),
                idAndStringSet2 = mapOfSets().find(id2);

        bool isSet1Empty = false,
                isSet2Empty = false;

        if (idAndStringSet1 == mapOfSets().end()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id1), dbg::toDebudMsg(id2)}),
                           "set " + dbg::toDebudMsg(id1) + " does not exist, treating as empty");

            isSet1Empty = true;
        }

        if (idAndStringSet2 == mapOfSets().end()) {

            dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id1), dbg::toDebudMsg(id2)}),
                           "set " + dbg::toDebudMsg(id2) + " does not exist, treating as empty");

            isSet2Empty = true;
        }

        SetOfStrs &strSet1 = isSet1Empty ? emptySet() : idAndStringSet1->second;
        SetOfStrs &strSet2 = isSet2Empty ? emptySet() : idAndStringSet2->second;

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id1), dbg::toDebudMsg(id2)}),
                       "calculating result");

        int cmpResult = setCompare(strSet1, strSet2);

        dbg::debug_msg(__func__, dbg::DebudArgs({dbg::toDebudMsg(id1), dbg::toDebudMsg(id2)}),
                       "result: " + dbg::toDebudMsg(cmpResult) + ", end");

        return cmpResult;
    }

}
