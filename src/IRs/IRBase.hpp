#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>

#include "SymbolTable.hpp"
#include "DataTypes/Line.hpp"

typedef const std::string & cStrRef;

class IRBase
{
    public:
    IRBase(std::string IRName);
    void print(cStrRef fileName = "");
    void setSymbolTable(std::shared_ptr<SymbolTable> symbolTable);
    std::vector<Block> getBlocks();
    std::string getVariable(std::string value);

    protected:
    Block generateBlock();

    std::shared_ptr<SymbolTable> _symbolTable;
    std::vector<Block> _blocks;
    std::stack<Block> _currentBlock;
    std::string forControlName = "_forControl";

    private:
    void writeToStream(std::ostream& stream);

    std::string _IRName;
    unsigned int _blockCounter = 0;
    static unsigned int _nextGeneratedVariableSuffix;
};