#include "FourthIR.hpp"

#include "Calculators/MathOperations.hpp"
#include "Calculators/NumberGenerator.hpp"

void FourthIR::parse(std::vector<Block> bs)
{
    for (auto& b : bs)
    {
        Pair p;
        p.block = b;
        _notYetConvertedBlocks.push_back(p);
    }
    _symbolTable->assignMemory();
    convertToAssembler();
}

void FourthIR::convertToAssembler()
{
    RegisterBlock registerBlock(_symbolTable);
    registerBlock.createRegisters();
    Pair p = _notYetConvertedBlocks.front();
    convertBlockToAssembler(p, registerBlock);
}

void FourthIR::convertBlockToAssembler(Pair& pair, RegisterBlock& registerBlock)
{
    std::cout<<"converting "<<pair.block.blockName<<std::endl;
    // std::cout<<"converting "<<block.blockName<<" with "<<lastBlock.blockName<<" as last"<<std::endl;
    pair.startRegisterBlock = registerBlock;
    // registerBlock.print();

    Block resultBlock;
    resultBlock.blockName = pair.block.blockName;
    for(auto& l : pair.block.lines)
    {
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
    }
    _blocks.push_back(resultBlock);

    continueConverting(pair, registerBlock);
}


void FourthIR::handleConst(RegisterBlock& rb, Block& b, Line& l)
{
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithoutLoading(rb, r, b, l.one);
    r.variableName = l.one;
    
    b.lines.push_back({"\t#generating number"});
    auto lines = NumberGenerator::generateConstFrom(std::stoull(l.two), {{r.name, 0}});
    b.lines.insert(b.lines.end(), lines.begin(), lines.end()); 
    b.lines.push_back({"\t#end of generating number"});

    updateRegisterStateWithConst(b, rb, r, l.one);
    r.constValue = std::stoull(l.two);

    b.lines.push_back({"\t#end of performing const"});
}

void FourthIR::handleWrite(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithLoading(rb, r, b, l.one);

    b.lines.push_back({"PUT", r.name});     

    updateRegisterStateWithConst(b, rb, r, l.one);
    b.lines.push_back({"\t#end of performing write"});    
}


void FourthIR::handleRead(RegisterBlock& rb, Block& b, Line& l)
{   
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithoutLoading(rb, r, b, l.one);
    r.variableName = l.one;

    b.lines.push_back({"GET", r.name});   

    updateRegisterState(b, rb, r, l.one);
    b.lines.push_back({"\t#end of performing write"});    
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
    b.lines.push_back({"\t#end of performing copy"});   
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
    b.lines.push_back({"\t#end of performing copy"});   
}

void FourthIR::handleSimpleOperation(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithLoading(rb, r, b, l.one);

    b.lines.push_back({l.operation, r.name, l.two});     

    updateRegisterStateWithConst(b, rb, r, l.one);
    b.lines.push_back({"\t#end of performing simple operation"});    
}

void FourthIR::handleMul(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithLoading(rb, registerB, b, l.one);

    Register& registerC = rb.getUniqueRegisterForVariable(l.two, b, {registerB});
    if (registerC.variableName == l.two)
    {
        prepareRegisterWithoutLoading(rb, registerC, b, "");
    }
    else
    {
        prepareRegisterWithLoading(rb, registerC, b, l.two);
    }

    Register& registerD = rb.getUniqueRegisterForVariable("TEMPORARY_2", b, {registerB, registerC});
    prepareRegisterWithoutLoading(rb, registerD, b, "TEMPORARY_2");

    auto lines = MathOperations::generateMultiplication(registerB.name, registerC.name, registerD.name, l);
    b.lines.insert(b.lines.end(), lines.begin(), lines.end());

    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;
    registerC.state = RegisterState::UNKNOWN;
    registerC.variableName = "";
    registerD.state = RegisterState::UNKNOWN;
    registerD.variableName = "";

    b.lines.push_back({"\t#end of performing MUL operation"});    
}

