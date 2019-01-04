#include "FifthIR.hpp"

void FifthIR::parse(std::vector<Block> b)
{
    std::cout<<"FifthIR WORKING"<<std::endl;
    _blocks = b;
    //todo: remove jumps if the block is the next block
    // removeUnneededJumps()
    calculateJumps();
}

void FifthIR::calculateJumps()
{
    unsigned long long instructionCounter = 0;
    for(auto& b : _blocks)
    {
        for(auto& line : b.lines)
        {
            if (line.operation.find("#") == std::string::npos)
            {
                line.operationNumber = instructionCounter;
                // line.two = line.two + " #" + std::to_string(instructionCounter);
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
                std::cout<<line<<std::endl;
                argument = argument.substr(1, std::string::npos);
                int jumpTarget = getFirstInstructionInBlock(getBlockByName(argument, _blocks));
                line.two = std::to_string(jumpTarget) + " " + line.two;
            }
        }
    }
}

int FifthIR::getFirstInstructionInBlock(Block& b)
{
    for(auto& line : b.lines)
    {
        if (line.operation.find("#") == std::string::npos)
        {
            return line.operationNumber;
        }
    }
    return -1;
}
