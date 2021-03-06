#include "IRBase.hpp"

#include <algorithm>

unsigned int IRBase::_nextGeneratedVariableSuffix = 0;
unsigned int IRBase::_blockCounter = 0;
unsigned int IRBase::_conditionCounter = 0;
unsigned int IRBase::_forCounter = 0;

IRBase::IRBase(std::string IRName) : _IRName(IRName)
{
    _currentBlock.push(generateBlock());
}

std::string IRBase::getVariable(std::string value)
{
    // std::cerr<<"generating variable with "<<value<<std::endl;
    std::string result = "variable_" + std::to_string(_nextGeneratedVariableSuffix);
    _nextGeneratedVariableSuffix++;
    if (!_symbolTable)
    {
        std::cout<<"SYMBOL TABLE NOT INITALIZED"<<std::endl;
        exit(2);
    }
    _symbolTable->addConst(result, value);
    return result;
}

std::vector<Block> IRBase::getBlocks()
{
    return _blocks;
}

Block IRBase::generateBlock()
{
    Block block;
    block.blockName = "Block_" + std::to_string(_blockCounter);
    _blockCounter++;
    return block;
}

std::string IRBase::generateForName()
{
    std::string name = "ForBlock_" + std::to_string(_forCounter);
    _forCounter++;
    return name;
}

Block IRBase::generateConditionBlock()
{
    Block block;
    block.blockName = "Block_Condition_" + std::to_string(_conditionCounter);
    _conditionCounter++;
    return block;
}

unsigned int IRBase::getForCounter()
{
    return _forCounter;
}


void IRBase::resetBlocks()
{
    for(auto& b : _blocks)
    {
        b.visited = false;
    }
}

Block& IRBase::getBlockByName(std::string searched, std::vector<Block>& blocks)
{
    auto result = std::find_if(blocks.begin(), blocks.end(),
    [&searched](auto& tested){
        return searched == tested.blockName;
    });     
    if (result == blocks.end())
    {
        std::cout<<"problem with getBlockByName, searching for="<<searched<<std::endl;
    }
    return *result;
}    

void IRBase::setSymbolTable(std::shared_ptr<SymbolTable> symbolTable)
{
    _symbolTable = symbolTable;
}

void IRBase::print(cStrRef fileName)
{
    std::streambuf * buf;
    if (fileName.empty())
    {
        std::cerr<<"**************************"<<std::endl;
        buf = std::cerr.rdbuf();
        std::ostream out(buf);
        writeToStream(out);
    }
    else
    {
        std::ofstream output(fileName);
        buf = output.rdbuf();
        std::ostream out(buf);
        writeToStream(out);
    }
}

void IRBase::writeToStream(std::ostream& stream)
{
    stream<<"#"<<_IRName<<":"<<std::endl;
    for(auto& b : _blocks)
    {
        stream << b;
    }
}