#include "RegisterAllocator.hpp"

RegisterAllocator::RegisterAllocator(SymbolTable& symbolTable)
    : _symbolTable(symbolTable)
{
    _addressRegister.registerName = 'A';

    for(int i = 1; i < 8; i++)
    {
        Register r;
        std::string str;
        str.push_back(char (int ('A') + i));
        r.registerName = str;
        _registers.push_back(r);
    }
    
}

void RegisterAllocator::print()
{
    std::cerr<<"**************************"<<std::endl;
    std::cerr<<"Registers:"<<std::endl;
    std::cerr<<_addressRegister.registerName<<" "<<_addressRegister.value<<" "<<_addressRegister.variableName<<std::endl;
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