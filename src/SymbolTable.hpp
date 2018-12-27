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
    ull memoryCell = 0;
};


class SymbolTable
{
    public:
    std::string addTable(std::string name, ull beginIndex, ull endIndex);
    std::string addVariable(std::string name);
    void assignMemory();
    void addConst(std::string key, std::string value);
    bool isConst(std::string name);

    std::string checkVariableExists(std::string name);
    std::string checkVariableExistsAndIsInitialized(std::string name);
    std::string checkVariableIsTable(std::string name);
    std::string checkVariableIsVariable(std::string name);
    void setInitialized(std::string name);

    unsigned long long getMemoryCell(std::string name);

    void addToIterators(std::string name)
    {
        Iterator i;
        _currentIterators[name] = i;
        _allIterators[name] = i;
    }

    void removeIterator(std::string name)
    {
        _currentIterators.erase(name);
    }

    void print();
    bool isNameTaken(std::string name);
    
    private:
    std::map<std::string, Variable> _variables;
    std::map<std::string, Table> _tables;
    std::map<std::string, std::string> _consts;
    std::map<std::string, Iterator> _currentIterators;
    std::map<std::string, Iterator> _allIterators;
};
