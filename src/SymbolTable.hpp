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
    std::string checkConstTableAccess(std::string tableName, unsigned long long value);
    void setInitialized(std::string name);

    unsigned long long getMemoryCell(std::string name);
    std::string getConstValue(std::string name)
    {
        return _consts[name];
    }

    void addToIterators(std::string name)
    {
        Iterator i;
        _currentIterators[name] = i;
        _allIterators[name] = i;
        Iterator iForControl;
        std::string newName = name + forControlName;
        _allIterators[newName] = iForControl;
    }

    std::string getForControl()
    {
        return forControlName;
    }

    void removeIterator(std::string name)
    {
        _currentIterators.erase(name);
    }

    bool isItTable(std::string var)
    {
        std::string table = var.substr(0, var.find('('));
        return _tables.find(table) != _tables.end();
    }

    bool isItVariable(std::string var)
    {
        if (_variables.find(var) != _variables.end())
        {
            return true;
        }
        if (_allIterators.find(var) != _allIterators.end())
        {
            return true;
        }

        return false;
    }

    unsigned long long getTableShift(std::string var)
    {
        return _tables[var].beginIndex;
    }

    std::string getExtraVariable()
    {
        return extraVariable;
    }

    void print();
    bool isNameTaken(std::string name);
    
    private:
    std::string extraVariable = "IF_CONTROL_VARIABLE";
    std::string extraVariable2 = "TABLE_EXTRA_VARIABLE";
    std::string forControlName = "_forControl";
    std::map<std::string, Variable> _variables;
    std::map<std::string, Table> _tables;
    std::map<std::string, std::string> _consts;
    std::map<std::string, Iterator> _currentIterators;
    std::map<std::string, Iterator> _allIterators;
};
