#include "IRBase.hpp"

#include "Register/Pair.hpp"
#include "Register/RegisterBlock.hpp"

class FourthIR : public IRBase
{
    public:
    FourthIR() : IRBase("FourthIR") {};
    void parse(std::vector<Block> b, std::string fileName, bool useEmulator);
    bool isThisVariableUsed(std::string name, Block& block);

    private:
    std::vector<Pair> _notYetConvertedBlocks;
    bool _removeConsts = true;

    bool isDeterministic();
    bool recursiveUsageTest(std::string name, Block& b);

    //converts to assembler
    void convertToAssembler();
    void convertBlockToAssembler(Pair& pair, RegisterBlock& registerBlock);
    void continueConverting(Pair& pair, RegisterBlock rb);
        void convertNextBlock(Pair& pair, RegisterBlock& rb, std::string name);
        void convertSplitBlock(Pair& pair, RegisterBlock& rb, std::string name);
        void alignRegisters(Pair& pair, Pair& next, RegisterBlock& rb, std::string name);

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

    Pair& getPairByName(std::string name)
    {
        for(auto& pair : _notYetConvertedBlocks)
        {
            if (pair.block.blockName == name)
            {
                return pair;
            }
        }
        // std::cout<<"PROBLEM asking for "<< name <<std::endl;
        return _notYetConvertedBlocks.front();
    }

    void restPairBlocks()
    {
        for(auto& p : _notYetConvertedBlocks)
        {
            p.block.visited = false;
        }
    }
};