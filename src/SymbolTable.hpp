#pragma once

#include <set>
#include <string>


struct Variable {
    std::string name;
};
struct Table {
    std::string name;
    long long begin;
    long long end;
};
struct Iterator {
    std::string name;
    long long begin;
    long long end;
    bool goes_up;
};


namespace symbol {
    template <class T>
    struct Comparator {
        bool operator() (const T& lhs, const T& rhs) const {
            return lhs.name < rhs.name;
        }
    };

    std::set<Variable, Comparator<Variable>> variable;
    std::set<Table, Comparator<Table>> table;
    std::set<Iterator, Comparator<Iterator>> iterator;
}