void FourthIR::handleDiv(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithLoading(rb, registerB, b, l.one);

    Register& registerC = rb.getUniqueRegisterForVariable(l.two, b, {registerB});
    if (registerC.variableName == l.two)
    {
        prepareRegisterWithoutLoading(rb, registerC, b, "");
    }
    else
    {
        prepareRegisterWithLoading(rb, registerC, b, l.two);
    }

    Register& registerD = rb.getUniqueRegisterForVariable("TEMPORARY_1", b, {registerB, registerC});
    prepareRegisterWithoutLoading(rb, registerD, b, "TEMPORARY_1");

    Register& registerE = rb.getUniqueRegisterForVariable("TEMPORARY_2", b, {registerB, registerC, registerD});
    prepareRegisterWithoutLoading(rb, registerE, b, "TEMPORARY_2");

    Register& registerF = rb.getUniqueRegisterForVariable("TEMPORARY_3", b, {registerB, registerC, registerD, registerE});
    prepareRegisterWithoutLoading(rb, registerF, b, "TEMPORARY_3");    

    auto lines = MathOperations::generateDivision(
        registerB.name, registerC.name, 
        registerD.name, registerE.name,
        registerF.name, l);

    b.lines.insert(b.lines.end(), lines.begin(), lines.end());

    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;
    registerC.state = RegisterState::UNKNOWN;
    registerC.variableName = "";
    registerD.state = RegisterState::UNKNOWN;
    registerD.variableName = "";
    registerE.state = RegisterState::UNKNOWN;
    registerE.variableName = "";
    registerF.state = RegisterState::UNKNOWN;
    registerF.variableName = "";    

    b.lines.push_back({"\t#end of performing DIV operation"});    
}

void FourthIR::handleMod(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& registerB = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithLoading(rb, registerB, b, l.one);

    Register& registerC = rb.getUniqueRegisterForVariable(l.two, b, {registerB});
    if (registerC.variableName == l.two)
    {
        prepareRegisterWithoutLoading(rb, registerC, b, "");
    }
    else
    {
        prepareRegisterWithLoading(rb, registerC, b, l.two);
    }

    Register& registerE = rb.getUniqueRegisterForVariable("TEMPORARY_2", b, {registerB, registerC});
    prepareRegisterWithoutLoading(rb, registerE, b, "TEMPORARY_2");

    Register& registerF = rb.getUniqueRegisterForVariable("TEMPORARY_3", b, {registerB, registerC, registerE});
    prepareRegisterWithoutLoading(rb, registerF, b, "TEMPORARY_3");    

    auto lines = MathOperations::generateModulo(
        registerB.name, registerC.name, 
        registerE.name, registerF.name, l);

    b.lines.insert(b.lines.end(), lines.begin(), lines.end());

    updateRegisterState(b, rb, registerB, l.one);
    registerB.variableName = l.one;
    registerC.state = RegisterState::UNKNOWN;
    registerC.variableName = "";
    registerE.state = RegisterState::UNKNOWN;
    registerE.variableName = "";
    registerF.state = RegisterState::UNKNOWN;
    registerF.variableName = "";    

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

void FourthIR::prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, std::string name, bool save)
{
    if (save)
    {
        prepareRegisterWithoutLoading(rb, r, b, name);
    }
    b.lines.push_back({"\t\t#loading " + name + " from memory"}); 

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
        else if (_symbolTable->isConst(name))
        {
            b.lines.push_back({"\t#USELESS OPERATION WAS PERFORMED, PLEASE FIX"});       
            std::string value = _symbolTable->getConstValue(name);
            auto lines = NumberGenerator::generateConstFrom(std::stoull(value), {{r.name, 0}});
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());     
        }
    }
    r.variableName = name;
    b.lines.push_back({"\t\t#end of loading " + name + " from memory"});       
}


std::vector<Line> FourthIR::prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, std::string name)
{
    b.lines.push_back({"\t\t#storing " + r.variableName + " to memory"});    
    if (r.variableName != name)
    {
        if (r.state == RegisterState::CONSTVARIABLE || r.state == RegisterState::VARIABLE)
        {
            auto lines = rb.performMemoryOperation("STORE", r, _symbolTable->getMemoryCell(r.variableName));
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
            return lines;
        }
        else if (r.state == RegisterState::TABLE)
        {
            auto lines = rb.performTableMemoryOperation("STORE", r.variableName, r);
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
            return lines;
        }
    }
    b.lines.push_back({"\t\t#end of storing " + r.variableName + " to memory"});  
    return {};  
}




















