#include "FourthIR.hpp"

#include "Calculators/MathOperations.hpp"
#include "Calculators/NumberGenerator.hpp"

void FourthIR::parse(std::vector<Block> bs)
{
    std::cout<<std::endl<<std::endl;
    _notYetConvertedBlocks = bs;
    _symbolTable->assignMemory();
    convertToAssembler();

}

void FourthIR::convertToAssembler()
{
    RegisterBlock registerBlock(_symbolTable);
    registerBlock.createRegisters();
    convertBlockToAssembler(_notYetConvertedBlocks.front(), registerBlock);
}

void FourthIR::convertBlockToAssembler(Block& block, RegisterBlock& registerBlock)
{
    std::cout<<"converting "<<block.blockName<<std::endl;
    // registerBlock.print();

    Block resultBlock;
    resultBlock.blockName = block.blockName;
    for(auto& l : block.lines)
    {
        std::cout<<l<<std::endl;
        resultBlock.lines.push_back({"\t#performing " + l.toString()});     
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
        if (l.operation == "ADD" || l.operation == "SUB")
        {
            handleDirectTranslation(registerBlock, resultBlock, l);
        }        
        if (l.operation == "HALT")
        {
            resultBlock.lines.push_back({"HALT"});
        }
        if (l.operation == "JUMP")
        {
            resultBlock.lines.push_back(l);
        }
        if (l.operation == "JZERO" || l.operation == "JODD" || l.operation == "INC" || l.operation == "DEC")
        {
            handleSimpleOperation(registerBlock, resultBlock, l);
        }
        // if (l.operation == "MUL")
        // {
        //     handleMul(registerBlock, resultBlock, l);
        // }
        // if (l.operation == "DIV")
        // {
        //     handleDiv(registerBlock, resultBlock, l);
        // }
        // if (l.operation == "MOD")
        // {
        //     handleMod(registerBlock, resultBlock, l);
        // }            
    }
    // registerBlock.exitBlock(resultBlock);
    _blocks.push_back(resultBlock);
    continueConverting(block, registerBlock);
}

void FourthIR::handleWrite(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& r = rb.getRegister(l.one, b, {});
    r.variableName = l.one;

    b.lines.push_back({"PUT", r.name});     
    updateRegisterState(b, rb, r, l.one);

    if (_symbolTable->isConst(l.one))
    {
        r.variableName = "";
        r.state = RegisterState::UNKNOWN;
    }

    b.lines.push_back({"\t#end of performing write"});    
}


void FourthIR::handleRead(RegisterBlock& rb, Block& b, Line& l)
{   
    Register& r = rb.getRegister(l.one, b, {});
    r.variableName = l.one;

    b.lines.push_back({"GET", r.name});    
    updateRegisterState(b, rb, r, l.one);

    b.lines.push_back({"\t#end of performing write"});    
}

void FourthIR::handleConst(RegisterBlock& rb, Block& b, Line& l)
{
    Register& r = rb.getRegister(l.one, b, {});
    r.variableName = l.one;

    b.lines.push_back({"\t#generating number"});
    auto lines = NumberGenerator::generateConstFrom(std::stoull(l.two), {{r.name, 0}});
    b.lines.insert(b.lines.end(), lines.begin(), lines.end()); 
    b.lines.push_back({"\t#end of generating number"});

    updateRegisterState(b, rb, r, l.one);
    r.constValue = std::stoull(l.two);
}

void FourthIR::handleCopy(RegisterBlock& rb, Block& b, Line& l)
{   
    Register& regOne = rb.getRegister(l.one, b, {});
    updateRegisterState(b, rb, regOne, l.one);
    regOne.variableName = l.one;

    Register& regTwo = rb.getSecondRegister(l.two, b, {});

    //todo remove this if unneeded
    if (_symbolTable->isConst(l.two))
    {
        regTwo.variableName = "";
        regTwo.state = RegisterState::UNKNOWN;
    }

    b.lines.push_back({"COPY", regOne.name, regTwo.name});  

    regOne.constValue = regTwo.constValue;
    b.lines.push_back({"\t#end of performing copy"});   
}

void FourthIR::handleDirectTranslation(RegisterBlock& rb, Block& b, Line& l)
{   
    Register& regTwo = rb.getSecondRegister(l.two, b, {});

    Register& regOne = rb.getRegister(l.one, b, {});
    updateRegisterState(b, rb, regOne, l.one);
    regOne.variableName = l.one;

    b.lines.push_back({l.operation, regOne.name, regTwo.name});  

    if (_symbolTable->isConst(l.two))
    {
        regTwo.variableName = "";
        regTwo.state = RegisterState::UNKNOWN;
    }

    regOne.constValue = regTwo.constValue;
    b.lines.push_back({"\t#end of performing operation"});   
}

void FourthIR::handleSimpleOperation(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& r = rb.getRegister(l.one, b, {});

    b.lines.push_back({l.operation, r.name, l.two});     

    updateRegisterState(b, rb, r, l.one);
    b.lines.push_back({"\t#end of performing simple operation"});    
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


void FourthIR::continueConverting(Block& block, RegisterBlock rb)
{
    std::cout<<block.blockName<<std::endl;
    rb.print();

    if (!block.blockJump.empty())
    {
        Block& next = getBlockByName(block.blockJump, _notYetConvertedBlocks);
        convertBlockToAssembler(next, rb);
    }
    else if (!block.blockIfFalse.empty() && !block.blockIfTrue.empty())
    {
        std::cout<<"handling split"<<std::endl;

        Block& nextTrue = getBlockByName(block.blockIfTrue, _notYetConvertedBlocks);
        convertBlockToAssembler(nextTrue, rb);

        Block& nextFalse = getBlockByName(block.blockIfFalse, _notYetConvertedBlocks);
        convertBlockToAssembler(nextFalse, rb);
    }

}