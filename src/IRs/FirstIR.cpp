#include "FirstIR.hpp"

void FirstIR::addNewCode(cStrRef operation, cStrRef one, cStrRef two)
{
    Line line;
    line.operation = operation;
    line.one = one;
    line.two = two;
    _currentBlock.top().lines.push_back(line);
    std::cerr<<line<<std::endl;
    if (operation == "HALT")
    {
        _blocks.push_back(_currentBlock.top());
    }
}

void FirstIR::handleMathOperation(cStrRef resultName)
{
    // std::cerr<<"Math="<<resultName<<" "<<_operation<<" "<<_firstExtraParameter<<" "<<_secondExtraParameter<<std::endl;
    if (_operation.empty())
    {
        addNewCode("CONST", resultName, _firstExtraParameter);
    }
    else if (_operation == "COPY")
    {
        addNewCode("COPY", resultName, _firstExtraParameter);      
    }
    else if (_secondExtraParameter == resultName)
    {
        if (_operation == "ADD" || _operation == "MUL")
        {
            addNewCode(_operation, resultName, _firstExtraParameter);
        }
        else if (_operation == "SUB" || _operation == "DIV" || _operation == "MOD")
        {
            handleNonCommutativeOperation(resultName, _operation);
        }
    }
    else
    {
        if (_firstExtraParameter == resultName)
        {
            addNewCode(_operation, resultName, _secondExtraParameter);
        }
        else
        {
            addNewCode("COPY", resultName, _firstExtraParameter);
            addNewCode(_operation, resultName, _secondExtraParameter);
        }  
    }
    reset();
}

void FirstIR::handleConditionOperation(cStrRef operation, cStrRef one, cStrRef two)
{
    addNewCode(operation, one, two);
    _blocks.push_back(_currentBlock.top());
    _currentBlock.pop();
    generateBlock();
    _blocks.back().blockIfFalse = _currentBlock.top().blockName;
    generateBlock();
    _blocks.back().blockIfTrue = _currentBlock.top().blockName;    

}

void FirstIR::setOperation(cStrRef operation)
{
    _operation = operation;
}
void FirstIR::setFirstExtraParameter(cStrRef first)
{
    _firstExtraParameter = first;
}
void FirstIR::setSecondExtraParameter(cStrRef second)
{
    _secondExtraParameter = second;
}

std::string FirstIR::getVariable(std::string value)
{
    std::cerr<<"generating variable with "<<value<<std::endl;
    std::string result = "variable_" + std::to_string(_registerCount);
    _registerCount++;
    _symbolTable->addConst(result, value);
    return result;
}

void FirstIR::endElse()
{
    Block s3 = _currentBlock.top();
    _currentBlock.pop();
    Block s2 = _currentBlock.top();
    _currentBlock.pop();
    generateBlock();
    Block s1 = _currentBlock.top();
    s3.blockJump = s1.blockName;
    _blocks.push_back(s3);
    _currentBlock.push(s2);
}

void FirstIR::endIf()
{
    _blocks.push_back(_currentBlock.top());
    _currentBlock.pop();
}

/*
PRIVATE
*/
void FirstIR::reset()
{
    _operation = "";
    _firstExtraParameter = "";
    _secondExtraParameter = "";
}

void FirstIR::handleNonCommutativeOperation(cStrRef resultName, cStrRef operation)
{
    std::cout<<"TEST="<<resultName<<std::endl;
    std::string reg = getVariable(resultName);
    addNewCode("COPY", reg, resultName);
    addNewCode("COPY", resultName, _firstExtraParameter);
    addNewCode(operation, resultName, reg);
}