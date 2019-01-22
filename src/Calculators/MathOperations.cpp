#include "MathOperations.hpp"

std::string MathOperations::labelName = "#LABEL_";
unsigned int MathOperations::labelCounter = 0;

std::string MathOperations::getLabelName()
{
    return labelName;
}

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

    return lines;
}

std::vector<Line> MathOperations::generateDivision(
    std::string registerB, std::string registerC,
    std::string registerD, std::string registerE,
    std::string registerF, Line l
)
{
    std::vector<Line> lines;

    lines.push_back({"COPY", registerD, registerB});
    lines.push_back({"INC", registerD});
    lines.push_back({"SUB", registerD, registerC});
    
    std::string label32 = generateLabel();
    lines.push_back({"JZERO", registerD, label32});

    lines.push_back({"SUB", registerD, registerD});
    lines.push_back({"SUB", registerE, registerE});

    std::string label31 = generateLabel();
    lines.push_back({"JZERO", registerC, label31});    

    std::string label9 = generateLabel();
    lines.push_back({label9});      

    lines.push_back({"COPY", registerF, registerB});
    lines.push_back({"INC", registerF});
    lines.push_back({"SUB", registerF, registerC});

    std::string label16 = generateLabel();
    lines.push_back({"JZERO", registerF, label16});    

    lines.push_back({"ADD", registerC, registerC});
    lines.push_back({"INC", registerE});

    lines.push_back({"JUMP", "", label9});
    lines.push_back({label16});  

    lines.push_back({"INC", registerD});
    lines.push_back({"DEC", registerE});
    lines.push_back({"HALF", registerC});
    lines.push_back({"SUB", registerB, registerC});

    std::string label20 = generateLabel();
    lines.push_back({label20}); 

    lines.push_back({"JZERO", registerE, label32});  

    lines.push_back({"ADD", registerD, registerD});
    lines.push_back({"HALF", registerC});
    lines.push_back({"DEC", registerE});
    lines.push_back({"COPY", registerF, registerB});
    lines.push_back({"INC", registerF});
    lines.push_back({"SUB", registerF, registerC});

    lines.push_back({"JZERO", registerF, label20});  
    lines.push_back({"INC", registerD});
    lines.push_back({"SUB", registerB, registerC});
    lines.push_back({"JUMP", "", label20});  

    lines.push_back({label31});    
    lines.push_back({"SUB", registerB, registerB});

    lines.push_back({label32});    

    return lines;
}

std::vector<Line> MathOperations::generateModulo(
    std::string registerB, std::string registerC,
    std::string registerE, std::string registerF, Line l
)
{
    std::vector<Line> lines;

    lines.push_back({"COPY", registerF, registerB});
    lines.push_back({"INC", registerF});
    lines.push_back({"SUB", registerF, registerC});
    
    std::string label32 = generateLabel();
    lines.push_back({"JZERO", registerF, label32});

    lines.push_back({"SUB", registerE, registerE});

    std::string label31 = generateLabel();
    lines.push_back({"JZERO", registerC, label31});    

    std::string label9 = generateLabel();
    lines.push_back({label9});      

    lines.push_back({"COPY", registerF, registerB});
    lines.push_back({"INC", registerF});
    lines.push_back({"SUB", registerF, registerC});

    std::string label16 = generateLabel();
    lines.push_back({"JZERO", registerF, label16});    

    lines.push_back({"ADD", registerC, registerC});
    lines.push_back({"INC", registerE});

    lines.push_back({"JUMP", "", label9});
    lines.push_back({label16});  

    lines.push_back({"DEC", registerE});
    lines.push_back({"HALF", registerC});
    lines.push_back({"SUB", registerB, registerC});

    std::string label20 = generateLabel();
    lines.push_back({label20}); 

    lines.push_back({"JZERO", registerE, label32});  

    lines.push_back({"HALF", registerC});
    lines.push_back({"DEC", registerE});
    lines.push_back({"COPY", registerF, registerB});
    lines.push_back({"INC", registerF});
    lines.push_back({"SUB", registerF, registerC});

    lines.push_back({"JZERO", registerF, label20});  
    lines.push_back({"SUB", registerB, registerC});
    lines.push_back({"JUMP", "", label20});  

    lines.push_back({label31});    
    lines.push_back({"SUB", registerB, registerB});

    lines.push_back({label32});    

    return lines;
}

std::vector<Line> MathOperations::generateModuloTwo(
    std::string registerB, Line l
)
{
    std::vector<Line> lines;
    std::string label4 = generateLabel();
    lines.push_back({"JODD", registerB, label4});
    lines.push_back({"SUB", registerB, registerB});
    std::string label6 = generateLabel();

    lines.push_back({"JUMP", "", label6}); 

    lines.push_back({label4});  
    lines.push_back({"SUB", registerB, registerB});
    lines.push_back({"INC", registerB});
    lines.push_back({label6});

    return lines;
}

std::string MathOperations::generateLabel()
{
    std::string label = labelName + std::to_string(labelCounter);
    labelCounter++;
    return label;
}