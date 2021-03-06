#include "FourthIR.hpp"

#include <stdlib.h>

#include "Calculators/MathOperations.hpp"
#include "Calculators/NumberGenerator.hpp"
#include "External/EmulatorRunner.hpp"

void FourthIR::parse(std::vector<Block> bs, std::string fileName, bool useEmulator)
{
    for(auto& b : bs)
    {
        Pair p;
        p.block = b;
        _notYetConvertedBlocks.push_back(p);
    }

    if (isDeterministic() && useEmulator)
    {
        _blocks = bs;
        std::string tmpFile = "tmp.ir";
        print(tmpFile);
        EmulatorRunner er;
        bool value = er.emulate(tmpFile, fileName);
        _blocks = {};
        if (value)
        {
            exit(0);
        }
    }

    _symbolTable->assignMemory();
    convertToAssembler();

}

void FourthIR::convertToAssembler()
{
    RegisterBlock registerBlock(_symbolTable);
    registerBlock.createRegisters();
    convertBlockToAssembler(_notYetConvertedBlocks.front(), registerBlock);
}

bool FourthIR::isDeterministic()
{
    for (auto& p : _notYetConvertedBlocks)
    {
        for(auto& l : p.block.lines)
        {
            if (l.operation == "READ")
            {
                return false;
            }
        }
    }
    return true;
}

void FourthIR::convertBlockToAssembler(Pair& pair, RegisterBlock& registerBlock)
{
    if (std::find(_blocks.begin(), _blocks.end(), pair.block) != _blocks.end())
    {
        return;
    }
    pair.startRegisterBlock = registerBlock;
    pair.registerBlockIsSet = true;

    Block resultBlock;
    resultBlock.blockName = pair.block.blockName;
    for(auto& l : pair.block.lines)
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
        if (l.operation == "JZERO" || l.operation == "JODD" || l.operation == "INC" || l.operation == "DEC" || l.operation == "HALF")
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
    pair.endRegisterBlock = registerBlock; 
    _blocks.push_back(resultBlock);
    continueConverting(pair, registerBlock);
}

void FourthIR::handleWrite(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& r = rb.getRegister(l.one, b, {}, true, true);
    r.variableName = l.one;

    b.lines.push_back({"PUT", r.name});     
    updateRegisterState(b, rb, r, l.one);

    if (_symbolTable->isConst(l.one) && _removeConsts)
    {
        r.variableName = "";
        r.state = RegisterState::UNKNOWN;
    }
}


void FourthIR::handleRead(RegisterBlock& rb, Block& b, Line& l)
{   
    Register& r = rb.getRegister(l.one, b, {}, true, false);
    r.variableName = l.one;
    r.needToSafe = true;

    b.lines.push_back({"GET", r.name});    
    updateRegisterState(b, rb, r, l.one);
}

void FourthIR::handleConst(RegisterBlock& rb, Block& b, Line& l)
{
    Register& r = rb.getRegister(l.one, b, {}, true, false);
    r.variableName = l.one;
    r.needToSafe = true;

    auto regs = rb.getRegisters();
    std::vector<std::pair<std::string, unsigned long long>> options = {{r.name, 0}};
    for(auto& reg : regs)
    {
        if (b.blockName != "Block_0")
        {
            break;
        }
        if (reg.state == RegisterState::CONSTVARIABLE)
        {
            options.push_back({reg.name, reg.constValue});
        }
    }

    auto lines = NumberGenerator::generateConstFrom(std::stoull(l.two), options);
    b.lines.insert(b.lines.end(), lines.begin(), lines.end()); 

    updateRegisterState(b, rb, r, l.one);
    r.constValue = std::stoull(l.two);
    if (r.state == RegisterState::VARIABLE)
    {
        r.state = RegisterState::CONSTVARIABLE;
        r.constValue = std::stoull(l.two);
    }
}

