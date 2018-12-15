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
    if (_firstExtraParameter == name)
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

void ThreeAddressCode::loadLocalParameters(cStrRef name, cStrRef first, cStrRef second)
{
    _operation = name;
    _firstExtraParameter = first;
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