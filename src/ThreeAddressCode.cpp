#include "ThreeAddressCode.hpp"

void ThreeAddressCode::addNewCode(cStrRef name, cStrRef one, cStrRef two)
{
    Line line;
    line.name = name;
    line.one = one;
    line.two = two;
    _lines.push_back(line);
}

void ThreeAddressCode::addAssignCode(cStrRef name)
{
    // std::cout<<name<<" "<<_operation<<" "<<_firstExtraParameter<<" "<<_secondExtraParameter<<std::endl;
    if (_operation.empty())
    {
        Line line;
        line.name = "CONST";
        line.one = name;
        line.two = _firstExtraParameter;
        _lines.push_back(line);
    }
    else if (_firstExtraParameter == name)
    {
        handleAssign(_operation, name, _secondExtraParameter);
    }
    else if (_secondExtraParameter == name)
    {
        handleAssign(_operation, name, _firstExtraParameter);
    }
    else 
    {
        Line line;
        line.name = "COPY";
        line.one = name;
        line.two = _firstExtraParameter;
        _lines.push_back(line);
        handleAssign(_operation, name, _secondExtraParameter);
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
            std::cerr<<l.name<<": "<<l.one<<" "<<l.two<<" "<<std::endl;
        }
    }
    else
    {
        std::ofstream output(fileName);
        output<<"ThreeAddressCode:"<<std::endl;
        for(auto l : _lines)
        {
            output<<l.name<<": "<<l.one<<" "<<l.two<<" "<<std::endl;
        }
    }

}

void ThreeAddressCode::handleAssign(cStrRef name, cStrRef first, cStrRef second)
{
    Line line;
    line.name = name;
    line.one = first;
    line.two = second;
    _lines.push_back(line);
}

void ThreeAddressCode::reset()
{
    _operation = "";
    _firstExtraParameter = "";
    _secondExtraParameter = "";
}