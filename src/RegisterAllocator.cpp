#include "RegisterAllocator.hpp"

RegisterAllocator::RegisterAllocator(SymbolTable& symbolTable)
    : _symbolTable(symbolTable)
{
    //should be i < 8
    for(int i = 1; i < 2; i++)
    {
        Register r;
        std::string str;
        str.push_back(char (int ('A') + i));
        r.registerName = str;
        _registers.push_back(r);
    }
}

RegisterAllocator::~RegisterAllocator()
{
    for(auto& s : _lines)
    {
        _outputFile<<s<<std::endl;
    }
}

void RegisterAllocator::print()
{
    std::cerr<<"**************************"<<std::endl;
    std::cerr<<"Registers:"<<std::endl;
    std::cerr<<_addressRegister.registerName<<" "<<_addressRegister.value<<std::endl;
    for(auto& r : _registers)
    {
        std::cerr<<r.registerName<<" "<<r.value<<" "<<r.variableName<<std::endl;
    }
}

void RegisterAllocator::compile(const std::string& fileName, std::vector<Line> finalIR)
{
    _outputFile.open(fileName);
    std::cerr<<"compiling!"<<std::endl;

    for (auto& l : finalIR)
    {
        if (l.operation == "READ")
        {
            handleRead(l.one);
        }
        if (l.operation == "WRITE")
        {
            handleWrite(l.one);
        }
        if (l.operation == "HALT")
        {
            _lines.push_back("HALT");
        }     
    }
}

/*
PRIVATE
*/

void RegisterAllocator::handleRead(std::string& variableName)
{
    
    Register& r = getRegisterForVariable(variableName).first;
    _lines.push_back("GET " + r.registerName);
}

void RegisterAllocator::handleWrite(std::string& variableName)
{
    auto pair = getRegisterForVariable(variableName);
    if(!pair.second)
    {
        prepareAddressRegister(pair.first);
        _lines.push_back("LOAD " + pair.first.registerName);

    }
    _lines.push_back("PUT " + pair.first.registerName);
} 

//todo upgrade this so that less calls to saving [Curent state: ROUND ROBIN]
std::pair<Register&, bool> RegisterAllocator::getRegisterForVariable(std::string name)
{
    std::cerr<<"Looking for "<<name<<std::endl;
    for(auto& r : _registers)
    {
        std::cerr<<"\tTesting "<<r.variableName<<std::endl;
        if(r.variableName == name)
        {
            r.variableName = name;
            return {r, true};
        }
    }

    Register r = _registers.front();
    _registers.pop_front();
    _registers.push_back(r);
    if (!r.variableName.empty())
    {
        prepareAddressRegister(r);
        _lines.push_back("STORE "+r.registerName);
    }
    _registers.back().variableName = name;
    return {_registers.back(), false};
}

void RegisterAllocator::prepareAddressRegister(Register& r)
{
    ull memoryCell = _symbolTable.getMemoryCell(r.variableName);
    if (_addressRegister.registerName.empty())
    {
        _addressRegister.registerName = "A";
        _lines.push_back("SUB A A");
    }
    //todo check if any register has a close const number to this
    //todo check how to generate this constant fastest
    std::cerr<<"seting address at "<<memoryCell<<" for "<<r.variableName<<std::endl;
    while(_addressRegister.value < memoryCell)
    {
        _lines.push_back("INC A");
        _addressRegister.value++;
    }
    while(_addressRegister.value > memoryCell)
    {
        _lines.push_back("DEC A");
        _addressRegister.value--;
    }
}