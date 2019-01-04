#pragma once

#include <string>
#include <vector>
#include <iostream>

struct Line
{
    Line(){};
    Line(std::string operationInit, std::string oneInit= "", std::string twoInit = "")
    {
        operation = operationInit;
        one = oneInit;
        two = twoInit;
    }
    std::string operation;
    std::string one;
    std::string two;
    unsigned int operationNumber;
    friend std::ostream& operator<<(std::ostream& os, const Line& l)
    {
        os << l.operation<< " " << l.one << " " << l.two;
        return os;
    };    
    std::string toString()
    {
        return operation + " " + one + " " + two;
    }
};