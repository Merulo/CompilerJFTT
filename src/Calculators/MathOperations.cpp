#include "MathOperations.hpp"


std::vector<Line> MathOperations::generateMultiplication(std::string registerB, std::string registerC, std::string registerD, Line l)
{
    std::vector<Line> lines;
    std::string labelInCaseOfZero = generateLabel();
    //check if either registerB or registerC is zero.
    lines.push_back({"JZERO", registerB, labelInCaseOfZero});
    lines.push_back({"JZERO", registerC, labelInCaseOfZero});

    //compare so that we multiply by smaller
    lines.push_back({"COPY", registerD, registerC});
    lines.push_back({"COPY", registerD, registerB});
    std::string labelInCaseNoSwapNeeded = generateLabel();

    lines.push_back({"JZERO", registerD, labelInCaseNoSwapNeeded});
    
    //swap
    lines.push_back({"COPY", registerD, registerC});
    lines.push_back({"COPY", registerC, registerB});
    lines.push_back({"COPY", registerB, registerD});

    lines.push_back({labelInCaseNoSwapNeeded});
    lines.push_back({"SUB", registerD, registerD});

    std::string skipOddC = generateLabel();
    std::string mainLoopLabel = generateLabel();

    lines.push_back({mainLoopLabel});
    lines.push_back({"JODD", registerC, skipOddC});

    std::string handleOddC = generateLabel();
    lines.push_back({handleOddC});
    
    lines.push_back({"ADD", registerB, registerB});
    lines.push_back({"HALF", registerC});

    std::string exitLabel = generateLabel();
    lines.push_back({"JZERO", registerC, exitLabel});
    lines.push_back({"JUMP", "",mainLoopLabel});

    lines.push_back({skipOddC});
    lines.push_back({"ADD", registerD, registerB});
    lines.push_back({"JUMP", "", handleOddC});

    //in case this is zero
    lines.push_back({labelInCaseOfZero});
    lines.push_back({"SUB", registerD, registerD});
    lines.push_back({exitLabel});
    lines.push_back({"\t#end of performing MUL operation"});    
    lines.push_back({"COPY", registerB, registerD});

    return lines;
}

unsigned int MathOperations::labelCounter = 0;