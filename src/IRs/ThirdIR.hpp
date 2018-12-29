#include "IRBase.hpp"

class ThirdIR : public IRBase
{
    public:
    ThirdIR() : IRBase("ThirdIR") {};

    void parse(std::vector<Block> b);

    private:
    bool checkVariablesAreSameTable(std::string one, std::string two)
    {
        if (one.find("(") == std::string::npos)
        {
            return false;
        }
        if (two.find("(") == std::string::npos)
        {
            return false;
        }
        std::string oneArray = one.substr(0, one.find("("));
        std::string twoArray = two.substr(0, two.find("("));
        return oneArray == twoArray;
    }

    void removeCopyWithSameArguments();
    void searchForTwoTablesWithSameOperation();
    void removeTwoTablesInSameOperation();
    void legalize();
    void legalizeJumps();
    void legalizeJLS(Block& b);
};