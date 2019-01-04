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
    Block b = generateBlock();
    _notYetConvertedBlocks.push_back(b);
    convertBlockToAssembler(_notYetConvertedBlocks.front(), registerBlock, _notYetConvertedBlocks.back());
}

Block& FourthIR::convertBlockToAssembler(Block& block, RegisterBlock& registerBlock, Block& lastBlock)
{
    std::cout<<"converting "<<block.blockName<<" with "<<lastBlock.blockName<<" as last"<<std::endl;
    if (block == lastBlock)
    {
        std::cout<<"TEST"<<std::endl;
        std::cout<<block<<std::endl;
        return block;
    }
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
    }
    _blocks.push_back(resultBlock);

    return continueConverting(block, registerBlock, lastBlock);
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

void FourthIR::handleSimpleOperation(RegisterBlock& rb, Block& b, Line& l)
{    
    Register& r = rb.getUniqueRegisterForVariable(l.one, b, {});
    prepareRegisterWithLoading(rb, r, b, l.one);

    b.lines.push_back({l.operation, r.name, l.two});     

    updateRegisterStateWithConst(b, rb, r, l.one);
    b.lines.push_back({"#end of performing simple operation"});    
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
    prepareRegisterWithoutLoading(rb, r, b, name);
    b.lines.push_back({"#loading " + name + " from memory"}); 

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
            b.lines.push_back({"#USELESS OPERATION WAS PERFORMED, PLEASE FIX"});       
            std::string value = _symbolTable->getConstValue(name);
            auto lines = NumberGenerator::generateConstFrom(std::stoull(value), {{r.name, 0}});
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());     
        }
    }
    r.variableName = name;
    b.lines.push_back({"#end of loading " + name + " from memory"});       
}


std::vector<Line> FourthIR::prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, std::string name)
{
    b.lines.push_back({"#storing " + r.variableName + " to memory"});    
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
    b.lines.push_back({"#end of storing " + r.variableName + " to memory"});  
    return {};  
}
























Block FourthIR::getMeetingBlock(Block&b)
{
    //find the meeting block
    std::vector<Block> ls;
    std::vector<Block> rs;
    std::vector<std::string> lv = {b.blockName};
    std::vector<std::string> rv = {b.blockName};
    traverse(getBlockByName(b.blockIfTrue, _notYetConvertedBlocks), ls, lv);
    traverse(getBlockByName(b.blockIfFalse, _notYetConvertedBlocks), rs, rv);
    
    for(auto& l : ls)
    {
        for(auto& r : rs)
        {
            if (l == r)
            {
                std::cout<<"meeting block is "<<r.blockName<<std::endl;
                return r;
            }
        }
    }
    std::cout<<"PROBLEM1 in traverse, no matching found"<<std::endl;
    if (lv.front() == lv.back())
    {
        std::cout<<"loop starting in "<<lv.front()<< " via "<<lv[1]<<std::endl;
    }
    if (rv.front() == rv.back())
    {
        std::cout<<"loop starting in "<<rv.front()<< " via "<<rv[1]<<std::endl;
    }

    return b;
}

void FourthIR::traverse(Block& b, std::vector<Block>& blocks, std::vector<std::string>& visited)
{
    std::cout<<"\tADDING "<<b.blockName<<std::endl;
    if (visited.front() == b.blockName)
    {
        blocks.push_back(b);
        visited.push_back(b.blockName);
        return;
    }
    
    visited.push_back(b.blockName);

    blocks.push_back(b);
    if (!b.blockJump.empty())
    {
        traverse(getBlockByName(b.blockJump, _notYetConvertedBlocks), blocks, visited);
        return;
    }
    else if (!b.blockIfFalse.empty() && !b.blockIfTrue.empty())
    {
        //find the meeting block
        std::vector<Block> ls;
        std::vector<Block> rs;
        traverse(getBlockByName(b.blockIfTrue, _notYetConvertedBlocks), ls, visited);
        traverse(getBlockByName(b.blockIfFalse, _notYetConvertedBlocks), rs, visited);
        
        for(auto& l : ls)
        {
            for(auto& r : rs)
            {
                if (l == r)
                {
                    blocks.push_back(r);
                    traverse(r, blocks, visited);
                    return;
                }
            }
        }
        std::cout<<"PROBLEM2 in traverse, no matching found"<<std::endl; 
    }
}

