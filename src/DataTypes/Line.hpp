#pragma once

#include <string>
#include <vector>
#include <iostream>

struct Line
{
    Line(){};
    Line(std::string operationInit, std::string oneInit= "", std::string twoInit = "")
    {
        operation = operationInit;
        one = oneInit;
        two = twoInit;
    }
    std::string operation;
    std::string one;
    std::string two;
    friend std::ostream& operator<<(std::ostream& os, const Line& l)
    {
        os << l.operation<< " " << l.one << " " << l.two;
        return os;
    };    
};


struct Block
{
    std::string blockName;
    std::vector<Line> lines;
    std::string blockIfTrue;
    std::string blockIfFalse;
    std::string blockJump;
    inline bool operator==(const Block& rhs){ return blockName == rhs.blockName; }

    friend std::ostream& operator<<(std::ostream& os, const Block& b)
    {
        os << "#"<< b.blockName << std::endl;
        for(auto l : b.lines)
        {
            os <<"\t" << l << std::endl;
        }
        if (!b.blockIfTrue.empty())
        {
            os << "#ifTrue "<< b.blockIfTrue << std::endl;
        }
        if (!b.blockIfFalse.empty())
        {
            os << "#ifFalse "<< b.blockIfFalse << std::endl;
        }
        if (!b.blockJump.empty())
        {
            os << "#jump to "<<b.blockJump << std::endl;
        }
        os << "#end of " << b.blockName << std::endl;
        return os;
    };     
};