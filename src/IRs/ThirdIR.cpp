#include "ThirdIR.hpp"

void ThirdIR::parse(std::vector<Block> b)
{
    _blocks = b;
    legalize();
}

void ThirdIR::legalize()
{
    removeCopyWithSameArguments();
    searchForTwoTablesWithSameOperation();
    legalizeSpecificJumps();
    legalizeJumps();
}

void ThirdIR::legalizeSpecificJumps()
{
    for(auto& b : _blocks)
    {
        for(auto i = b.lines.begin(); i != b.lines.end(); i++)
        {
            if (i->operation == "JZERO")
            {
                i->two = "#" + b.blockIfTrue;
                b.lines.push_back({"JUMP", "", "#" + b.blockIfFalse});
                break;
            }
        }
    }    
}

void ThirdIR::removeCopyWithSameArguments()
{
    for(auto& b : _blocks)
    {
        for(auto i = b.lines.begin(); i != b.lines.end(); i++)
        {
            if (i->operation == "COPY" && i->one == i->two)
            {
                i = b.lines.erase(i);
                i--;
            }
        }
    }
}

bool ThirdIR::checkVariablesAreSameTable(std::string one, std::string two)
{
    if (one.find("(") == std::string::npos)
    {
        return false;
    }
    if (two.find("(") == std::string::npos)
    {
        return false;
    }
    std::string oneArray = one.substr(0, one.find("("));
    std::string twoArray = two.substr(0, two.find("("));
    return oneArray == twoArray;
}

void ThirdIR::searchForTwoTablesWithSameOperation()
{
    for(auto& b : _blocks)
    {
        for(auto i = b.lines.begin(); i != b.lines.end(); i++)
        {
            if (checkVariablesAreSameTable(i->one, i->two))
            {
                std::string var = getVariable("NAN");
                i = b.lines.insert(i, {"COPY", var, i->two});
                i++;
                i->two = var;
            }
        }
    }
}

void ThirdIR::legalizeJumps()
{
    for (size_t i = 0; i < _blocks.size(); i++)
    {
        if (!_blocks[i].blockJump.empty())
        {
            Line l;
            l.operation = "JUMP";
            l.two = "#" + _blocks[i].blockJump;
            _blocks[i].lines.push_back(l);
            continue;
        }
        if (_blocks[i].lines.back().operation == "JLS")
        {
            legalizeJLS(_blocks[i], false);
        }
        if(_blocks[i].lines.back().operation == "JMR")
        {
            legalizeJMR(_blocks[i], false);
        }
        if (_blocks[i].lines.back().operation == "JLE")
        {
            legalizeJLS(_blocks[i], true);
        }
        if(_blocks[i].lines.back().operation == "JME")
        {
            legalizeJMR(_blocks[i], true);
        }
        if(_blocks[i].lines.back().operation == "JEQ")
        {
            legalizeEquality(_blocks[i]);
        }
        if(_blocks[i].lines.back().operation == "JNE")
        {
            std::swap(_blocks[i].blockIfFalse, _blocks[i].blockIfTrue);
            legalizeEquality(_blocks[i]);
        }           
    }
}

std::string ThirdIR::calculateDiff(Block& b, std::string one, std::string two, bool inc)
{
    std::string var = _symbolTable->getExtraVariable();
    
    Line copyLine;
    copyLine.operation = "COPY";
    copyLine.one = var;
    copyLine.two = one;
    b.lines.push_back(copyLine);
    
    if (inc)
    {
        Line incLine;
        incLine.operation = "INC";
        incLine.one = var;
        b.lines.push_back(incLine);
    }

    Line subLine;
    subLine.operation = "SUB";
    subLine.one = var;
    subLine.two = two;
    b.lines.push_back(subLine);    

    return var;
}

void ThirdIR::insertJumps(Block&b, std::string var)
{
    Line jzero;
    jzero.operation = "JZERO";
    jzero.one = var;
    jzero.two = "#"+b.blockIfFalse;
    b.lines.push_back(jzero);

    Line jump;
    jump.operation = "JUMP";
    jump.two = "#"+b.blockIfTrue;
    b.lines.push_back(jump);
}

void ThirdIR::legalizeJLS(Block& b, bool inc)
{
    Line lastLine = b.lines.back();
    b.lines.pop_back();

    std::string var = calculateDiff(b, lastLine.two, lastLine.one, inc);
    insertJumps(b, var);
}

void ThirdIR::legalizeJMR(Block& b, bool inc)
{
    if (checkZeroJump(b) && !inc)
    {
        legalizeZeroJMR(b);
        return;
    }

    Line lastLine = b.lines.back();
    b.lines.pop_back();

    std::string var = calculateDiff(b, lastLine.one, lastLine.two, inc);
    insertJumps(b, var);
}

void ThirdIR::legalizeEquality(Block& b)
{
    if (checkZeroJump(b))
    {
        legalizeZeroJump(b);
        return;
    }
    b.lines.insert(b.lines.begin() + b.lines.size() - 1, {"#DO NOT REMOVE CONST"});
    Line lastLine = b.lines.back();
    legalizeJLS(b, true);
    b.lines.back().one = lastLine.one;
    b.lines.back().two = lastLine.two;
    legalizeJMR(b, true);
}

bool ThirdIR::checkZeroJump(Block& b)
{
    Line lastLine = b.lines.back();
    if (_symbolTable->isConst(lastLine.one) && _symbolTable->getConstValue(lastLine.one) == "0")
    {
        return true;
    }
    if (_symbolTable->isConst(lastLine.two) && _symbolTable->getConstValue(lastLine.two) == "0")
    {
        return true;
    }    
    return false;
}

void ThirdIR::legalizeZeroJump(Block& b)
{
    Line lastLine = b.lines.back();
    b.lines.clear();

    std::string argument = lastLine.one;
    if (_symbolTable->isConst(argument))
    {
        argument = lastLine.two;
    }
    Line l;
    l.operation = "JZERO";
    l.one = argument;
    l.two = "#" + b.blockIfTrue;
    b.lines.push_back(l);

    Line l2;
    l2.operation = "JUMP";
    l2.two = "#" + b.blockIfFalse;
    b.lines.push_back(l2);
}

void ThirdIR::legalizeZeroJMR(Block& b)
{
    Line lastLine = b.lines.back();
    b.lines.clear();

    std::swap(b.blockIfFalse, b.blockIfTrue);
    std::string argument = lastLine.one;
    if (_symbolTable->isConst(argument))
    {
        Line l;
        l.operation = "JUMP";
        l.two = "#" + b.blockIfTrue;
        b.lines.push_back(l);
        return;
    }
    
    Line l;
    l.operation = "JZERO";
    l.one = argument;
    l.two = "#" + b.blockIfTrue;
    b.lines.push_back(l);

    Line l2;
    l2.operation = "JUMP";
    l2.two = "#" + b.blockIfFalse;
    b.lines.push_back(l2);
}