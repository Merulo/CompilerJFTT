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
        resultBlock.lines.push_back({"#performing " + l.toString()});     
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
        if (l.operation == "COPY")
        {
            handleCopy(registerBlock, resultBlock, l);
        }
        if (l.operation == "ADD")
        {
            handleDirectTranslation(registerBlock, resultBlock, l);
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
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithoutLoading(rb, r, b, l.one);
    r.variableName = l.one;
    
    b.lines.push_back({"#generating number"});
    auto lines = NumberGenerator::generateConstFrom(std::stoull(l.two), {{r.name, 0}});
    b.lines.insert(b.lines.end(), lines.begin(), lines.end()); 
    b.lines.push_back({"#end of generating number"});

    updateRegisterStateWithConst(b, rb, r, l.one);
    r.constValue = std::stoull(l.two);

    b.lines.push_back({"#end of performing const"});
}

void FourthIR::handleWrite(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithLoading(rb, r, b, l.one);

    b.lines.push_back({"PUT", r.name});     

    updateRegisterStateWithConst(b, rb, r, l.one);
    b.lines.push_back({"#end of performing write"});    
}


void FourthIR::handleRead(RegisterBlock& rb, Block& b, Line& l)
{   
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithoutLoading(rb, r, b, l.one);
    r.variableName = l.one;

    b.lines.push_back({"GET", r.name});   

    updateRegisterState(b, rb, r, l.one);
    b.lines.push_back({"#end of performing write"});    
}

void FourthIR::handleCopy(RegisterBlock& rb, Block& b, Line& l)
{   
    //TODO: prevent regTwo from being the register containing variable needed by l.one
    Register& regTwo = rb.getUniqueRegisterForVariable(l.two, b, {});
    prepareRegisterWithLoading(rb, regTwo, b, l.two);
    updateRegisterState(b, rb, regTwo, l.two);
    regTwo.variableName = l.two;

    Register& regOne = rb.getUniqueRegisterForVariable(l.one, b, {regTwo});
    prepareRegisterWithoutLoading(rb, regOne, b, l.one);
    updateRegisterState(b, rb, regOne, l.one);
    regOne.variableName = l.one;

    b.lines.push_back({"COPY", regOne.name, regTwo.name});  

    regOne.constValue = regTwo.constValue;
    b.lines.push_back({"#end of performing copy"});   
}

void FourthIR::handleDirectTranslation(RegisterBlock& rb, Block& b, Line& l)
{   
    //TODO: prevent regTwo from being the register containing variable needed by l.one
    Register& regTwo = rb.getUniqueRegisterForVariable(l.two, b, {});
    prepareRegisterWithLoading(rb, regTwo, b, l.two);
    updateRegisterState(b, rb, regTwo, l.two);
    regTwo.variableName = l.two;

    Register& regOne = rb.getUniqueRegisterForVariable(l.one, b, {regTwo});
    prepareRegisterWithLoading(rb, regOne, b, l.one);
    updateRegisterState(b, rb, regOne, l.one);
    regOne.variableName = l.one;

    b.lines.push_back({l.operation, regOne.name, regTwo.name});  

    regOne.constValue = regTwo.constValue;
    b.lines.push_back({"#end of performing copy"});   
}

void FourthIR::updateRegisterState(Block& b, RegisterBlock& rb, Register& r, std::string name)
{
    if (_symbolTable->isItVariable(name))
    {
        r.state = RegisterState::VARIABLE;
    }
    else if (_symbolTable->isItTable(name))
    {
        r.state = RegisterState::TABLE;
    }
    else
    {
        r.state = RegisterState::CONST;
    }       
}

void FourthIR::updateRegisterStateWithConst(Block& b, RegisterBlock& rb, Register& r, std::string name)
{
    if (_symbolTable->isItVariable(name))
    {
        r.state = RegisterState::CONSTVARIABLE;
    }
    else if (_symbolTable->isItTable(name))
    {
        r.state = RegisterState::TABLE;
    }
    else
    {
        r.state = RegisterState::CONST;
    }       
}

void FourthIR::prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, std::string name)
{
    b.lines.push_back({"#loading " + name + " from memory"}); 
    prepareRegisterWithoutLoading(rb, r, b, name);

    if(r.variableName != name)
    {
        if (_symbolTable->isItVariable(name))
        {
            auto lines = rb.performMemoryOperation("LOAD", r, _symbolTable->getMemoryCell(name));
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
        }
        else if (_symbolTable->isItTable(name))
        {
            auto lines = rb.performTableMemoryOperation("LOAD", name, r);
            b.lines.insert(b.lines.end(), lines.begin(), lines.end()); 
        }
    }
    r.variableName = name;
    b.lines.push_back({"#end of loading " + name + " from memory"});       
}


void FourthIR::prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, std::string name)
{
    b.lines.push_back({"#storing " + r.variableName + " to memory"});    
    if (r.variableName != name)
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