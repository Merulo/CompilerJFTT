#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

typedef const std::string & cStrRef;

struct Line
{
    std::string operation;
    std::string one;
    std::string two;
};

class ThreeAddressCode
{
    public:
    void addNewCode(cStrRef operation, cStrRef one = "", cStrRef two = "");
    void handleMathOperation(cStrRef resultName);
    void setOperation(cStrRef operation);
    void setFirstExtraParameter(cStrRef first);
    void setSecondExtraParameter(cStrRef second);
    std::string getRegister();

    void print(cStrRef fileName = "");
    private:
    void reset();
    void handleNonCommutativeOperation(cStrRef resultName, cStrRef operation);

    int _registerCount = 0;
    std::vector<Line> _lines;
    std::string _operation;
    std::string _firstExtraParameter;
    std::string _secondExtraParameter;

};