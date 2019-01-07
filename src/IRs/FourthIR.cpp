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
    if (std::find(_blocks.begin(), _blocks.end(), block) != _blocks.end())
    {
        return;
    }

    std::cout<<"STARTING CONVERTING "<<block.blockName<<std::endl;
    // registerBlock.print();

    Block resultBlock;
    resultBlock.blockName = block.blockName;
    for(auto& l : block.lines)
    {
        // std::cout<<"TEST="<<l<<std::endl;
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
        if (l.operation == "MUL")
        {
            handleMul(registerBlock, resultBlock, l);
        }
        if (l.operation == "DIV")
        {
            handleDiv(registerBlock, resultBlock, l);
        }
        if (l.operation == "MOD")
        {
            handleMod(registerBlock, resultBlock, l);
        }        
        if (l.operation == "#DO NOT REMOVE CONST")
        {
            _removeConsts = false;
        }    
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

    if (_symbolTable->isConst(l.one) && _removeConsts)
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

    if (_symbolTable->isConst(l.two) && _removeConsts)
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

    if (_symbolTable->isConst(l.two) && _removeConsts)
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
    // b.lines.push_back({"CURRENT REGISTER STATE = " + r.variableName});

    b.lines.push_back({l.operation, r.name, l.two});     

    updateRegisterState(b, rb, r, l.one);
        r.variableName = l.one;
    // b.lines.push_back({"CURRENT REGISTER STATE = " + r.variableName});
    b.lines.push_back({"\t#end of performing simple operation"});    
}

void FourthIR::handleMul(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getRegister(l.one, b, {});
    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;

    Register& registerC = rb.getSecondRegister(l.two, b, {registerB});
    registerC.state = RegisterState::UNKNOWN;
    registerC.variableName = "";    

    Register& registerD = rb.getRegistersForOperation("TEMPORARY_2", b, {registerB, registerC});
    registerD.state = RegisterState::UNKNOWN;
    registerD.variableName = "";

    b.lines.push_back({"#using: " + registerB.name + " " + registerC.name + " " + registerD.name});

    auto lines = MathOperations::generateMultiplication(registerB.name, registerC.name, registerD.name, l);
    b.lines.insert(b.lines.end(), lines.begin(), lines.end());

    b.lines.push_back({"\t#end of performing MUL operation"});    
}

void FourthIR::handleDiv(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getRegister(l.one, b, {});
    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;

    Register& registerC = rb.getSecondRegister(l.two, b, {registerB});
    registerC.state = RegisterState::UNKNOWN;
    registerC.variableName = "";

    Register& registerD = rb.getRegistersForOperation("TEMPORARY_1", b, {registerB, registerC});
    registerD.state = RegisterState::UNKNOWN;
    registerD.variableName = "";

    Register& registerE = rb.getRegistersForOperation("TEMPORARY_2", b, {registerB, registerC, registerD});
    registerE.state = RegisterState::UNKNOWN;
    registerE.variableName = "";

    Register& registerF = rb.getRegistersForOperation("TEMPORARY_3", b, {registerB, registerC, registerD, registerE});  
    registerF.state = RegisterState::UNKNOWN;
    registerF.variableName = "";    

    b.lines.push_back({"#using: " + registerB.name + " " + registerC.name + " " + registerD.name + " " + registerE.name + " " + registerF.name});

    auto lines = MathOperations::generateDivision(
        registerB.name, registerC.name, 
        registerD.name, registerE.name,
        registerF.name, l);

    b.lines.insert(b.lines.end(), lines.begin(), lines.end());

    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;

    b.lines.push_back({"\t#end of performing DIV operation"});    
}

void FourthIR::handleMod(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getRegister(l.one, b, {});
    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;

    Register& registerC = rb.getSecondRegister(l.two, b, {registerB});
    registerC.state = RegisterState::UNKNOWN;
    registerC.variableName = "";

    Register& registerE = rb.getRegistersForOperation("TEMPORARY_2", b, {registerB, registerC});
    registerE.state = RegisterState::UNKNOWN;
    registerE.variableName = "";

    Register& registerF = rb.getRegistersForOperation("TEMPORARY_3", b, {registerB, registerC, registerE});  
    registerF.state = RegisterState::UNKNOWN;
    registerF.variableName = "";   

    auto lines = MathOperations::generateModulo(
        registerB.name, registerC.name, 
        registerE.name, registerF.name, l);

    b.lines.insert(b.lines.end(), lines.begin(), lines.end());
    b.lines.push_back({"\t#end of performing DIV operation"});    
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
    _removeConsts = true;
     
    if (!block.blockJump.empty())
    {
        Block& next = getBlockByName(block.blockJump, _notYetConvertedBlocks);

        Block handle = generateBlock();
        rb.exitBlock(handle);
        handle.blockJump = next.blockName;

        Block& grr = getBlockByName(block.blockName, _blocks);
            for(auto& l : grr.lines)
            {
                if (l.two == "#" + block.blockJump)
                {
                    l.two = "#" + handle.blockName;
                }
            }
        handle.lines.push_back({"JUMP", "", "#" + next.blockName});
        _blocks.push_back(handle);
        RegisterBlock copy(rb);
        convertBlockToAssembler(next, copy);
    }
    else if (!block.blockIfFalse.empty() && !block.blockIfTrue.empty())
    {
        std::cout<<"handling split"<<std::endl;
        {
            Block& nextTrue = getBlockByName(block.blockIfTrue, _notYetConvertedBlocks);
            Block handle = generateBlock();
            RegisterBlock copy(rb);
            copy.exitBlock(handle);
            handle.blockJump = nextTrue.blockName;

            Block& grr = getBlockByName(block.blockName, _blocks);

            for(auto& l : grr.lines)
            {
                if (l.two == "#" + block.blockIfTrue)
                {
                    l.two = "#" + handle.blockName;
                }
            }
            handle.lines.push_back({"JUMP", "", "#" +  nextTrue.blockName});
            _blocks.push_back(handle);
                    
            convertBlockToAssembler(nextTrue, copy);
        }

        {
            Block& nextFalse = getBlockByName(block.blockIfFalse, _notYetConvertedBlocks);
            Block handle = generateBlock();
            RegisterBlock copy(rb);
            copy.exitBlock(handle);
            handle.blockJump = nextFalse.blockName;

            Block& grr = getBlockByName(block.blockName, _blocks);
            for(auto& l : grr.lines)
            {
                if (l.two == "#" + block.blockIfFalse)
                {
                    l.two = "#" + handle.blockName;
                }
            }

            handle.lines.push_back({"JUMP", "", "#" + nextFalse.blockName});
            _blocks.push_back(handle);
            convertBlockToAssembler(nextFalse, copy);
        }
    }

}