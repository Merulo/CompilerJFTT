#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

typedef const std::string & cStrRef;

struct Line
{
    std::string name;
    std::string one;
    std::string two;
};

class ThreeAddressCode
{
    public:
    void addNewCode(cStrRef name, cStrRef one = "", cStrRef two = "");
    void addAssignCode(cStrRef name);
    void setOperation(cStrRef operation);
    void setFirstExtraParameter(cStrRef first);
    void setSecondExtraParameter(cStrRef second);
    
    std::string getRegister();

    void print(cStrRef fileName = "");
    private:
    void handleAssign(cStrRef name, cStrRef first, cStrRef second);
    void reset();

    int _registerCount = 0;
    std::vector<Line> _lines;
    std::string _operation;
    std::string _firstExtraParameter;
    std::string _secondExtraParameter;

};