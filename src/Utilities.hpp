#pragma once

#include <string>

#include "Driver.hpp"

struct Data 
{
    Data()
    {
        value = 0;
    }
    unsigned long long value;
    std::string name;
};

void checkForErrors(const std::string& str)
{
    if (!str.empty())
    {
        std::cout << "Error at line " << yylineno << ": " << str << std::endl;
        exit(1);
    }
}

std::string validateAndConvertData(Driver& d, const Data& data)
{
    if (!data.name.empty())
    {
        checkForErrors(d.ST.checkVariableExistsAndIsInitialized(data.name));
        return data.name;
    }
    else
    {
        std::string reg = d.TAC.getVariable();
        d.TAC.addNewCode("CONST", reg , std::to_string(data.value));
        return reg;
    }
}

void handleOperation(Driver& d, const std::string& operation, Data first, Data second)
{
    d.TAC.setOperation(operation);
    d.TAC.setFirstExtraParameter(validateAndConvertData(d, first));
    d.TAC.setSecondExtraParameter(validateAndConvertData(d, second)); 
}

void handleConditionOperation(Driver& d, const std::string& operation, Data first, Data second)
{
    std::string arg1 = validateAndConvertData(d, first);
    std::string arg2 = validateAndConvertData(d, second);
    d.TAC.handleConditionOperation(operation, arg1, arg2);
}