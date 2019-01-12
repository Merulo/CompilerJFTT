#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>

#include "SymbolTable.hpp"
#include "DataTypes/Block.hpp"

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
    Block generateConditionBlock();
    std::string generateForName();
    Block& getBlockByName(std::string searched, std::vector<Block>& blocks);
    static unsigned int getForCounter()
    {
        return _forCounter;
    }

 
    void resetBlocks()
    {
        for(auto& b : _blocks)
        {
            b.visited = false;
        }
    }
   

    std::shared_ptr<SymbolTable> _symbolTable;
    std::vector<Block> _blocks;
    std::stack<Block> _currentBlock;
    std::string forControlName = "_forControl";

    private:
    void writeToStream(std::ostream& stream);

    std::string _IRName;
    static unsigned int _blockCounter;
    static unsigned int _forCounter;
    static unsigned int _conditionCounter;
    static unsigned int _nextGeneratedVariableSuffix;
};