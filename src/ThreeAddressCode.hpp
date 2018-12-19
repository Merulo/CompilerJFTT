#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <stack>

typedef const std::string & cStrRef;

struct Line
{
    std::string thisLabel;
    std::string targetLabel;
    std::string operation;
    std::string one;
    std::string two;
};

class ThreeAddressCode
{
    public:
    void addNewCode(cStrRef operation, cStrRef one = "", cStrRef two = "");
    void handleMathOperation(cStrRef resultName);
    void handleConditionOperation(cStrRef operation, cStrRef one, cStrRef two)
    {
        addNewCode(operation, one, two);
        std::string label = generateLabel();
        _lines.back().targetLabel = label;
        std::cout<<"Generated label="<<label<<std::endl;
        _labels.push(label);

    }
    void setOperation(cStrRef operation);
    void setFirstExtraParameter(cStrRef first);
    void setSecondExtraParameter(cStrRef second);
    std::string getVariable();
    std::string generateLabel()
    {
        std::string result = "L_" + std::to_string(_labelCount);
        _labelCount++;
        return result; 
    }

    void addJump()
    {
        addNewCode("JUMP");
        std::string label = generateLabel();
        _lines.back().targetLabel = label;
        std::cout<<"Generated label="<<label<<std::endl;
        _labels.push(label);        
    }

    void swap()
    {
        std::string s1 = _labels.top();
        _labels.pop();
        std::string s2 = _labels.top();
        _labels.pop();
        _labels.push(s1);
        _labels.push(s2);
    }

    void endIf()
    {
        // std::cerr<<"ENDIF="<<tester<<std::endl;
        tester++;
    }

    void print(cStrRef fileName = "");
    private:
    void reset();
    void writeToStream(std::ostream& stream)
    {
        stream<<"ThreeAddressCode:"<<std::endl;
        for(auto l : _lines)
        {
            if (!l.thisLabel.empty())
            {
                stream<<"#"<<l.thisLabel<<std::endl;
                continue;
            }
            stream<<l.operation<<": "<<l.one<<" "<<l.two<<" ";
            if (!l.targetLabel.empty())
            {
                stream<<"goto "<<l.targetLabel;
            }
            stream<<std::endl;
        }
    }
    void handleNonCommutativeOperation(cStrRef resultName, cStrRef operation);

    int _registerCount = 0;
    int _labelCount = 0;
    std::vector<Line> _lines;

    std::string _operation;
    std::string _firstExtraParameter;
    std::string _secondExtraParameter;

    std::stack<std::string> _labels;

    int tester = 0;

};