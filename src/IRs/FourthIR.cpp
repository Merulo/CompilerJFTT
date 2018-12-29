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
    registerBlock.print();
}


void FourthIR::handleConst(RegisterBlock& rb, Block& b, Line& l)
{
    b.lines.push_back({"#performing " + l.toString()});
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithoutLoading(rb, r, b, l);
    r.variableName = l.one;
    
    b.lines.push_back({"#generating number"});
    auto lines = NumberGenerator::generateConstFrom(std::stoull(l.two), {{r.name, 0}});
    b.lines.insert(b.lines.end(), lines.begin(), lines.end()); 
    b.lines.push_back({"#end of generating number"});

    updateRegisterStateWithConst(b, rb, r, l);
    r.constValue = std::stoull(l.two);

    b.lines.push_back({"#end of performing const"});
}

void FourthIR::handleWrite(RegisterBlock& rb, Block& b, Line& l)
{
    b.lines.push_back({"#performing " + l.toString()});    
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithLoading(rb, r, b, l);

    b.lines.push_back({"PUT", r.name});     

    updateRegisterStateWithConst(b, rb, r, l);
    b.lines.push_back({"#end of performing write"});    
}


void FourthIR::handleRead(RegisterBlock& rb, Block& b, Line& l)
{
    b.lines.push_back({"#performing " + l.toString()});   
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithoutLoading(rb, r, b, l);
    r.variableName = l.one;

    b.lines.push_back({"GET", r.name});   

    updateRegisterState(b, rb, r, l);
    b.lines.push_back({"#end of performing write"});    
}

void FourthIR::handleCopy(RegisterBlock& rb, Block& b, Line& l)
{

}

void FourthIR::updateRegisterState(Block& b, RegisterBlock& rb, Register& r, Line l)
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

void FourthIR::updateRegisterStateWithConst(Block& b, RegisterBlock& rb, Register& r, Line l)
{
    if (_symbolTable->isItVariable(l.one))
    {
        r.state = RegisterState::CONSTVARIABLE;
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

void FourthIR::prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, Line l)
{
    b.lines.push_back({"#loading " + l.one + " from memory"}); 
    prepareRegisterWithoutLoading(rb, r, b, l);

    if(r.variableName != l.one)
    {
        if (_symbolTable->isItVariable(l.one))
        {
            auto lines = rb.performMemoryOperation("LOAD", r, _symbolTable->getMemoryCell(l.one));
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
        }
        else
        {
            auto lines = rb.performTableMemoryOperation("LOAD", l.one, r);
            b.lines.insert(b.lines.end(), lines.begin(), lines.end()); 
        }
    }
    r.variableName = l.one;
    b.lines.push_back({"#end of loading " + l.one + " from memory"});       
}


void FourthIR::prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, Line l)
{
    b.lines.push_back({"#storing " + r.variableName + " to memory"});    
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
    }
    b.lines.push_back({"#end of storing " + r.variableName + " to memory"});    
}