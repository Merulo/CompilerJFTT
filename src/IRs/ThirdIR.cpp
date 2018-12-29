#include "ThirdIR.hpp"

void ThirdIR::parse(std::vector<Block> b)
{
    _blocks = b;
    legalize();
}

void ThirdIR::legalize()
{
    removeCopyWithSameArguments();
    legalizeJumps();


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

void ThirdIR::legalizeJumps()
{
    for (auto& b : _blocks)
    {
        if (!b.blockJump.empty())
        {
            Line l;
            l.operation = "JUMP";
            l.two = "#" + b.blockJump;
            b.lines.push_back(l);
            b.blockJump = "";
            continue;
        }
        if (b.lines.back().operation == "JLS")
        {
            legalizeJLS(b);
        }
    }
}

void ThirdIR::legalizeJLS(Block& b)
{
    Line lastLine = b.lines.back();
    b.lines.pop_back();

    std::string var = getVariable("NAN");
    
    Line copyLine;
    copyLine.operation = "COPY";
    copyLine.one = var;
    copyLine.two = lastLine.two;
    
    Line subLine;
    subLine.operation = "SUB";
    subLine.one = var;
    subLine.two = lastLine.one;

    Line jzero;
    jzero.operation = "JZERO";
    jzero.one = var;
    jzero.two = "#"+b.blockIfFalse;

    Line jump;
    jump.operation = "JUMP";
    jump.two = "#"+b.blockIfTrue;

    b.lines.push_back(copyLine);
    b.lines.push_back(subLine);
    b.lines.push_back(jzero);
    b.lines.push_back(jump);
    b.blockIfFalse = "";
    b.blockIfTrue = "";
}

