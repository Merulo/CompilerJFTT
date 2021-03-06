#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <optional>

#include "Line.hpp"

struct Block
{
    std::string blockName;
    std::vector<Line> lines;
    std::string blockIfTrue;
    std::string blockIfFalse;
    std::string blockJump;
    bool visited = false;

    inline bool operator==(const Block& rhs){ return blockName == rhs.blockName; }
    inline bool operator!=(const Block& rhs){ return !(*this == rhs); }

    void insert(std::vector<Line>& ls)
    {
        lines.insert(lines.end(), ls.begin(), ls.end());
    }

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