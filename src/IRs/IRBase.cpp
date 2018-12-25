#include "IRBase.hpp"

IRBase::IRBase(std::string IRName) : _IRName(IRName)
{
    _currentBlock.push(generateBlock());
}

std::vector<Block> IRBase::getBlocks()
{
    return _blocks;
}

Block IRBase::generateBlock()
{
    Block block;
    block.blockName = "Block_" + std::to_string(_blockCounter);
    std::cerr<<"generating block "<<block.blockName<<std::endl;
    _blockCounter++;
    return block;
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
    stream<<_IRName<<":"<<std::endl;
    for(auto& b : _blocks)
    {
        stream << b;
    }
}