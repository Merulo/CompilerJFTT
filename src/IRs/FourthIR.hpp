#include "IRBase.hpp"
#include "Register/RegisterBlock.hpp"

#include "Calculators/NumberGenerator.hpp"

class FourthIR : public IRBase
{
    public:
    FourthIR() : IRBase("FourthIR") {};

    void parse(std::vector<Block> b);

    private:
    std::vector<Block> _notYetConvertedBlocks;
    void convertToAssembler();
    void convertBlockToAssembler(Block& block, RegisterBlock registerBlock);

    void handleConst(RegisterBlock& rb, Block& b, Line& l);
    void handleWrite(RegisterBlock& rb, Block& b, Line& l);
    void handleRead(RegisterBlock& rb, Block& b, Line& l);

    void updateRegisterState(Block& b, RegisterBlock rb, Register& r, Line l);
    void updateRegisterStateWithConst(Block& b, RegisterBlock rb, Register& r, Line l);

    void prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, Line l);
    void prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, Line l);
};