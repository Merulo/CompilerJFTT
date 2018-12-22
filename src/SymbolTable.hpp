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
    std::string addTable(std::string name, ull beginIndex, ull endIndex);
    std::string addVariable(std::string name);
    void assignMemory();
    void setConsts(std::map<std::string, std::string> consts);
    bool isConst(std::string name);

    std::string checkVariableExists(std::string name);
    std::string checkVariableExistsAndIsInitialized(std::string name);
    std::string checkVariableIsTable(std::string name);
    std::string checkVariableIsVariable(std::string name);
    void setInitialized(std::string name);

    unsigned long long getMemoryCell(std::string name);

    void print();
    
    private:
    bool isNameTaken(std::string name);
    std::map<std::string, Variable> _variables;
    std::map<std::string, Table> _tables;
    std::map<std::string, std::string> _consts;
    // std::map<std::string, Iterator> _iterators;
};
