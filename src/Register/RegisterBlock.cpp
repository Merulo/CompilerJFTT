#include "RegisterBlock.hpp"

RegisterBlock::RegisterBlock(std::shared_ptr<SymbolTable> symbolTable)
{
    _symbolTable = symbolTable;
    _currentRegister = 0;
}
void RegisterBlock::createRegisters()
{
    for(size_t i = 1; i < 8; i++){
        Register r;
        char c =  'A' + i;
        r.name = c;
        _registers.push_back(r);
    }
}

std::vector<Line> RegisterBlock::performTableMemoryOperation(std::string operation, std::string varName, Register& r)
{
    std::string array = varName.substr(0, varName.find("("));

    unsigned long long memoryCell = _symbolTable->getMemoryCell(array);
    std::cout<<"memory cell is "<<memoryCell<<std::endl;
    std::string rest = varName.substr(varName.find("(") + 1, std::string::npos);
    rest.pop_back();
    std::cout<<"Searching for= "<<rest<<std::endl;
    unsigned long long shift = _symbolTable->getTableShift(array);
    std::cout<<"Shift= "<<shift<<std::endl;

    if (isDigits(rest))
    {
        memoryCell = std::stoull(rest) - shift;
        std::cout<<"real memory cell="<<memoryCell<<std::endl;
        return performMemoryOperation(operation, r, memoryCell);
    }
    else
    {
        std::cout<<"not yet implemented"<<std::endl;
    }
    return {};
}

std::vector<Line> RegisterBlock::performMemoryOperation(std::string operation, Register& r, unsigned long long memoryCell)
{
    std::vector<Line> lines;
    if (_addressRegister.state == RegisterState::UNKNOWN)
    {
        lines.push_back({"SUB", "A", "A"});
        _addressRegister.constValue = 0;
        _addressRegister.state = RegisterState::CONST;
    }

    while(_addressRegister.constValue < memoryCell)
    {
        lines.push_back({"INC", "A", ""});
        _addressRegister.constValue++;
    }
    while(_addressRegister.constValue > memoryCell)
    {
        lines.push_back({"DEC", "A", ""});
        _addressRegister.constValue--;
    }
    lines.push_back({operation , r.name});
    return lines;
}

Register& RegisterBlock::getRegisterForVariable(std::string name)
{
    for(auto& r : _registers)
    {
        if (r.state == RegisterState::CONSTVARIABLE || r.state == RegisterState::VARIABLE || r.state == RegisterState::TABLE)
        {
            if (r.variableName == name)
            {
                return r;
            }
        }
    }

    for(auto&r : _registers)
    {
        if (r.state == RegisterState::UNKNOWN)
        {
            return r;
        }
    }

    for(auto& r : _registers)
    {
        if (r.state == RegisterState::CONST)
        {
            return r;
        }
    }
    Register& r = _registers[_currentRegister];
    _currentRegister++;
    if (_currentRegister == _registers.size())
    {
        _currentRegister = 0;
    }
    return r;  
}

void RegisterBlock::print()
{
    std::cerr<<_addressRegister<<std::endl;
    for(auto& r : _registers)
    {
        std::cerr<<r<<std::endl;
    }
}
