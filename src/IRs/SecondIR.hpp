#include <set>

#include "IRBase.hpp"

class SecondIR : public IRBase
{
    public:
    SecondIR() : IRBase("SecondIR") {};

    void parse(std::vector<Block> b);

    private:
    std::set<std::string> _banned = {
        "INC", "DEC", "JZERO", 
        "JODD", "JLS", "JMR", 
        "JLE", "JME", "JNE", 
        "JEQ", "WRITE", "MUL", 
        "ADD", "SUB", "DIV", 
        "MOD", "HALF"};

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
        //mul by power of 2
        void searchForMulAndDiv();
            void checkPowerOfTwoMul(Line& l, Block& b, size_t i);
            void checkPowerOfTwoDiv(Line& l, Block& b, size_t i);
            void checkTwoMod(Line& l, Block& b, size_t i);
        //removing unused calculations
        void removeUnusedCalculations();
            void checkOperations(std::vector<Line>::iterator& line, Block& b);
            bool canRemove(std::vector<Line>::iterator line, Block& b, std::string name);
            bool canRemoveRecursive(Block& b, std::string name);
};