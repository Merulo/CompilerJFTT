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
    RegisterBlock(const RegisterBlock &rb){
        for(auto r : rb._registers)
        {
            Register copy = r;
            _registers.push_back(copy);
        }
        _addressRegister = rb._addressRegister;
        _currentRegister = rb._currentRegister;
        _symbolTable = rb._symbolTable;
    }

    void createRegisters();
    void exitBlock(Block& b);

    Register& getRegister(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> r, bool load = true);
    Register& getSecondRegister(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> r);
    
    void print();

    private:
    void generateNumber(std::vector<Line>& lines, unsigned long long& firstNumber, unsigned long long second, Register& freeRegister);

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
    std::vector<Register> _registers;
    unsigned int _currentRegister;
};