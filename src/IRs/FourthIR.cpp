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


void FourthIR::handleConst(RegisterBlock& rb, Block& b, Line& l)
{
    Register& r = rb.getRegisterForVariable(l.one);
    prepareRegisterWithoutLoading(rb, r, b, l);

    auto lines = NumberGenerator::generateConstFrom(std::stoull(l.two), {{r.name, 0}});
    b.lines.insert(b.lines.end(), lines.begin(), lines.end()); 

    updateRegisterState(b, rb, r, l);

    if (_symbolTable->isItVariable(l.one))
    {
        r.state = RegisterState::CONSTVARIABLE;
    }
}

void FourthIR::handleWrite(RegisterBlock& rb, Block& b, Line& l)
{
    Register& r = rb.getRegisterForVariable(l.one);
    prepareRegisterWithLoading(rb, r, b, l);

    b.lines.push_back({"PUT", r.name});     

    updateRegisterState(b, rb, r, l);
}

void FourthIR::updateRegisterState(Block& b, RegisterBlock rb, Register& r, Line l)
{
    if (_symbolTable->isItVariable(l.one))
    {
        r.state = RegisterState::VARIABLE;
    }
    else if (_symbolTable->isItTable(l.one))
    {
        r.state = RegisterState::TABLE;
    }
    else
    {
        r.state = RegisterState::CONST;
    }       
}

void FourthIR::handleRead(RegisterBlock& rb, Block& b, Line& l)
{
    Register& r = rb.getRegisterForVariable(l.one);
    prepareRegisterWithoutLoading(rb, r, b, l);
    
    b.lines.push_back({"GET", r.name});   

    updateRegisterState(b, rb, r, l);
}

void FourthIR::prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, Line l)
{
    if (r.variableName != l.one)
    {
        if (r.state == RegisterState::CONSTVARIABLE || r.state == RegisterState::VARIABLE)
        {
            auto lines = rb.performMemoryOperation("STORE", r, _symbolTable->getMemoryCell(r.variableName));
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());

            lines = rb.performMemoryOperation("LOAD", r, _symbolTable->getMemoryCell(l.one));
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
        }
        else if (r.state == RegisterState::TABLE)
        {
            auto lines = rb.performTableMemoryOperation("STORE", r.variableName, r);
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());

            lines = rb.performTableMemoryOperation("LOAD", l.one, r);
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
        }
        r.variableName = l.one;  
    }
    
}


void FourthIR::prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, Line l)
{
    if (r.variableName != l.one)
    {
        if (r.state == RegisterState::CONSTVARIABLE || r.state == RegisterState::VARIABLE)
        {
            auto lines = rb.performMemoryOperation("STORE", r, _symbolTable->getMemoryCell(r.variableName));
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
        }
        else if (r.state == RegisterState::TABLE)
        {
            auto lines = rb.performTableMemoryOperation("STORE", r.variableName, r);
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
        }
        r.variableName = l.one;
    }
}