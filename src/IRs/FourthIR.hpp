#include "IRBase.hpp"

#include "Register/Pair.hpp"
#include "Register/RegisterBlock.hpp"

class FourthIR : public IRBase
{
    public:
    FourthIR() : IRBase("FourthIR") {};
    void parse(std::vector<Block> b);

    private:
    std::vector<Pair> _notYetConvertedBlocks;

    Pair& getBlockByName(std::string searched)
    {
        auto result = std::find_if(_notYetConvertedBlocks.begin(), _notYetConvertedBlocks.end(),
        [&searched](auto& tested){
            return searched == tested.block.blockName;
        });     
        if (result == _notYetConvertedBlocks.end())
        {
            std::cout<<"problem with getBlockByName, searching for="<<searched<<std::endl;
        }
        return *result;
    }    


    //converts to assembler
    void convertToAssembler();
    void convertBlockToAssembler(Pair& pair, RegisterBlock& registerBlock);
    void continueConverting(Pair& p, RegisterBlock rb);
        void handleSinglePath(Pair& p, RegisterBlock rb);
        void handleBranchSimple(std::string name, RegisterBlock rb);
        void alignRegisters(Pair& p, Pair& next);
        void handleBranch(Pair& p, RegisterBlock& rb, std::string next);
        void checkVariableIsInOtherRegister(Register& r, std::vector<Register>& regTargeted, Block& tmp, Pair& p);
        
    //handles translating operations
    void handleConst(RegisterBlock& rb, Block& b, Line& l);
    void handleWrite(RegisterBlock& rb, Block& b, Line& l);
    void handleRead(RegisterBlock& rb, Block& b, Line& l);
    void handleCopy(RegisterBlock& rb, Block& b, Line& l);
    void handleDirectTranslation(RegisterBlock& rb, Block& b, Line& l);
    void handleSimpleOperation(RegisterBlock& rb, Block& b, Line& l);
    void handleMul(RegisterBlock& rb, Block& b, Line& l);
    void handleDiv(RegisterBlock& rb, Block& b, Line& l);

    //updates registers status
    void updateRegisterState(Block& b, RegisterBlock& rb, Register& r, std::string name);
    void updateRegisterStateWithConst(Block& b, RegisterBlock& rb, Register& r, std::string name);

    //handles memory management
    void prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, std::string name, bool save = true);
    std::vector<Line> prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, std::string name);
};