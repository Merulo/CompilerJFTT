#pragma once

#include <string>
#include <iostream>
#include "DataTypes/Line.hpp"

enum class RegisterState
{
    UNKNOWN = 0,
    VARIABLE = 1,
    CONST = 2,
    TABLE = 3,
    CONSTVARIABLE = 4,
};

struct Register
{
    Register()
    {
        constValue = rand();    //simulate garbage
        state = RegisterState::UNKNOWN;
    }
    RegisterState state;
    unsigned long long constValue;
    std::string name;
    std::string variableName;

    friend std::ostream& operator<<(std::ostream& os, const Register& r)
    {
        os << r.name << " ";
        switch(r.state)
        {
            case RegisterState::UNKNOWN:
            {
                os << "unknown state";
            }
            case RegisterState::VARIABLE:
            {
                os << "variable="<<r.variableName;
            }
            case RegisterState::CONST:
            {
                os << "const="<<r.constValue;
            }
            case RegisterState::TABLE:
            {
                os << "table="<<r.variableName;
            }
            case RegisterState::CONSTVARIABLE:
            {
                os << "variable="<<r.variableName<<" with value="<<r.constValue;
            }
        }
        return os;
    }; 
};