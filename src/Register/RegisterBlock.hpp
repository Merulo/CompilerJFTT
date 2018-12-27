#pragma once

#include <vector>
#include <memory>

#include "Register.hpp"
#include "SymbolTable.hpp"

class RegisterBlock
{
    public:
    RegisterBlock(std::shared_ptr<SymbolTable> symbolTable);
    void createRegisters();
    std::vector<Line> loadVariableToRegister(Register& r, std::string varName);
    std::vector<Line> performMemoryOperation(std::string operation, Register& r, unsigned long long memoryCell);
    Register& getRegisterForVariable(std::string name);
    void print();

    private:
    std::shared_ptr<SymbolTable> _symbolTable;
    Register _addressRegister;
    std::vector<Register> _registers;
    unsigned int _currentRegister;
};