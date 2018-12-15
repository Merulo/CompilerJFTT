#include "SymbolTable.hpp"

std::string SymbolTable::addVariable(const std::string& name)
{
    if (isNameTaken(name))
    {
        return "second " + name + " declaration!";
    }
    Variable v;
    _variables[name] = v;
    return "";
}

std::string SymbolTable::addTable(const std::string& name, ull beginIndex, ull endIndex)
{
    if (isNameTaken(name))
    {
        return "second " + name + " declaration!";
    }
    if (beginIndex > endIndex)
    {
        return "wrong " + name + " table size!";
    }
    Table t;
    _tables[name] = t;
    return "";
}

std::string SymbolTable::checkVariableExists(const std::string& name)
{
    if (! isNameTaken(name))
    {
        return "undeclared variable " + name;
    }
    if (_variables.find(name) != _variables.end())
    {
        _variables[name].isInitialized = true;
    }
    return "";
}

std::string SymbolTable::checkVariableIsTable(const std::string& name)
{
    if (_tables.find(name) == _tables.end() && _variables.find(name) != _variables.end())
    {
        return "incorrect usage of variable " + name;
    }
    return "";
}

std::string SymbolTable::checkVariableIsVariable(const std::string& name)
{
    if (_variables.find(name) == _variables.end() && _tables.find(name) != _tables.end())
    {
        return "incorrect usage of table " + name;
    }
    return "";
}

std::string SymbolTable::checkVariableExistsAndIsInitialized(const std::string& name)
{
    if (! isNameTaken(name))
    {
        return "undeclared variable " + name;
    }
    if (_variables.find(name) != _variables.end())
    {
        if (! _variables[name].isInitialized)
        {
            return "using uninitialized variable " + name;
        }
    }

    return "";
}






bool SymbolTable::isNameTaken(const std::string& name)
{
    if(_variables.find(name) != _variables.end())
    {
        return true;
    }
    if(_tables.find(name) != _tables.end())
    {
        return true;
    }
    return false;
}
