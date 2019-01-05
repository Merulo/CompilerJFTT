#include "SymbolTable.hpp"

void SymbolTable::print()
{
    std::cerr<<"Variables:" <<std::endl;
    for(auto v : _variables)
    {
        std::cerr<<v.first<<" at="<<v.second.memoryCell<<" ";
        if (v.second.isInitialized)
        {
            std::cerr<<"is initialized ";
        }
        else
        {
            std::cerr<<"is not initialized ";
        }
        std::cerr<<std::endl;
    }
    std::cerr<<"Tables:" <<std::endl;
    for(auto t : _tables)
    {
        std::cerr<<t.first<<std::endl;
    }
}

std::string SymbolTable::addVariable(std::string name)
{
    if (isNameTaken(name))
    {
        return "second " + name + " declaration!";
    }
    Variable v;
    _variables[name] = v;
    return "";
}

unsigned long long SymbolTable::getMemoryCell(std::string name)
{
    if (_variables.find(name) != _variables.end())
    {
        return _variables[name].memoryCell;
    }

    if(_allIterators.find(name) != _allIterators.end())
    {
        return _allIterators[name].memoryCell;
    }

    if (_tables.find(name) != _tables.end())
    {
        return _tables[name].memoryCellStart;
    }
    std::cerr<<"ADDRESS OF "<<name<<" WAS NOT FOUND"<<std::endl;
    return 0;
}

void SymbolTable::assignMemory()
{
    ull index = 1;
    
    //special if control variable!
    Variable v;
    v.memoryCell = 0;

    //variables
    for (auto& v : _variables)
    {
        v.second.memoryCell = index;
        index++;
    }
    _variables[extraVariable] = v;   

    //iterators
    for(auto& i : _allIterators)
    {
        i.second.memoryCell = index;
        index++;
    }

    //tables
    //TODO: use heuristic
    for(auto& t : _tables)
    {
        std::cout<<t.first<<" allocated at "<<index<<std::endl;
        t.second.memoryCellStart = index;
        index += t.second.endIndex - t.second.beginIndex + 1;
    }
}

void SymbolTable::addConst(std::string key, std::string value)
{
    _consts[key] = value;
}

bool SymbolTable::isConst(std::string name)
{
    if (name.find("temporary") != std::string::npos)
    {
        return true;
    }
    return _consts.find(name) != _consts.end();
}

std::string SymbolTable::addTable(std::string name, ull beginIndex, ull endIndex)
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
    t.beginIndex = beginIndex;
    t.endIndex = endIndex;
    _tables[name] = t;
    return "";
}

std::string SymbolTable::checkVariableExists(std::string name)
{
    if (name.find("(") != std::string::npos)
    {
        name = name.substr(0, name.find("("));
    }

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

std::string SymbolTable::checkVariableIsTable(std::string name)
{
    if (_tables.find(name) == _tables.end() && _variables.find(name) != _variables.end())
    {
        return "incorrect usage of variable " + name;
    }
    return "";
}

std::string SymbolTable::checkVariableIsVariable(std::string name)
{
    if (_variables.find(name) == _variables.end() && _tables.find(name) != _tables.end())
    {
        return "incorrect usage of table " + name;
    }
    return "";
}

std::string SymbolTable::checkVariableExistsAndIsInitialized(std::string name)
{
    if (name.find("(") != std::string::npos)
    {
        name = name.substr(0, name.find("("));
    }

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

std::string SymbolTable::checkConstTableAccess(std::string tableName, unsigned long long value)
{
    auto table = _tables[tableName];
    if (value < table.beginIndex)
    {
        return std::to_string(value) + " out of lower bound of " + tableName;
    }
    if (value > table.endIndex)
    {
        return std::to_string(value) + " out of upper bound of " + tableName;
    }

    return "";
}

void SymbolTable::setInitialized(std::string name)
{
    if (_variables.find(name) != _variables.end())
    {
        _variables[name].isInitialized = true;
    }
}

/*
PRIVATE
*/
bool SymbolTable::isNameTaken(std::string name)
{
    if(_variables.find(name) != _variables.end())
    {
        return true;
    }
    if(_tables.find(name) != _tables.end())
    {
        return true;
    }
    if(_currentIterators.find(name) != _currentIterators.end())
    {
        return true;
    }

    return false;
}
