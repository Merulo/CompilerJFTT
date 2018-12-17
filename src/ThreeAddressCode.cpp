#include "ThreeAddressCode.hpp"

void ThreeAddressCode::addNewCode(cStrRef operation, cStrRef one, cStrRef two)
{
    while (tester > 0)
    {
        // std::cerr<<"Adding label="<<_labels.top()<<std::endl; 
        Line line;
        line.thisLabel = _labels.top();
        _labels.pop();
        _lines.push_back(line);
        tester--;
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
    std::streambuf * buf;
    if (fileName.empty())
    {
        buf = std::cerr.rdbuf();
        std::ostream out(buf);
        writeToStream(out);
    }
    else
    {
        std::ofstream output(fileName);
        buf = output.rdbuf();
        std::ostream out(buf);
        writeToStream(out);
    }

}

std::string ThreeAddressCode::getVariable()
{
    std::string result = "variable_" + std::to_string(_registerCount);
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

void ThreeAddressCode::handleNonCommutativeOperation(cStrRef resultName, cStrRef operation)
{
    std::string reg = getVariable();
    addNewCode("COPY", reg, resultName);
    addNewCode("COPY", resultName, _firstExtraParameter);
    addNewCode(operation, resultName, reg);
}