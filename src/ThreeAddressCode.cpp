#include "ThreeAddressCode.hpp"

void ThreeAddressCode::addNewCode(cStrRef operation, cStrRef one, cStrRef two)
{
    Line line;
    line.operation = operation;
    line.one = one;
    line.two = two;
    _lines.push_back(line);
}

void ThreeAddressCode::handleMathOperation(cStrRef resultName)
{
    std::cerr<<resultName<<" "<<_operation<<" "<<_firstExtraParameter<<" "<<_secondExtraParameter<<std::endl;
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
        if (_operation == "ADD")
        {
            addNewCode(_operation, resultName, _firstExtraParameter);
        }
        else if (_operation == "SUB")
        {
            handleNonCommutativeOperation(resultName);
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

void ThreeAddressCode::print(cStrRef fileName)
{
    if (fileName.empty())
    {
        std::cerr<<"ThreeAddressCode:"<<std::endl;
        for(auto l : _lines)
        {
            std::cerr<<l.operation<<": "<<l.one<<" "<<l.two<<" "<<std::endl;
        }
    }
    else
    {
        std::ofstream output(fileName);
        output<<"ThreeAddressCode:"<<std::endl;
        for(auto l : _lines)
        {
            output<<l.operation<<": "<<l.one<<" "<<l.two<<" "<<std::endl;
        }
    }
}

std::string ThreeAddressCode::getRegister()
{
    std::string result = "register_" + std::to_string(_registerCount);
    _registerCount++;
    return result;
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

void ThreeAddressCode::handleNonCommutativeOperation(cStrRef resultName)
{
    std::string reg = getRegister();
    addNewCode("COPY", reg, resultName);
    addNewCode("COPY", resultName, _firstExtraParameter);
    addNewCode("SUB", resultName, reg);
}