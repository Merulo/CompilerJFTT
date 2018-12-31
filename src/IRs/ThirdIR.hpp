#include "IRBase.hpp"

class ThirdIR : public IRBase
{
    public:
    ThirdIR() : IRBase("ThirdIR") {};

    void parse(std::vector<Block> b);

    private:
    bool checkVariablesAreSameTable(std::string one, std::string two);

    void removeCopyWithSameArguments();
    void searchForTwoTablesWithSameOperation();
    void legalize();
    void legalizeJumps();
    void legalizeJLS(Block& b);
    void legalizeJMR(Block& b);

    std::string calculateDiff(Block& b, std::string one, std::string two);
    void insertJumps(Block&b, std::string var);
};