void FourthIR::handleCopy(RegisterBlock& rb, Block& b, Line& l)
{   
    Register& regTwo = rb.getRegister(l.two, b, {}, true, true);
    updateRegisterState(b, rb, regTwo, l.two);
    regTwo.variableName = l.two;

    Register& regOne = rb.getRegister(l.one, b, {regTwo}, true, false);
    updateRegisterState(b, rb, regOne, l.one);
    regOne.variableName = l.one;
    regOne.needToSafe = true;

    if (_symbolTable->isConst(l.two) && _removeConsts)
    {
        regTwo.variableName = "";
        regTwo.state = RegisterState::UNKNOWN;
    }

    b.lines.push_back({"COPY", regOne.name, regTwo.name});  

    regOne.constValue = regTwo.constValue;
}

void FourthIR::handleDirectTranslation(RegisterBlock& rb, Block& b, Line& l)
{   
    Register& regOne = rb.getRegister(l.one, b, {}, true, true);
    updateRegisterState(b, rb, regOne, l.one);
    regOne.variableName = l.one;
    regOne.needToSafe = true;

    if (l.one == l.two && l.operation == "ADD")
    {
        b.lines.push_back({l.operation, regOne.name, regOne.name});
        return;
    }

    Register& regTwo = rb.getRegister(l.two, b, {regOne}, false, true);
    updateRegisterState(b, rb, regTwo, l.two);
    regTwo.variableName = l.two;

    b.lines.push_back({l.operation, regOne.name, regTwo.name});  

    if (_symbolTable->isConst(l.two) && _removeConsts)
    {
        regTwo.variableName = "";
        regTwo.state = RegisterState::UNKNOWN;
    }
}

void FourthIR::handleSimpleOperation(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& r = rb.getRegister(l.one, b, {}, true, true);
    if (l.operation == "DEC" || l.operation == "INC" || l.operation == "HALF")
    {
        r.needToSafe = true;
    }

    b.lines.push_back({l.operation, r.name, l.two});     

    updateRegisterState(b, rb, r, l.one);
    r.variableName = l.one;
}

void FourthIR::handleMul(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getRegister(l.one, b, {}, true, true);
    registerB.variableName = l.one;
    updateRegisterState(b, rb, registerB, l.one);
    registerB.needToSafe = true;

    std::string argument = l.two;
    if (l.one == l.two)
    {
        argument = "";
    }

    Register& registerC = rb.getRegistersForOperation(argument, b, {registerB});
    registerC.state = RegisterState::UNKNOWN;
    registerC.variableName = "";

    if (l.one == l.two)
    {
        b.lines.push_back({"COPY", registerC.name, registerB.name});
    }

    Register& registerD = rb.getRegistersForOperation("TEMPORARY_2", b, {registerB, registerC});
    registerD.state = RegisterState::UNKNOWN;
    registerD.variableName = "";

    auto lines = MathOperations::generateMultiplication(registerB.name, registerC.name, registerD.name, l);
    b.lines.insert(b.lines.end(), lines.begin(), lines.end());

    registerD.variableName = l.one;
    updateRegisterState(b, rb, registerD, l.one);
    registerD.needToSafe = true;

    registerB.state = RegisterState::UNKNOWN;
    registerB.variableName = "";
    registerB.needToSafe = false;
}

void FourthIR::handleDiv(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getRegister(l.one, b, {}, true, true);
    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;
    registerB.needToSafe = true;

    Register& registerC = rb.getRegister(l.two, b, {registerB}, false, true);
    updateRegisterState(b, rb, registerC, l.two);
    registerC.variableName = l.two;

    Register& registerD = rb.getRegistersForOperation("TEMPORARY_1", b, {registerB, registerC});
    registerD.state = RegisterState::UNKNOWN;
    registerD.variableName = "";

    Register& registerE = rb.getRegistersForOperation("TEMPORARY_2", b, {registerB, registerC, registerD});
    registerE.state = RegisterState::UNKNOWN;
    registerE.variableName = "";

    Register& registerF = rb.getRegistersForOperation("TEMPORARY_3", b, {registerB, registerC, registerD, registerE});  
    registerF.state = RegisterState::UNKNOWN;
    registerF.variableName = "";    

    auto lines = MathOperations::generateDivision(
        registerB.name, registerC.name, 
        registerD.name, registerE.name,
        registerF.name, l);

    b.lines.insert(b.lines.end(), lines.begin(), lines.end());

    updateRegisterState(b, rb, registerD, l.one);
    registerD.variableName = l.one;
    registerD.needToSafe = true;

    registerB.state = RegisterState::UNKNOWN;
    registerB.variableName = "";
    registerB.needToSafe = false;
}

