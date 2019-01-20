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


    //optimizations:
        //iterators:
        void removeUnusedIterators();
            bool isUsedInEndingBlock(std::string variable, Block& startingBlock, Block& endingBlock);
            void stripVariable(std::string variable, Block& b);
            void stripForIterator(std::string variable, Block& b);
        //add sub small const:
        void removeAddSubSmallConst();
            void checkIfThisCanBeRemoved(std::vector<Line>& lines, std::vector<Line>::iterator& iterator);
};