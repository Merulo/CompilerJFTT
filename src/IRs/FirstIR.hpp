#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <stack>
#include <map>
#include <stack>
#include <algorithm>

#include "DataTypes/Line.hpp"
#include "DataTypes/Data.hpp"
#include "IRBase.hpp"

typedef const std::string & cStrRef;

class FirstIR : public IRBase
{
    public:
    FirstIR() : IRBase("FirstIR") {};
    
    //adds new code
    void addNewCode(cStrRef operation, cStrRef one = "", cStrRef two = "");
    //performs math operation
    void handleMathOperation(cStrRef resultName);
    //handles condition operation
    void handleConditionOperation(cStrRef operation, cStrRef one, cStrRef two);
    
    //setters for nested operations
    void setOperation(cStrRef operation);
    void setFirstExtraParameter(cStrRef first);
    void setSecondExtraParameter(cStrRef second);

    //for conditions
    void endElse();
    void endIf();
    void closeConditionBlock();

    //for loops
    void closeForBlock();
    void endWhileDo();
    void endDoWhile();
    void insertFor(std::string iterator, Data from, Data to, bool isForTo);
    Block createBeforeForBlock(std::string iterator, Data from, Data to, bool isForTo, std::string name);
    Block createSecondControlBlock(std::string iterator, bool isForTo, std::string name);
    Line getLine(Data d);

    private:
    void reset();
    void handleNonCommutativeOperation(cStrRef resultName, cStrRef operation);

    std::string _operation;
    std::string _firstExtraParameter;
    std::string _secondExtraParameter;
    std::stack<Block> _conditionBlocks;
    std::stack<Block> _forBlocks;
 
};