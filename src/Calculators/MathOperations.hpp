#include "DataTypes/Line.hpp"


class MathOperations
{
    public:
    static std::vector<Line> generateMultiplication(
        std::string registerB, std::string registerC, 
        std::string registerD, Line l);
    static std::vector<Line> generateDivision(
        std::string registerB, std::string registerC,
        std::string registerD, std::string registerE,
        std::string registerG, Line l
    );
    static std::vector<Line> generateModulo(
        std::string registerB, std::string registerC,
        std::string registerE, std::string registerG, Line l
    );
    static std::string getLabelName();

    private:
    static std::string generateLabel();

    static unsigned int labelCounter;
    static std::string labelName;

};