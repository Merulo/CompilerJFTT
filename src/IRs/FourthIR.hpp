#include "IRBase.hpp"

#include "Register/Pair.hpp"
#include "Register/RegisterBlock.hpp"

class FourthIR : public IRBase
{
    public:
    FourthIR() : IRBase("FourthIR") {};
    void parse(std::vector<Block> b);

    private:
    std::vector<Block> _notYetConvertedBlocks;
    bool _removeConsts = true;

    //converts to assembler
    void convertToAssembler();
    void convertBlockToAssembler(Block& block, RegisterBlock& registerBlock);
    void continueConverting(Block& block, RegisterBlock rb);
        void convertNextBlock(Block& block, RegisterBlock& rb, std::string name);
        void convertSplitBlock(Block& block, RegisterBlock& rb, std::string name);

    //handles translating operations
    void handleConst(RegisterBlock& rb, Block& b, Line& l);
    void handleWrite(RegisterBlock& rb, Block& b, Line& l);
    void handleRead(RegisterBlock& rb, Block& b, Line& l);
    void handleCopy(RegisterBlock& rb, Block& b, Line& l);
    void handleDirectTranslation(RegisterBlock& rb, Block& b, Line& l);
    void handleSimpleOperation(RegisterBlock& rb, Block& b, Line& l);
    void handleMul(RegisterBlock& rb, Block& b, Line& l);
    void handleDiv(RegisterBlock& rb, Block& b, Line& l);
    void handleMod(RegisterBlock& rb, Block& b, Line& l);

    //updates registers status
    void updateRegisterState(Block& b, RegisterBlock& rb, Register& r, std::string name);
};