#include "FifthIR.hpp"

#include <algorithm>

#include "Calculators/MathOperations.hpp"

void FifthIR::parse(std::vector<Block> b)
{
    _blocks = b;
    removeJumps();   
    calculateJumps();
}

void FifthIR::calculateJumps()
{
    unsigned long long instructionCounter = 0;
    for(auto& b : _blocks)
    {
        for(auto& line : b.lines)
        {
            if (line.operation.find("#") == std::string::npos && !line.operation.empty())
            {
                line.operationNumber = instructionCounter;
                instructionCounter++;
            }
        }
    }

    for(auto& b : _blocks)
    {
        for(auto& line : b.lines)
        {
            
            if  (line.operation == "JUMP" 
                || line.operation == "JZERO"
                || line.operation == "JODD")
            {
                std::string argument = line.two;
                if (argument.find(MathOperations::getLabelName()) == std::string::npos)
                {
                    argument = argument.substr(1, std::string::npos);
                    int jumpTarget = getFirstInstructionInBlock(getBlockByName(argument, _blocks));
                    line.two = std::to_string(jumpTarget) + " " + line.two;
                }
                else
                {
                    int jumpTarget = getFirstInstructionAfter(b, argument);
                    line.two = std::to_string(jumpTarget) + " " + line.two;
                }
            }
        }
    }
}

int FifthIR::getFirstInstructionInBlock(Block& b)
{
    for(auto& line : b.lines)
    {
        if (line.operation.find("#") == std::string::npos && !line.operation.empty())
        {
            return line.operationNumber;
        }
    }
    return -1;
}

int FifthIR::getFirstInstructionAfter(Block& b, std::string name)
{
    bool found = false;
    for(auto& line : b.lines)
    {
        if (line.operation == name)
        {
            found = true;
        }

        if (found && line.operation.find("#") == std::string::npos && !line.operation.empty())
        {
            return line.operationNumber;
        }
    }
    Block next = getNextBlock(b);
    return getFirstInstructionInBlock(next);
}

void FifthIR::removeJumps()
{
    for(auto b = _blocks.begin(); b != _blocks.end(); b++)
    {
        if (onlyComments(*b))
        {
            if ((*b).lines.back().operation == "JUMP")
            {
                std::string arg = (*b).lines.back().two;
                arg.erase(0, 1);
                Block& current = getBlockByName(arg, _blocks);
                replaceJump((*b), current);  
                b = _blocks.erase(b);
                b--;
            }
        }
    }
    for(auto it = _blocks.begin() + 1; it != _blocks.end(); it++)
    {
        Block& previous = *(it-1);
        Block& current = *it;
        if(previous.lines.back().two == "#" + current.blockName)
        {
            previous.lines.erase(previous.lines.end() - 1);
            if (onlyComments(previous))
            {
                replaceJump(previous, current);
            }
        }
    }

}

bool FifthIR::onlyComments(Block& b)
{
    for(auto& l : b.lines)
    {
        if (l.operation.find("#") == std::string::npos && l.operation != "JUMP")
        {
            return false;
        }
    }

    return true;
}

void FifthIR::replaceJump(Block& previous, Block& current)
{
    for(auto& b : _blocks)
    {
        for(auto& l : b.lines)
        {
            replace(l.operation, previous.blockName, current.blockName);
            replace(l.one, previous.blockName, current.blockName);
            replace(l.two, previous.blockName, current.blockName);
        }
    }
}