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
    std::set<std::string> _simpleInstructions = {"READ", "WRITE", "HALT", "CONST"};
};