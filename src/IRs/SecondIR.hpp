#include "IRBase.hpp"

class SecondIR : public IRBase
{
    public:
    SecondIR() : IRBase("SecondIR") {};

    void parse(std::vector<Block> b);

    private:
    void addSimpleJumps(std::vector<Block> b);
    void removeEmptyBlocks();

    void optimize();

};