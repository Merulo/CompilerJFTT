#pragma once
#include <vector>
#include <set>

#include "DataTypes/Line.hpp"
#include "IRBase.hpp"

class FinalIR : public IRBase
{
    public:
    FinalIR() : IRBase("FinalIR") {};

    void parse(const std::vector<Line>& TACLines);

    private:
    std::set<std::string> _simpleInstructions = {"READ", "WRITE", "HALT", "CONST", "ADD", "SUB", "MOD", "DIV"};
    std::set<std::string> _jumpInstructions = {"JLE"};
    std::string generateTemporaryVariable()
    {
        std::string result = "temporary_" + std::to_string(_temporaryVariableCount);
        _temporaryVariableCount++;
        return result;
    }

    unsigned int _temporaryVariableCount = 0;

void handleJump(Line line)
{
    std::string var = generateTemporaryVariable();
    {
        Line l;
        l.operation = "COPY";
        l.one = var;
        l.two = line.one;
        _lines.push_back(l);
    }
    {
        Line l;
        l.operation = "SUB";
        l.one = var;
        l.two = line.two;
        _lines.push_back(l);       
    }
    {
        Line l;
        l.operation = "JZERO";
        l.one = var;
        l.targetLabel = line.targetLabel;
        _lines.push_back(l);       
    }
}    
};