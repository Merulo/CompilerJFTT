#include "SecondIR.hpp"

void SecondIR::parse(std::vector<Block> b)
{
    addSimpleJumps(b);
    removeEmptyBlocks();
    optimize();
}

void SecondIR::optimize()
{
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