#include "FinalIR.hpp"

void FinalIR::parse(const std::vector<Line>& TACLines)
{
    for(auto& l : TACLines)
    {
        if (_simpleInstructions.count(l.operation))
        {
            _lines.push_back(l);
        }
    }
}