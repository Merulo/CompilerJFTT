#include "FirstIR.hpp"

void FirstIR::addNewCode(cStrRef operation, cStrRef one, cStrRef two)
{
    // std::cerr<<"OP="<<operation<<" one="<<one<<" two="<<two<<std::endl;
    Line line;
    line.operation = operation;
    line.one = one;
    line.two = two;
    _currentBlock.top().lines.push_back(line);
    std::cerr<<line<<std::endl;
    if (operation == "HALT")
    {
        _blocks.push_back(_currentBlock.top());
    }
}

void FirstIR::handleMathOperation(cStrRef resultName)
{
    std::cerr<<"Math="<<resultName<<" "<<_operation<<" "<<_firstExtraParameter<<" "<<_secondExtraParameter<<std::endl;
    
    if (_firstExtraParameter == _secondExtraParameter && _symbolTable->isItTable(_firstExtraParameter))
    {
        std::string res = _firstExtraParameter.substr(_firstExtraParameter.find("(") + 1, std::string::npos);
        res.pop_back();
        if (res == resultName)
        {
            std::string var = getVariable("NAN");
            addNewCode("COPY", var, _secondExtraParameter);
            _secondExtraParameter = var;
        }
    }
    if (_operation.empty())
    {
        addNewCode("CONST", resultName, _firstExtraParameter);
    }
    else if (_operation == "COPY")
    {
        addNewCode("COPY", resultName, _firstExtraParameter);      
    }
    else if (_secondExtraParameter == resultName)
    {
        if (_operation == "ADD" || _operation == "MUL")
        {
            addNewCode(_operation, resultName, _firstExtraParameter);
        }
        else if (_operation == "SUB" || _operation == "DIV" || _operation == "MOD")
        {
            handleNonCommutativeOperation(resultName, _operation);
        }
    }
    else
    {
        if (_firstExtraParameter == resultName)
        {
            addNewCode(_operation, resultName, _secondExtraParameter);
        }
        else
        {
            addNewCode("COPY", resultName, _firstExtraParameter);
            addNewCode(_operation, resultName, _secondExtraParameter);
        }  
    }
    reset();
}

void FirstIR::handleConditionOperation(cStrRef operation, cStrRef one, cStrRef two)
{
    _conditionBlocks.push(_currentBlock.top());
    addNewCode(operation, one, two);
    _blocks.push_back(_currentBlock.top());
    _currentBlock.pop();

    //creates blockIfFalse
    Block b2 = generateBlock();
    _currentBlock.push(b2);
    _blocks.back().blockIfFalse = _currentBlock.top().blockName;

    //creates blockIfTrue
    Block b3 = generateBlock();
    _currentBlock.push(b3);
    _blocks.back().blockIfTrue = _currentBlock.top().blockName;    

}

void FirstIR::setOperation(cStrRef operation)
{
    _operation = operation;
}
void FirstIR::setFirstExtraParameter(cStrRef first)
{
    _firstExtraParameter = first;
}
void FirstIR::setSecondExtraParameter(cStrRef second)
{
    _secondExtraParameter = second;
}

void FirstIR::endElse()
{
    Block s3 = _currentBlock.top();
    _currentBlock.pop();
    Block s2 = _currentBlock.top();
    _currentBlock.pop();
    Block b1 = generateBlock();
    _currentBlock.push(b1);
    Block s1 = _currentBlock.top();
    s3.blockJump = s1.blockName;
    _blocks.push_back(s3);
    _currentBlock.push(s2);
}

void FirstIR::endIf()
{
    Block endOfTrueBlock = _currentBlock.top();
    _currentBlock.pop();
    endOfTrueBlock.blockJump = _currentBlock.top().blockName;
    _blocks.push_back(endOfTrueBlock);
    _conditionBlocks.pop();
}

void FirstIR::closeConditionBlock()
{
    //closes current block
    _blocks.push_back(_currentBlock.top());
    _currentBlock.pop();
    Block b1 = generateBlock();
    _currentBlock.push(b1);
    _conditionBlocks.push(b1);
}

void FirstIR::endWhileDo()
{
    Block endOfTrueBlock = _currentBlock.top();
    _currentBlock.pop();
    std::cerr<<"Setting "<<endOfTrueBlock.blockName<<" to "<<_conditionBlocks.top().blockName<<std::endl;
    _conditionBlocks.pop();
    endOfTrueBlock.blockJump = _conditionBlocks.top().blockName;
    _conditionBlocks.pop();
    _blocks.push_back(endOfTrueBlock);  
}

void FirstIR::endDoWhile()
{
    Block endOfTrueBlock = _currentBlock.top();
    _currentBlock.pop();
    _conditionBlocks.pop();
    _conditionBlocks.pop();
    endOfTrueBlock.blockJump = _conditionBlocks.top().blockName;
    _conditionBlocks.pop();
    _blocks.push_back(endOfTrueBlock);       
}

