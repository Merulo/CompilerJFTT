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

    std::vector<Register> getRegisters()
    {
        return _registers;
    }
    void setAddressRegisterAsUnkown()
    {
        _addressRegister.constValue = rand();
        _addressRegister.variableName = "";
        _addressRegister.state = RegisterState::UNKNOWN;       
    }
    void setUnkown(unsigned int i)
    {
        _registers[i].constValue = rand();
        _registers[i].variableName = "";
        _registers[i].state = RegisterState::UNKNOWN;
    }
    void setRegister(unsigned int i, Register r)
    {
        _registers[i] = r;
    }

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