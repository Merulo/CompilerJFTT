#include "IRBase.hpp"

class SecondIR : public IRBase
{
    public:
    SecondIR() : IRBase("SecondIR") {};

    void parse(std::vector<Block> b);
    void addSimpleJumps(std::vector<Block> b);
    void removeEmptyBlocks();

    private:
    Block& getBlockByName(std::string name);

};