void FirstIR::closeForBlock()
{
    //closes current block
    _blocks.push_back(_currentBlock.top());
    _currentBlock.pop();
    Block b1 = generateBlock();
    _currentBlock.push(b1);
    _forBlocks.push(b1);       
}

void FirstIR::insertFor(std::string iterator, Data from, Data to, bool isForTo)
{
    closeForBlock();
    Block b1 = _forBlocks.top();
    _forBlocks.pop();
    Block b2 = _forBlocks.top();
    _forBlocks.pop();

    Block firstControlBlock = createBeforeForBlock(iterator, from, to, isForTo);
    firstControlBlock.blockIfFalse = b2.blockName;
    firstControlBlock.blockIfTrue = b1.blockName;

    Block secondControlBlock = createSecondControlBlock(iterator, isForTo);
    secondControlBlock.blockIfFalse = b2.blockName;
    secondControlBlock.blockIfTrue = b1.blockName;        
    
    for(size_t i = 0; i < _blocks.size(); i++)
    {
        if (_blocks[i] == b2)
        {
            _blocks.insert(_blocks.begin() + i, firstControlBlock);
            break;
        }
    }

    _blocks.push_back(_currentBlock.top());
    _currentBlock.pop();
    Block testing = generateBlock();
    _currentBlock.push(testing);

    for(size_t i = 0; i < _blocks.size(); i++)
    {
        if (_blocks[i] == b1)
        {
            _blocks.insert(_blocks.begin() + i, secondControlBlock);
            return;
        }
    }        

}

Block FirstIR::createBeforeForBlock(std::string iterator, Data from, Data to, bool isForTo)
{
    Block b = generateBlock();
    Line iteratorInit = getLine(from);
    iteratorInit.one = iterator;

    Line counterInit = getLine(to);
    counterInit.one = iterator + forControlName;

    Line counterFinal = getLine(from);
    counterFinal.operation = "SUB";
    counterFinal.one = counterInit.one;
    counterFinal.two = iterator;

    Line inclusiveFor;
    inclusiveFor.operation = "INC";
    inclusiveFor.one = counterInit.one;

    Line l;
    if (!isForTo)
    {
        std::swap(counterInit.two, counterFinal.two);
        if (counterInit.operation == "CONST")
        {
            counterInit.operation = "COPY";
            
            std::string var = getVariable(counterFinal.two);
            l.operation = "CONST";
            l.one = var;
            l.two = counterFinal.two;
            counterFinal.two = var;
        }
    }

    b.lines.push_back(iteratorInit);
    b.lines.push_back(counterInit);
    b.lines.push_back(inclusiveFor);

    if (!isForTo && !l.operation.empty())
    {
        b.lines.push_back(l);
    }
    
    b.lines.push_back(counterFinal);
    
    Line writeLine;
    writeLine.operation = "WRITE";
    writeLine.one = iterator + forControlName; 
    // b.lines.push_back(writeLine);
    Line jump;
    jump.operation = "JZERO";
    jump.one = counterInit.one;
    b.lines.push_back(jump);
    return b;
}

Block FirstIR::createSecondControlBlock(std::string iterator, bool isForTo)
{
    Block b = generateBlock();
    Line incLine;
    incLine.operation = "INC";
    incLine.one = iterator;

    Line decLine;
    decLine.operation = "DEC";
    decLine.one = iterator + forControlName;

    Line writeLine;
    writeLine.operation = "WRITE";
    writeLine.one = iterator + forControlName;    

    Line line;
    line.operation = "JZERO";
    line.one = iterator + forControlName;

    if (!isForTo)
    {
        incLine.operation = "DEC";
    }

    b.lines.push_back(incLine);
    b.lines.push_back(decLine);
    // b.lines.push_back(writeLine);
    b.lines.push_back(line);    

    return b;
}

Line FirstIR::getLine(Data d)
{
    Line l;
    if (!d.name.empty())
    {
        l.operation = "COPY";
        l.two = d.name;
    }
    else
    {
        l.operation = "CONST";
        l.two = std::to_string(d.value);
    }
    return l;
}

/*
PRIVATE
*/
void FirstIR::reset()
{
    _operation = "";
    _firstExtraParameter = "";
    _secondExtraParameter = "";
}

void FirstIR::handleNonCommutativeOperation(cStrRef resultName, cStrRef operation)
{
    std::string reg = getVariable("NAN");
    addNewCode("COPY", reg, resultName);
    addNewCode("COPY", resultName, _firstExtraParameter);
    addNewCode(operation, resultName, reg);
}