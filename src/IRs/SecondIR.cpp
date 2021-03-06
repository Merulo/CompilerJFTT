#include "SecondIR.hpp"


void SecondIR::parse(std::vector<Block> b)
{
    addSimpleJumps(b);
    removeEmptyBlocks();
    optimize();
}

void SecondIR::optimize()
{
    removeDivAndMod();
    removeUnusedIterators();
    removeAddSubSmallConst();
    searchForMulAndDiv();
    removeUnusedCalculations();
}

void SecondIR::addSimpleJumps(std::vector<Block> b)
{
    for(size_t i = 0; i < b.size() - 1; i++)
    {
        if (b[i].blockIfFalse.empty() && b[i].blockIfTrue.empty() && b[i].blockJump.empty())
        {
            b[i].blockJump = b[i + 1].blockName;
        }
        _blocks.push_back(b[i]);
    }
    _blocks.push_back(b.back());
    
}

void SecondIR::removeEmptyBlocks()
{
    for(auto& b : _blocks)
    {
        if (b.lines.size() == 0)
        {
            std::string jumpBlockName = b.blockJump;
            for(auto& bb : _blocks)
            {
                if (bb.blockJump == b.blockName)
                {
                    bb.blockJump = jumpBlockName;
                    continue;
                }
                if (bb.blockIfTrue == b.blockName)
                {
                    bb.blockIfTrue = jumpBlockName;
                    continue;
                }
                if (bb.blockIfFalse == b.blockName)
                {
                    bb.blockIfFalse = jumpBlockName;
                    continue;
                }
            }
        }
    }
    for(auto it = _blocks.begin(); it != _blocks.end(); it++)
    {
        if (it->lines.size() == 0)
        {
            it = _blocks.erase(it);
            it--;
        }
    }
}














void SecondIR::removeUnusedIterators()
{
    for(auto& b : _blocks)
    {
        if (b.blockName.find("_initialForControl") != std::string::npos)
        {
            auto lastLine = b.lines.back();
            std::string iteratorName = lastLine.one;
            iteratorName = iteratorName.substr(0, iteratorName.find(_symbolTable->getForControl()));

            Block& nextBlock = getBlockByName(b.blockIfFalse, _blocks);
            std::string name = b.blockName;
            name = name.substr(0, name.find_last_of("_"));
            name = name + "_exitFor";
            Block& endingBlock = getBlockByName(name, _blocks);
            resetBlocks();
            bool result = isUsedInEndingBlock(iteratorName, nextBlock, endingBlock);
            
            if (!result)
            {
                continue;
            }
            stripVariable(iteratorName, endingBlock);
            stripForIterator(iteratorName, b);
        }
    }
}

void SecondIR::stripVariable(std::string variable, Block& b)
{
    for(auto i = b.lines.begin(); i != b.lines.end(); i++)
    {
        if (i->one == variable || i->two == variable)
        {
            i = b.lines.erase(i);
            i--;
        }
    }
}

void SecondIR::stripForIterator(std::string variable, Block& b)
{
    auto firstLine = b.lines.front();
    auto lineToChange = b.lines.end();
    lineToChange--;
    lineToChange--;

    if (firstLine.operation == "CONST")
    {
        auto nextLine = b.lines.begin() + 1;
        if (nextLine->operation == "COPY")
        {
            nextLine->operation = "CONST";
            nextLine->two = firstLine.two;
        }
        auto prevprev = b.lines.end() - 2;
        if (prevprev->operation == "SUB" && prevprev->two == firstLine.one)
        {
            std::string var = getVariable(firstLine.two);
            Line line = {"CONST", var, firstLine.two};
            prevprev = b.lines.insert(prevprev, line);
            prevprev++;
            prevprev->two = var;
        }
        b.lines.erase(b.lines.begin());
    }
    else
    {
        lineToChange->two = firstLine.two;
        b.lines.erase(b.lines.begin());
    }
}

