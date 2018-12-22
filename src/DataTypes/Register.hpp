#pragma once

#include <string>
#include <iostream>

struct Register
{
    unsigned long long value = 0;
    std::string variableName;
    std::string registerName;
    inline bool operator==(const Register& rhs){ return registerName == rhs.registerName; }
    friend std::ostream& operator<<(std::ostream& os, const Register& r)
    {
        os << r.registerName<< " " << r.value << " " <<r.variableName;
        return os;
    };
};


struct AddressRegister
{
    unsigned long long value = 0;
    std::string registerName;
};