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

    //related to mergin registers
    Block getMeetingBlock(Block&b);
    void traverse(Block& b, std::vector<Block>& blocks);
    void appendSaveOfVariable(Block& target, Block& meeting, RegisterBlock& copy, Register& reg, Block& last);
    void mergeRegisters(
        std::vector<Register>& regT, std::vector<Register>& regF, 
        Block& t, Block& f, 
        RegisterBlock& copyForT, RegisterBlock& copyForF,
        Block& lastT, Block& lastF,
        Block& meeting, RegisterBlock& rb);


    Block& handleSplit(Block& b, RegisterBlock rb, Block& lastBlock);
    Block& continueConverting(Block& b, RegisterBlock rb, Block& lastBlock);

    void convertToAssembler();
    Block& convertBlockToAssembler(Block& block, RegisterBlock& registerBlock, Block& lastBlock);

    void handleConst(RegisterBlock& rb, Block& b, Line& l);
    void handleWrite(RegisterBlock& rb, Block& b, Line& l);
    void handleRead(RegisterBlock& rb, Block& b, Line& l);
    void handleCopy(RegisterBlock& rb, Block& b, Line& l);
    void handleDirectTranslation(RegisterBlock& rb, Block& b, Line& l);
    void handleSimpleOperation(RegisterBlock& rb, Block& b, Line& l);

    void updateRegisterState(Block& b, RegisterBlock& rb, Register& r, std::string name);
    void updateRegisterStateWithConst(Block& b, RegisterBlock& rb, Register& r, std::string name);

    void prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, std::string name);
    std::vector<Line> prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, std::string name);
};