bool SecondIR::isUsedInEndingBlock(std::string variable, Block& startingBlock, Block& endingBlock)
{
    if (startingBlock.visited)
    {
        return true;
    }
    if (startingBlock == endingBlock)
    {
        return true;
    }

    startingBlock.visited = true;

    for(auto& l : startingBlock.lines)
    {
        if (l.one == variable || l.two == variable)
        {
            return false;
        }
        std::string index = "(" + variable + ")";
        if (l.one.find(index) != std::string::npos || l.two.find(index) != std::string::npos)
        {
            return false;
        }
    }

    if (!startingBlock.blockJump.empty())
    {
        Block& next = getBlockByName(startingBlock.blockJump, _blocks);
        return isUsedInEndingBlock(variable, next, endingBlock);
    }

    Block& nextTrue = getBlockByName(startingBlock.blockIfTrue, _blocks);
    Block& nextFalse = getBlockByName(startingBlock.blockIfFalse, _blocks);

    return isUsedInEndingBlock(variable, nextTrue, endingBlock) 
        && isUsedInEndingBlock(variable, nextFalse, endingBlock);
}


void SecondIR::removeAddSubSmallConst()
{
    for(auto& b : _blocks)
    {
        for(auto line = b.lines.begin(); line != b.lines.end(); line++)
        {
            checkIfThisCanBeRemoved(b.lines, line);
        }
    }
}


void SecondIR::checkIfThisCanBeRemoved(std::vector<Line>& lines, std::vector<Line>::iterator& iterator)
{
    if (iterator->operation != "ADD" && iterator->operation != "SUB")
    {
        return;
    }
    std::string operation = iterator->operation;
    std::string arg = iterator->one;
    if (!_symbolTable->isConst(iterator->two))
    {
        return;
    }
    auto constString = _symbolTable->getConstValue(iterator->two);
    if (constString == "NAN")
    {
        return;
    }
    unsigned long long value = std::stoull(constString);
    if (value > 15)
    {
        return;
    }
    if ((iterator-1)->operation == "CONST")
    {
        iterator = lines.erase(iterator - 1);
    }
    else
    {
        iterator = lines.erase(iterator - 2);
        iterator++;
    }

    iterator = lines.erase(iterator);
    iterator--;

    Line l;
    l.operation = "DEC";
    if (operation == "ADD")
    {
        l.operation = "INC";
    }
    l.one = arg;
    for(unsigned long long i = 0; i < value; i++)
    {
        iterator++;
        iterator = lines.insert(iterator, l);
    }
}

void SecondIR::searchForMulAndDiv()
{
    for(auto& b : _blocks)
    {
        for(size_t i = 0; i < b.lines.size(); i++)
        {
            if (b.lines[i].operation == "MUL")
            {
                checkPowerOfTwoMul(b.lines[i], b, i);
            }
            if (b.lines[i].operation == "DIV")
            {
                checkPowerOfTwoDiv(b.lines[i], b, i);
            }       
            if (b.lines[i].operation == "MOD")
            {
                checkTwoMod(b.lines[i], b, i);
            }     
        }
    }
}

void SecondIR::checkPowerOfTwoMul(Line& l, Block& b, size_t i)
{
    if (!_symbolTable->isConst(l.two))
    {
        return;
    }
    std::string value = _symbolTable->getConstValue(l.two);
    if (value == "0")
    {
        l.operation = "CONST";
        std::string arg = l.two;
        l.two = "0";
        stripVariable(arg, b);
        return;
    }
    unsigned long long number = std::stoull(value);
    if ((number & (number - 1)) != 0)
    {
        return;
    }

    Line addLine;
    addLine.operation = "ADD";
    addLine.one = l.one;
    addLine.two = l.one;

    stripVariable(l.two, b);

    while (number != 1)
    {
        b.lines.insert(b.lines.begin() + i - 1, addLine);
        number = number / 2;
    }
}

void SecondIR::checkPowerOfTwoDiv(Line& l, Block& b, size_t i)
{
    if (!_symbolTable->isConst(l.two))
    {
        return;
    }
    std::string value = _symbolTable->getConstValue(l.two);
    if (value == "0")
    {
        l.operation = "CONST";
        std::string arg = l.two;
        l.two = "0";
        stripVariable(arg, b);
        return;
    }
    if (value == "NAN")
    {
        return;
    }
    unsigned long long number = std::stoull(value);
    if ((number & (number - 1)) != 0)
    {
        return;
    }

    Line addLine;
    addLine.operation = "HALF";
    addLine.one = l.one;

    stripVariable(l.two, b);

    while (number != 1)
    {
        b.lines.insert(b.lines.begin() + i - 1, addLine);
        number = number / 2;
    }
}

void SecondIR::checkTwoMod(Line& l, Block& b, size_t i)
{
    if (!_symbolTable->isConst(l.two))
    {
        return;
    }
    std::string value = _symbolTable->getConstValue(l.two);
    if (value == "0")
    {
        l.operation = "CONST";
        std::string arg = l.two;
        l.two = "0";
        stripVariable(arg, b);
        return;
    }
    if (value != "2")
    {
        return;
    }
    Line copy = l;
    stripVariable(l.two, b);
    b.lines.insert(b.lines.begin() + i - 1, copy);
}

