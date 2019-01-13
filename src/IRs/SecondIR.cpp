#include "SecondIR.hpp"

void SecondIR::parse(std::vector<Block> b)
{
    addSimpleJumps(b);
    removeEmptyBlocks();
    optimize();
}

void SecondIR::optimize()
{
    removeUnusedIterators();


    //TODO: constant propagation
    //TODO: loop unrolling
    //TODO: ADD X 1 -> inc etc
    //TODO: removed unused values
    //TODO: reorder of constant generation inside blocks
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
            std::cout<<b.blockName<<" is for control block"<<std::endl; 
            auto lastLine = b.lines.back();
            std::string iteratorName = lastLine.one;
            iteratorName = iteratorName.substr(0, iteratorName.find(_symbolTable->getForControl()));
            std::cout<<"TEST="<<iteratorName<<std::endl;

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
        std::string var = getVariable(firstLine.two);
        Line newLine = {"CONST", var, firstLine.two};
        lineToChange = b.lines.insert(lineToChange, newLine);
        lineToChange++;
        lineToChange->two = var;
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