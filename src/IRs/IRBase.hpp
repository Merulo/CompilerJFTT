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
    // std::vector<Line> getLines();
    void setSymbolTable(std::shared_ptr<SymbolTable> symbolTable);

    protected:
    void generateBlock()
    {
        Block block;
        block.blockName = "Block_" + std::to_string(_blockCounter);
        _blockCounter++;
        _currentBlock.push(block);
    }

    std::shared_ptr<SymbolTable> _symbolTable;
    std::vector<Block> _blocks;
    std::stack<Block> _currentBlock;

    private:
    unsigned int _blockCounter = 0;
    void writeToStream(std::ostream& stream);
    std::string _IRName;
};