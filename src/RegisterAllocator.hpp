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
    void handleRead(std::string& variableName);
    void handleWrite(std::string& variableName);

    //todo upgrade this so that less calls to saving [Curent state: ROUND ROBIN]
    std::pair<Register&, bool> getRegisterForVariable(std::string name);
    void prepareAddressRegister(Register& r);
    
    std::vector<std::string> _lines;
    std::ofstream _outputFile;
    SymbolTable& _symbolTable;
    AddressRegister _addressRegister;
    std::list<Register> _registers;


};