void FourthIR::appendSaveOfVariable(Block& target, Block& meeting, RegisterBlock& copy, Register& reg, Block& last)
{
    if (target != meeting && meeting != last)
    {
        auto lines = prepareRegisterWithoutLoading(copy, reg, last, "");
        Block& toInsertT = getBlockByName(last.blockName, _blocks);
        lines.insert(lines.begin(), {"#saving register" + reg.name + " content"});
        lines.push_back({"#end of saving register" + reg.name + " content"});
        toInsertT.lines.insert(toInsertT.lines.end() - 1, lines.begin(), lines.end());
    }    
}

void FourthIR::mergeRegisters(
    std::vector<Register>& regT, std::vector<Register>& regF, 
    Block& t, Block& f, 
    RegisterBlock& copyForT, RegisterBlock& copyForF,
    Block& lastT, Block& lastF,
    Block& meeting, RegisterBlock& rb)
{
    for(size_t i = 0; i < regT.size(); i++)
    {
        // std::cout<<"comparing "<<regT[i] <<" and "<<regF[i]<<std::endl;
        if (regT[i].shouldSave(regF[i]))
        {
            // std::cout<<"should save"<<std::endl;
            appendSaveOfVariable(t, meeting, copyForT, regT[i], lastT);
            appendSaveOfVariable(f, meeting, copyForF, regF[i], lastF);
            rb.setUnkown(i);
        }
        else
        {
            rb.setRegister(i, regF[i]);
        }
    }
}

Block& FourthIR::handleSplit(Block& b, RegisterBlock rb, Block& lastBlock)
{
    std::cout<<"meeting block= "<<std::endl;
    Block meeting = getMeetingBlock(b);
    if (meeting == b)
    {
        std::cout<<"MEETING BLOCK WAS NOT FOUND! for "<<b.blockName<<std::endl;
        return b;
    }

    std::cout<<"meeting block= "<<meeting.blockName<<std::endl;
    Block& t = getBlockByName(b.blockIfTrue, _notYetConvertedBlocks);
    Block& f = getBlockByName(b.blockIfFalse, _notYetConvertedBlocks);

    RegisterBlock copyForT(rb);
    RegisterBlock copyForF(rb);
    Block lastT = meeting;
    Block lastF = meeting;

    if (t != meeting)
    {
        lastT = convertBlockToAssembler(t, copyForT, meeting);
    }
    if (f != meeting)
    {
        lastF = convertBlockToAssembler(f, copyForF, meeting);
    }

    std::cout<<"merge of registers"<<std::endl;

    // copyForT.print();
    // copyForF.print();
    auto regT = copyForT.getRegisters();
    auto regF = copyForF.getRegisters();

    mergeRegisters(regT, regF, t, f, copyForT, copyForF, lastT, lastF, meeting, rb);

    rb.setAddressRegisterAsUnkown();
    return convertBlockToAssembler(meeting, rb, lastBlock);
}

Block& FourthIR::continueConverting(Block& b, RegisterBlock rb, Block& lastBlock)
{
    if (!b.blockJump.empty())
    {
        Block next = getBlockByName(b.blockJump, _notYetConvertedBlocks);
        if (next == lastBlock)
        {
            return b;
        }
        return convertBlockToAssembler(next, rb, lastBlock);
    }
    else if (!b.blockIfFalse.empty() && !b.blockIfTrue.empty())
    {
        return handleSplit(b, rb, lastBlock);
    }
    return b;
}