#include "FinalIR.hpp"

void FinalIR::parse(const std::vector<Line>& FIRLines)
{
    for(auto& l : FIRLines)
    {
        if (_simpleInstructions.count(l.operation))
        {
            _lines.push_back(l);
        }
        else if (l.operation == "MUL")
        {
            std::string target = l.one;
            std::string first = l.two;
            std::string second;


            if (_lines.back().operation == "COPY")
            {
                second = _lines.back().two;
                _lines.pop_back();
            }
            else if (_lines.back().operation == "CONST")
            {
                second = l.one;
            }
            // print();
            // std::cerr<<"WIERD ORDER"<<std::endl;
            std::cerr<<"Creating mult using "<<first<< " and "<<second<<std::endl; 
            // legalize
        }
        else if (_jumpInstructions.count(l.operation))
        {
            handleJump(l);
        }
        else
        {
            _lines.push_back(l);
        }
    }
}