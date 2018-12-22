#pragma once

#include <list>
#include <fstream>
#include <queue>

#include "DataTypes/Line.hpp"
#include "DataTypes/Register.hpp"
#include "SymbolTable.hpp"

class RegisterAllocator
{
    public:
    RegisterAllocator(SymbolTable& symbolTable);
    ~RegisterAllocator();

    void compile(const std::string& fileName, std::vector<Line> _lines);
    void print();


    private:
    void handleConst(std::string variableName, ull value);
    void handleRead(std::string& variableName);
    void handleWrite(std::string& variableName);
    void handleCopy(Line line);
    void handleSimpleMath(Line line);

    //todo upgrade this so that less calls to saving [Curent state: ROUND ROBIN]
    std::pair<Register, bool> getRegisterForVariable(std::string name);
    void prepareAddressRegister(Register& r);
    void loadIfNeeded(Register& r, bool value)
    {
        if(!value)
        {
            prepareAddressRegister(r);
            r.value = 0;
            _lines.push_back("LOAD " + r.registerName);

        }
    }
    
    std::vector<std::string> _lines;
    std::ofstream _outputFile;
    SymbolTable& _symbolTable;
    AddressRegister _addressRegister;
    std::list<Register> _registers;


};