#include <algorithm>

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

    Block& getBlockByName(std::string searched)
    {
        auto result = std::find_if(_notYetConvertedBlocks.begin(), _notYetConvertedBlocks.end(),
        [&searched](auto& tested){
            return searched == tested.blockName;
        });     
        return *result;
    }

    void findNextBlock(Block& b, RegisterBlock rb)
    {
        if (!b.blockJump.empty())
        {
            convertBlockToAssembler(getBlockByName(b.blockJump), rb);
            return;
        }
        else if (!b.blockIfFalse.empty() && !b.blockIfTrue.empty())
        {
            //find the meeting block
            std::vector<Block> ls;
            std::vector<Block> rs;
            traverse(getBlockByName(b.blockIfTrue), ls);
            traverse(getBlockByName(b.blockIfFalse), rs);
            
            for(auto& l : ls)
            {
                for(auto& r : rs)
                {
                    if (l == r)
                    {
                        std::cout<<"meeting block is "<<r.blockName<<std::endl;
                        break;
                    }
                }
            }
            return;
        }

    }

    void traverse(Block& b, std::vector<Block>& blocks)
    {
        blocks.push_back(b);
        if (!b.blockJump.empty())
        {
            traverse(getBlockByName(b.blockJump), blocks);
            return;
        }
        else if (!b.blockIfFalse.empty() && !b.blockIfTrue.empty())
        {
            //find the meeting block
            std::vector<Block> ls;
            std::vector<Block> rs;
            traverse(getBlockByName(b.blockIfTrue), ls);
            traverse(getBlockByName(b.blockIfFalse), rs);
            
            for(auto& l : ls)
            {
                for(auto& r : rs)
                {
                    if (l == r)
                    {
                        blocks.push_back(r);
                        traverse(r, blocks);
                        return;
                    }
                }
            }
            std::cout<<"PROBLEM in traverse, no matching found"<<std::endl;
        }
        
    }

    void convertToAssembler();
    void convertBlockToAssembler(Block& block, RegisterBlock registerBlock);

    void handleConst(RegisterBlock& rb, Block& b, Line& l);
    void handleWrite(RegisterBlock& rb, Block& b, Line& l);
    void handleRead(RegisterBlock& rb, Block& b, Line& l);
    void handleCopy(RegisterBlock& rb, Block& b, Line& l);
    void handleDirectTranslation(RegisterBlock& rb, Block& b, Line& l);

    void updateRegisterState(Block& b, RegisterBlock& rb, Register& r, std::string name);
    void updateRegisterStateWithConst(Block& b, RegisterBlock& rb, Register& r, std::string name);

    void prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, std::string name);
    void prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, std::string name);
};