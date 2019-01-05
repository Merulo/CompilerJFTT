#include "DataTypes/Line.hpp"


class MathOperations
{
    public:
    static std::vector<Line> generateMultiplication(
        std::string registerB, std::string registerC, 
        std::string registerD, Line l);
    static std::vector<Line> generateDivision(
        std::string registerB, std::string regsiterC,
        std::string registerD, std::string registerE,
        std::string registerG, Line l
    );


    private:
    static unsigned int labelCounter;
    static std::string generateLabel()
    {
        std::string label = "#LABEL:" + std::to_string(labelCounter);
        labelCounter++;
        return label;
    }

};