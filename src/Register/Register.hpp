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
    bool needToSafe = false;

    bool operator==(const Register& rhs)
    {
        return name == rhs.name;
    }
    bool operator!=(const Register& rhs)
    {
        return !(*this == rhs);
    }
    friend std::ostream& operator<<(std::ostream& os, const Register& r)
    {
        os <<"\t"<< r.name << " ";
        switch(r.state)
        {
            case RegisterState::UNKNOWN:
            {
                os << "unknown state";
                break;
            }
            case RegisterState::VARIABLE:
            {
                os << "variable="<<r.variableName;
                break;
            }
            case RegisterState::CONST:
            {
                os << "const="<<r.constValue<<" "<<r.variableName;
                break;
            }
            case RegisterState::TABLE:
            {
                os << "table="<<r.variableName;
                break;
            }
            case RegisterState::CONSTVARIABLE:
            {
                os <<"constvalue"<<r.variableName<<" with value "<<r.constValue;
            }
        }
        return os;
    }; 
};