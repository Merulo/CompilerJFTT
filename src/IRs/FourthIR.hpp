#include "IRBase.hpp"

#include "Register/Pair.hpp"
#include "Register/RegisterBlock.hpp"
#include "Calculators/NumberGenerator.hpp"

class FourthIR : public IRBase
{
    public:
    FourthIR() : IRBase("FourthIR") {};

    void parse(std::vector<Block> b);

    private:
    std::vector<Pair> _notYetConvertedBlocks;

    Pair& getBlockByName(std::string searched, std::vector<Pair>& blocks)
    {
        auto result = std::find_if(blocks.begin(), blocks.end(),
        [&searched](auto& tested){
            return searched == tested.block.blockName;
        });     
        if (result == blocks.end())
        {
            std::cout<<"problem with getBlockByName, searching for="<<searched<<std::endl;
        }
        return *result;
    }    


    // //related to mergin registers
    // Block getMeetingBlock(Block&b);
    // void traverse(Block& b, std::vector<Block>& blocks, std::vector<std::string>& visited);
    // void appendSaveOfVariable(Block& target, Block& meeting, RegisterBlock& copy, Register& reg, Block& last);
    // void mergeRegisters(
    //     std::vector<Register>& regT, std::vector<Register>& regF, 
    //     Block& t, Block& f, 
    //     RegisterBlock& copyForT, RegisterBlock& copyForF,
    //     Block& lastT, Block& lastF,
    //     Block& meeting, RegisterBlock& rb);


    // Block& handleSplit(Block& b, RegisterBlock rb, Block& lastBlock);
    // Block& continueConverting(Block& b, RegisterBlock rb, Block& lastBlock);

    void convertToAssembler();
    void convertBlockToAssembler(Pair& pair, RegisterBlock& registerBlock);
    void continueConverting(Pair& p, RegisterBlock rb);

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