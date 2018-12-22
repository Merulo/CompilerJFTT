#include "ThreeAddressCode.hpp"

void ThreeAddressCode::addNewCode(cStrRef operation, cStrRef one, cStrRef two)
{
    while (_labelsToRemove > 0)
    {
        // std::cerr<<"Adding label="<<_labels.top()<<std::endl; 
        Line line;
        line.thisLabel = _labels.top();
        _labels.pop();
        _lines.push_back(line);
        _labelsToRemove--;
    }
    Line line;
    line.operation = operation;
    line.one = one;
    line.two = two;
    _lines.push_back(line);
}

void ThreeAddressCode::handleMathOperation(cStrRef resultName)
{
    // std::cerr<<"Math="<<resultName<<" "<<_operation<<" "<<_firstExtraParameter<<" "<<_secondExtraParameter<<std::endl;
    if (_operation.empty())
    {
        addNewCode("CONST", resultName, _firstExtraParameter);
    }
    else if (_operation == "COPY")
    {
        if (resultName != _firstExtraParameter)
        {
            addNewCode("COPY", resultName, _firstExtraParameter);      
        }
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

std::map<std::string, std::string> ThreeAddressCode::getConsts()
{
    return _consts;
}

void ThreeAddressCode::handleConditionOperation(cStrRef operation, cStrRef one, cStrRef two)
{
    addNewCode(operation, one, two);
    std::string label = generateLabel();
    _lines.back().targetLabel = label;
    std::cout<<"Generated label="<<label<<std::endl;
    _labels.push(label);
}

void ThreeAddressCode::setOperation(cStrRef operation)
{
    _operation = operation;
}
void ThreeAddressCode::setFirstExtraParameter(cStrRef first)
{
    _firstExtraParameter = first;
}
void ThreeAddressCode::setSecondExtraParameter(cStrRef second)
{
    _secondExtraParameter = second;
}

std::string ThreeAddressCode::getVariable(std::string value)
{
    std::cerr<<"generating variable with "<<value<<std::endl;
    std::string result = "variable_" + std::to_string(_registerCount);
    _registerCount++;
    _consts[result] = value;
    return result;
}

void ThreeAddressCode::addJump()
{
    addNewCode("JUMP");
    std::string label = generateLabel();
    _lines.back().targetLabel = label;
    std::cout<<"Generated label="<<label<<std::endl;
    _labels.push(label);        
}

void ThreeAddressCode::swap()
{
    std::string s1 = _labels.top();
    _labels.pop();
    std::string s2 = _labels.top();
    _labels.pop();
    _labels.push(s1);
    _labels.push(s2);
}

void ThreeAddressCode::endIf()
{
    // std::cerr<<"ENDIF="<<tester<<std::endl;
    _labelsToRemove++;
}

/*
PRIVATE
*/
void ThreeAddressCode::reset()
{
    _operation = "";
    _firstExtraParameter = "";
    _secondExtraParameter = "";
}

void ThreeAddressCode::handleNonCommutativeOperation(cStrRef resultName, cStrRef operation)
{
    std::cout<<"TEST="<<resultName<<std::endl;
    std::string reg = getVariable(resultName);
    addNewCode("COPY", reg, resultName);
    addNewCode("COPY", resultName, _firstExtraParameter);
    addNewCode(operation, resultName, reg);
}

std::string ThreeAddressCode::generateLabel()
{
    std::string result = "L_" + std::to_string(_labelCount);
    _labelCount++;
    return result; 
}