void SecondIR::removeUnusedCalculations()
{
    for(auto& b : _blocks)
    {
        for(auto line = b.lines.begin(); line != b.lines.end(); line++)
        {
            if (_symbolTable->isItVariable(line->one))
            {
                checkOperations(line, b);
            }
        }
    }


}

void SecondIR::checkOperations(std::vector<Line>::iterator& line, Block& b)
{
    if (line == b.lines.end())
    {
        return;
    }
    if (_banned.find(line->operation) != _banned.end())
    {
        return;
    } 
    resetBlocks();
    if (canRemove(line, b, line->one))
    {
        if (line->operation != "READ")
        {
            line = b.lines.erase(line);
            line--;
        }
        else
        {
            line->one = _symbolTable->getExtraVariable();
        }
    }



}

bool SecondIR::canRemove(std::vector<Line>::iterator line, Block& b, std::string name)
{
    line++;
    for (; line != b.lines.end(); line++)
    {
        if(line->two == name)
        {
            return false;
        }
        if (line->two.find("(" + name + ")") != std::string::npos)
        {
            return false;
        }
        if (line->one.find("(" + name + ")") != std::string::npos)
        {
            return false;
        }  
        if (line->one == name)
        {
            if (_banned.find(line->operation) != _banned.end())
            {
                return false;
            } 
            return true;
        }       
    }
    if (!b.blockJump.empty())
    {
        Block& nextBlock = getBlockByName(b.blockJump, _blocks);
        return canRemoveRecursive(nextBlock, name);
    }
    else if (!b.blockIfFalse.empty())
    {
        Block& blockTrue = getBlockByName(b.blockIfFalse, _blocks);
        bool r1 = canRemoveRecursive(blockTrue, name);  
        Block& blockFalse = getBlockByName(b.blockIfTrue, _blocks);
        bool r2 = canRemoveRecursive(blockFalse, name);  
        return r1 && r2;
    }
    return true;
}
bool SecondIR::canRemoveRecursive(Block& b, std::string name)
{
    if (b.visited)
    {
        return true;
    }
    b.visited = true;
    for (auto line = b.lines.begin(); line != b.lines.end(); line++)
    {
        if(line->two == name)
        {
            return false;
        }
        if (line->two.find("(" + name + ")") != std::string::npos)
        {
            return false;
        }
        if (line->one.find("(" + name + ")") != std::string::npos)
        {
            return false;
        }  
        if (line->one == name)
        {
            if (_banned.find(line->operation) != _banned.end())
            {
                return false;
            } 
            return true;
        }       
    }
    if (!b.blockJump.empty())
    {
        Block& nextBlock = getBlockByName(b.blockJump, _blocks);
        return canRemoveRecursive(nextBlock, name);
    }
    else if (!b.blockIfFalse.empty())
    {
        Block& blockTrue = getBlockByName(b.blockIfFalse, _blocks);
        bool r1 = canRemoveRecursive(blockTrue, name);  
        Block& blockFalse = getBlockByName(b.blockIfTrue, _blocks);
        bool r2 = canRemoveRecursive(blockFalse, name);  
        return r1 && r2;
    }
    return true;
}

void SecondIR::removeDivAndMod()
{
    for(auto& b : _blocks)
    {
        for(auto l = b.lines.begin(); l != b.lines.end(); l++)
        {
            if (l->operation == "DIV" || l->operation == "MOD")
            {
                std::string arg1 = l->one;
                std::string arg2 = l->two;
                std::string operation = l->operation;
                auto prev = l - 1;
                if (prev->operation == "COPY" && prev->one == arg1 && prev->two == arg1)
                {
                    auto prevprev = prev - 1;
                    if (prevprev == b.lines.begin())
                    {
                        continue;
                    }
                    if (prevprev->operation == "COPY" && prevprev->one == arg2 && prevprev->two == arg1)
                    {
                        l = b.lines.erase(l);
                        l--;
                        l = b.lines.erase(l);
                        l--;
                        l = b.lines.erase(l);
                        Line line{"CONST", arg1, "1"};
                        if (operation == "MOD")
                        {
                            line.two = "0";
                        }
                        b.lines.insert(l, line);
                    }
                }
            }
        }
    }
}