#pragma once

#include <vector>
#include <memory>

#include "Register.hpp"
#include "SymbolTable.hpp"

class RegisterBlock
{
    public:
    RegisterBlock(std::shared_ptr<SymbolTable> symbolTable)
    {
        _symbolTable = symbolTable;
        _currentRegister = 0;
    }
    void createRegisters()
    {
        for(size_t i = 1; i < 8; i++){
            Register r;
            char c =  'A' + i;
            r.name = c;
            _registers.push_back(r);
        }
    }

    std::vector<Line> loadVariableToRegister(Register& r, std::string varName)
    {
        if (_symbolTable->checkVariableIsVariable(varName).empty())
        {
            std::cerr<<"loading variable"<<std::endl;
            return performMemoryOperation("LOAD", r, _symbolTable->getMemoryCell(varName));
        }
        else if (_symbolTable->checkVariableIsTable(varName).empty())
        {
            std::cerr<<"loading table"<<std::endl;
        }
        else
        {
            std::cerr<<"SOMETHING WENT WRONG"<<std::endl;
        }
        return {};
    }

    std::vector<Line> performMemoryOperation(std::string operation, Register& r, unsigned long long memoryCell)
    {
        std::vector<Line> lines;
        if (_addressRegister.state == RegisterState::UNKNOWN)
        {
            lines.push_back({"SUB", "A", "A"});
            _addressRegister.constValue = 0;
            _addressRegister.state = RegisterState::CONST;
        }

        while(_addressRegister.constValue < memoryCell)
        {
            lines.push_back({"INC", "A", ""});
            _addressRegister.constValue++;
        }
        while(_addressRegister.constValue > memoryCell)
        {
            lines.push_back({"DEC", "A", ""});
            _addressRegister.constValue--;
        }
        lines.push_back({operation , r.name});
        return lines;
    }
    
    Register& getRegisterForVariable(std::string name)
    {
        for(auto& r : _registers)
        {
            if (r.state == RegisterState::CONSTVARIABLE || r.state == RegisterState::VARIABLE)
            {
                if (r.variableName == name)
                {
                    return r;
                }
            }
        }

        for(auto&r : _registers)
        {
            if (r.state == RegisterState::UNKNOWN || r.state == RegisterState::TABLE)
            {
                return r;
            }
        }

        for(auto& r : _registers)
        {
            if (r.state == RegisterState::CONST)
            {
                return r;
            }
        }
        Register& r = _registers[_currentRegister];
        _currentRegister++;
        std::cerr<<"SHOULD SAVE CURRENT CONTENT OF REGISTER "<<r<<std::endl;
        if (_currentRegister == _registers.size())
        {
            _currentRegister = 0;
        }
        return r;  
    }

    void print()
    {
        std::cerr<<_addressRegister<<std::endl;
        for(auto& r : _registers)
        {
            std::cerr<<r<<std::endl;
        }
    }

    private:
    std::shared_ptr<SymbolTable> _symbolTable;
    Register _addressRegister;
    std::vector<Register> _registers;
    unsigned int _currentRegister;
};