void FourthIR::handleSinglePath(Pair&p, RegisterBlock rb)
{
    Pair& next = getBlockByName(p.block.blockJump);
    if (next.registerBlockIsSet)
    {
        std::cout<<"perform operation to fit registerBlock in blockaaaaaa "<<p.block.blockName<< " and "<<next.block.blockName<<std::endl;
        alignRegisters(p, next);
    }
    else
    {
        std::cout<<"parsing single jump "<<next.block.blockName<<std::endl;
        next.registerBlockIsSet = true;
        convertBlockToAssembler(next, rb);
    }
}

void FourthIR::handleBranchSimple(std::string name, RegisterBlock rb)
{
    // std::cout<<"parsing first jump "<<next.block.blockName<<std::endl;
    Pair& next = getBlockByName(name);
    next.registerBlockIsSet = true;
    convertBlockToAssembler(next, rb); 
}

void FourthIR::checkVariableIsInOtherRegister(Register& r, std::vector<Register>& regTargeted, Block& tmp, Pair& p)
{
    std::string variable =r.variableName;
    for(size_t j = 0; j < regTargeted.size(); j++)
    {
        if (regTargeted[j].state == RegisterState::VARIABLE || regTargeted[j].state == RegisterState::CONSTVARIABLE)
        {
            if (variable == regTargeted[j].variableName)
            {
                // std::cout<<"\t\t\tShould save "<<regToChange[i]<<std::endl;
                prepareRegisterWithoutLoading(p.registerBlock, r, tmp, "");

            }
        }
    }
}

void FourthIR::alignRegisters(Pair& p, Pair& next)
{
    auto regToChange = p.registerBlock.getRegisters();
    auto regTargeted = next.startRegisterBlock.getRegisters();
    Block tmp = generateBlock();
    tmp.blockName+= " for register values update!";

        for(size_t i = 0; i < regToChange.size(); i++)
        {
            std::cout<<"\tcomparing "<<regToChange[i] <<" and "<<regTargeted[i]<<std::endl;
            if (regToChange[i].shouldSave(regTargeted[i]))
            {
                std::cout<<"\t\tshould save "<<regToChange[i]<<std::endl;
                prepareRegisterWithoutLoading(p.registerBlock, regToChange[i], tmp, regTargeted[i].variableName);
            }
            else if (regToChange[i].state == RegisterState::VARIABLE || regToChange[i].state == RegisterState::CONSTVARIABLE)
            {
                checkVariableIsInOtherRegister(regToChange[i], regTargeted, tmp, p);
            }
        }

        for(size_t i = 0; i < regToChange.size(); i++)
        {
            std::cout<<"\tcomparing "<<regToChange[i] <<" and "<<regTargeted[i]<<std::endl;
            if (regToChange[i].shouldSave(regTargeted[i]))
            {
                std::cout<<"\t\tshould load "<<regTargeted[i]<<std::endl;
                prepareRegisterWithLoading(p.registerBlock, regToChange[i], tmp, regTargeted[i].variableName, false);
            }
        }

    tmp.lines.push_back({"JUMP", "", "#" + next.block.blockName});
    _blocks.push_back(tmp);
    
    //TODO: COPY ADDRESS REGISTER STATE
    p.registerBlock.setAddressRegisterAsUnkown();

    Block& b = IRBase::getBlockByName(p.block.blockName, _blocks);

    for(auto& line : b.lines)
    {
        if (line.two == "#" + next.block.blockName)
        {
            line.two = "#" + tmp.blockName;
        }
    }
}


void FourthIR::handleBranch(Pair& p, RegisterBlock& rb, std::string nextName)
{
    Pair& next = getBlockByName(nextName);
    if (next.registerBlockIsSet)
    {
        std::cout<<"perform operation to fit registerBlock in block "<<p.block.blockName<< " and "<<next.block.blockName<<std::endl;
        alignRegisters(p, next);
    }
    else
    {
        handleBranchSimple(nextName, rb);
    }
}

void FourthIR::continueConverting(Pair& p, RegisterBlock rb)
{
    // std::cout<<p.block.blockName<<std::endl;
    // rb.print();
    p.registerBlock = rb;

    if (!p.block.blockJump.empty())
    {
        handleSinglePath(p, rb);
    }
    else if (!p.block.blockIfFalse.empty() && !p.block.blockIfTrue.empty())
    {
        std::cout<<"handling split"<<std::endl;
        
        handleBranch(p, rb, p.block.blockIfTrue);
        handleBranch(p, rb, p.block.blockIfFalse);

    }

}