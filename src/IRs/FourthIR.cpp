#include "FourthIR.hpp"

void FourthIR::parse(std::vector<Block> b)
{
    _notYetConvertedBlocks = b;
    _symbolTable->assignMemory();
    convertToAssembler();
}

void FourthIR::convertToAssembler()
{
    RegisterBlock registerBlock(_symbolTable);
    registerBlock.createRegisters();
    convertBlockToAssembler(_notYetConvertedBlocks.front(), registerBlock);
}

void FourthIR::convertBlockToAssembler(Block& block, RegisterBlock registerBlock)
{
    Block resultBlock;
    resultBlock.blockName = block.blockName;
    for(auto& l : block.lines)
    {
        if (l.operation == "CONST")
        {
            handleConst(registerBlock, resultBlock, l);
        }
        if (l.operation == "WRITE")
        {
            handleWrite(registerBlock, resultBlock, l);
        }
        if (l.operation == "READ")
        {
            handleRead(registerBlock, resultBlock, l);
        }
        if (l.operation == "HALT")
        {
            resultBlock.lines.push_back({"HALT"});
        }
    }
    _blocks.push_back(resultBlock);
}
