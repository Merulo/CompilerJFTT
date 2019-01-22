#include "IRBase.hpp"

class FifthIR : public IRBase
{
    public:
    FifthIR() : IRBase("FifthIR") {};

    void parse(std::vector<Block> b);

    private:
    void removeJumps();
    void replaceJump(Block& previous, Block& current);
    void calculateJumps();
    bool onlyComments(Block& b);
    int getFirstInstructionInBlock(Block& b);
    int getFirstInstructionAfter(Block& b, std::string name);

    void replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
        {
            return;
        }
        if(start_pos + from.length() != str.length())
        {
            return;
        }
        str.replace(start_pos, from.length(), to);
        return;
    }
    Block& getNextBlock(Block& start)
    {
        bool found = false;
        for (auto& b : _blocks)
        {
            if (found)
            {
                return b;
            }
            if (b == start)
            {
                found = true;
            }
        }
        return start;
    }
};