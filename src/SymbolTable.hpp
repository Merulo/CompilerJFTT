#pragma once

#include <map>
#include <string>
#include <iostream>

typedef unsigned long long ull;

struct Variable 
{
    bool isInitialized = false;
    ull memoryCell = 0;
};

struct Table 
{
    ull memoryCellStart = 0;  
    ull beginIndex;
    ull endIndex;
};

struct Iterator 
{
    ull memoryCellStart = 0;
    ull begin;
    ull end;
    bool increment = false;
};


class SymbolTable
{
    public:
    void print()
    {
        std::cerr<<"Variables:" <<std::endl;
        for(auto v : _variables)
        {
            std::cerr<<v.first<<std::endl;
        }
        std::cerr<<"Tables:" <<std::endl;
        for(auto t : _tables)
        {
            std::cerr<<t.first<<std::endl;
        }
    }

    std::string addVariable(const std::string& name);
    std::string addTable(const std::string& name, ull beginIndex, ull endIndex);
    std::string checkVariableExists(const std::string& name);
    std::string checkVariableExistsAndIsInitialized(const std::string& name);
    std::string checkVariableIsTable(const std::string& name);
    std::string checkVariableIsVariable(const std::string& name);

    private:
    bool isNameTaken(const std::string& name);
    std::map<std::string, Variable> _variables;
    std::map<std::string, Table> _tables;
    // std::map<std::string, Iterator> _iterators;
};
