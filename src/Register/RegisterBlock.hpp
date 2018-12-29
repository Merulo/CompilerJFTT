#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "Register.hpp"
#include "SymbolTable.hpp"

class RegisterBlock
{
    public:
    RegisterBlock(std::shared_ptr<SymbolTable> symbolTable);
    void createRegisters();

    std::vector<Line> performMemoryOperation(std::string operation, Register& r, unsigned long long memoryCell);
    std::vector<Line> performTableMemoryOperation(std::string operation, std::string varName, Register& r);
    
    Register& getUniqueRegisterForVariable(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> r);
    
    void print();

    private:
    
    void generateNumber(std::vector<Line>& lines, unsigned long long& firstNumber, unsigned long long second);
    void saveTablesIfNeeded(std::string name, Block& b);
    void saveTableToMemory(Block& b, Register& r);
    std::vector<Line> prepareAddressRegister(unsigned long long memoryCell);

    bool isDigits(const std::string &str)
    {
        return std::all_of(str.begin(), str.end(), ::isdigit);
    }
    

    std::shared_ptr<SymbolTable> _symbolTable;
    Register _addressRegister;
    std::vector<Register> _registers;
    unsigned int _currentRegister;
};