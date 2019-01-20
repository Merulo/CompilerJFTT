#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "Register.hpp"
#include "SymbolTable.hpp"
#include "DataTypes/Block.hpp"

#include "Calculators/NumberGenerator.hpp"

class RegisterBlock
{
    public:
    RegisterBlock(){}
    RegisterBlock(std::shared_ptr<SymbolTable> symbolTable);
    RegisterBlock(const RegisterBlock &rb);

    void createRegisters();
    void exitBlock(Block& b);
    void exitBlock(Block& b, RegisterBlock& other);

    Register& getRegistersForOperation(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> r);
    Register& getRegister(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> r, 
        bool changeValue, bool load, bool extra = true);
    void storeSameTable(std::string name, Block& b, bool extra);

    void print();

    private:
    std::vector<Line> generateNumber(unsigned long long& firstNumber, unsigned long long second, Register& freeRegister);
    std::vector<Line> generateNumberFrom(unsigned long long& firstNumber, unsigned long long second, Register& freeRegister);
    std::vector<Line> generateNumberShift(unsigned long long value, Register& freeRegister, std::string operation);

    void loadFromMemory(Block& b, std::string name, Register& r, Register& freeRegister);
    void loadVariableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister);
    void loadConstTableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister, unsigned long long diff);
    void loadVarTableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister, std::string addresVariableName);

    void saveToMemory(Block& b, Register& r, Register& freeRegister);
    void saveVariableToMemory(Block& b, Register& r, Register& freeRegister);
    void saveConstTableToMemory(Block& b, Register& r, Register& freeRegister, unsigned long long diff);
    void saveVarTableToMemory(Block& b, Register& r, Register& freeRegister, std::string name);

    bool isDigits(const std::string &str)
    {
        return std::all_of(str.begin(), str.end(), ::isdigit);
    }

    std::shared_ptr<SymbolTable> _symbolTable;
    Register _addressRegister;
    Register _registerH;
    std::vector<Register> _registers;
    unsigned int _currentRegister;
};