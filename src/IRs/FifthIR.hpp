#include "IRBase.hpp"

class FifthIR : public IRBase
{
    public:
    FifthIR() : IRBase("FifthIR") {};

    void parse(std::vector<Block> b);

    private:
    void calculateJumps();
    int getFirstInstructionInBlock(Block& b);

};