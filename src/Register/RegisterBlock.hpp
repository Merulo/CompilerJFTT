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
    
    Register& getRegisterForVariable(std::string name, Block& b);
    
    void print();

    private:

    bool isDigits(const std::string &str)
    {
        return std::all_of(str.begin(), str.end(), ::isdigit);
    }

    void saveTablesIfNeeded(std::string name, Block& b)
    {
        if(_symbolTable->isItTable(name))
        {
            std::string rest = name.substr(name.find("(") + 1, std::string::npos);
            rest.pop_back();

            std::string array = name.substr(0, name.find("("));
            for(auto& r : _registers)
            {
                if (r.variableName.find(array) != std::string::npos)
                {
                    b.lines.push_back({"#saving " + r.variableName + " to memory"});
                    auto lines = performTableMemoryOperation("STORE", r.variableName, r);
                    b.lines.insert(b.lines.end(), lines.begin(), lines.end());
                    r.variableName = "";
                    r.state = RegisterState::UNKNOWN;
                    b.lines.push_back({"#end of saving to memory"});
                }
            }
        }
    }
    std::vector<Line> prepareAddressRegister(unsigned long long memoryCell);

    std::shared_ptr<SymbolTable> _symbolTable;
    Register _addressRegister;
    std::vector<Register> _registers;
    unsigned int _currentRegister;
};