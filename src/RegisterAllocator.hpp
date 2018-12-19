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
    ~RegisterAllocator()
    {
        for(auto& s : _lines)
        {
            _outputFile<<s<<std::endl;
        }
    }

    void compile(const std::string& fileName, std::vector<Line> _lines);
    void print();

    private:
    void handleRead(std::string& variableName)
    {
        Register& r = getRegisterForVariable(variableName);
        r.variableName = variableName;
        _lines.push_back("GET " + r.registerName);
    }

    void handleWrite(std::string& variableName)
    {
        Register * r = checkIfValueInRegister(variableName);
        if(r)
        {
            _lines.push_back("PUT " + r->registerName);
        }
        //todo: load if not in register!
    }

    Register * checkIfValueInRegister(std::string name)
    {
        for(auto&r : _registers)
        {
            if(r.variableName == name)
            {
                return &r;
            }
        }
        return nullptr;
    } 

    Register& getRegisterForVariable(std::string name)
    {
        for(auto& r : _registers)
        {
            if(r.variableName == name)
            {
                return r;
            }
        }

        //todo: save value to memory!
        Register r = _registers.front();
        _registers.pop_front();
        _registers.push_back(r);
        return _registers.back();
    }
    
    std::vector<std::string> _lines;
    std::ofstream _outputFile;
    SymbolTable& _symbolTable;
    Register _addressRegister;
    std::list<Register> _registers;


};