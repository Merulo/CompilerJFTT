#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <stack>

#include "DataTypes/Line.hpp"
#include "IRBase.hpp"

typedef const std::string & cStrRef;

class ThreeAddressCode : public IRBase
{
    public:
    ThreeAddressCode() : IRBase("ThreeAddressCode") {};
    
    //adds new code
    void addNewCode(cStrRef operation, cStrRef one = "", cStrRef two = "");
    //performs math operation
    void handleMathOperation(cStrRef resultName);
    //handles condition operation
    void handleConditionOperation(cStrRef operation, cStrRef one, cStrRef two);
    std::string getVariable();
    
    //setters for nested operations
    void setOperation(cStrRef operation);
    void setFirstExtraParameter(cStrRef first);
    void setSecondExtraParameter(cStrRef second);

    void addJump();
    void swap();
    void endIf();

    private:

    std::string generateLabel();    
    void reset();
    void handleNonCommutativeOperation(cStrRef resultName, cStrRef operation);

    int _registerCount = 0;
    int _labelCount = 0;

    std::string _operation;
    std::string _firstExtraParameter;
    std::string _secondExtraParameter;

    std::stack<std::string> _labels;

    int _labelsToRemove = 0;

};