void FourthIR::handleMod(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getRegister(l.one, b, {}, true, true);
    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;
    registerB.needToSafe = true;

    if (_symbolTable->isConst(l.two) && _symbolTable->getConstValue(l.two) == "2")
    {
        auto lines = MathOperations::generateModuloTwo(registerB.name, l);
        b.insert(lines);
        return;
    }

    Register& registerC = rb.getRegister(l.two, b, {registerB}, false, true);
    updateRegisterState(b, rb, registerC, l.two);
    registerC.variableName = l.two;

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
}















bool FourthIR::isThisVariableUsed(std::string name, Block& block)
{
    Block& nextBlock = getPairByName(block.blockJump).block;
    resetBlocks();
    restPairBlocks();
    bool result = recursiveUsageTest(name , nextBlock);
    return result;
}

bool FourthIR::recursiveUsageTest(std::string name, Block& b)
{
    if (b.visited)
    {
        return true;
    }
    b.visited = true;

    for(auto& l : b.lines)
    {
        if(l.two == name)
        {
            return false;
        }
        if (l.two.find("(" + name + ")") != std::string::npos)
        {
            return false;
        }
        if (l.one.find("(" + name + ")") != std::string::npos)
        {
            return false;
        }  
        if (l.one == name)
        {
            if (l.operation == "COPY")
            {
                return true;
            }
            if (l.operation == "CONST")
            {
                return true;
            }
            if (l.operation == "READ")
            {
                return true;
            }
            return false;
        }
    }
    if (!b.blockJump.empty())
    {
        Block& nextBlock = getPairByName(b.blockJump).block;
        return recursiveUsageTest(name, nextBlock);
    }
    else if (!b.blockIfFalse.empty())
    {
        Block& blockTrue = getPairByName(b.blockIfTrue).block;
        bool r1 = recursiveUsageTest(name, blockTrue);  
        Block& blockFalse = getPairByName(b.blockIfFalse).block;
        bool r2 = recursiveUsageTest(name, blockFalse);  
        return r1 && r2;
    }
    return true;
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

void FourthIR::convertNextBlock(Pair& pair, RegisterBlock& rb, std::string name)
{
    Pair& next = getPairByName(name);
    if (!next.registerBlockIsSet)
    {
        RegisterBlock copy(rb);
        convertBlockToAssembler(next, copy);   
        return;         
    }
    alignRegisters(pair, next, rb, name);
}

void FourthIR::convertSplitBlock(Pair& pair, RegisterBlock& rb, std::string name)
{
    Pair& next = getPairByName(name);
    if (!next.registerBlockIsSet)
    {
        RegisterBlock copy(rb);
        convertBlockToAssembler(next, copy);   
        return;         
    }
    alignRegisters(pair, next, rb, name);
}

void FourthIR::alignRegisters(Pair& pair, Pair& next, RegisterBlock& rb, std::string name)
{
    Block handle = generateBlock();
    RegisterBlock copy(pair.endRegisterBlock);
    handle.blockJump = next.block.blockName;
    copy.exitBlock(handle, next.startRegisterBlock, *this);

    Block& grr = getBlockByName(pair.block.blockName, _blocks);

    for(auto& l : grr.lines)
    {
        if (l.two == "#" + name)
        {
            l.two = "#" + handle.blockName;
        }
        if (l.operation.find(name) != std::string::npos)
        {
            l.operation.replace(l.operation.find(name), name.length(), handle.blockName);
        }        
    }
    handle.lines.push_back({"JUMP", "", "#" +  next.block.blockName});
    _blocks.push_back(handle);
            
    convertBlockToAssembler(next, copy);
}

void FourthIR::continueConverting(Pair& pair, RegisterBlock rb)
{
    _removeConsts = true;
     
    if (!pair.block.blockJump.empty())
    {
        convertNextBlock(pair, rb, pair.block.blockJump);
    }
    else if (!pair.block.blockIfFalse.empty() && !pair.block.blockIfTrue.empty())
    {
        convertSplitBlock(pair, rb, pair.block.blockIfFalse);
        convertSplitBlock(pair, rb, pair.block.blockIfTrue);
    }

}