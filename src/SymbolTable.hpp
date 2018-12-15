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
    std::string addTable(const std::string& name, ull beginIndex, ull endIndex);
    std::string addVariable(const std::string& name);
    

    std::string checkVariableExists(const std::string& name);
    std::string checkVariableExistsAndIsInitialized(const std::string& name);
    std::string checkVariableIsTable(const std::string& name);
    std::string checkVariableIsVariable(const std::string& name);
    void setInitialized(const std::string& name);

    void print();
    
    private:
    bool isNameTaken(const std::string& name);
    std::map<std::string, Variable> _variables;
    std::map<std::string, Table> _tables;
    // std::map<std::string, Iterator> _iterators;
};
