#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <stack>
#include <map>
#include <stack>

#include "DataTypes/Line.hpp"
#include "DataTypes/Data.hpp"
#include "IRBase.hpp"

typedef const std::string & cStrRef;

class FirstIR : public IRBase
{
    public:
    FirstIR() : IRBase("FirstIR") {};
    
    //adds new code
    void addNewCode(cStrRef operation, cStrRef one = "", cStrRef two = "");
    //performs math operation
    void handleMathOperation(cStrRef resultName);
    //handles condition operation
    void handleConditionOperation(cStrRef operation, cStrRef one, cStrRef two);
    //generates new variable
    std::string getVariable(std::string value);
    
    //setters for nested operations
    void setOperation(cStrRef operation);
    void setFirstExtraParameter(cStrRef first);
    void setSecondExtraParameter(cStrRef second);

    //for conditions
    void endElse();
    void endIf();
    void closeConditionBlock();


    void closeForBlock()
    {
        //closes current block
        _blocks.push_back(_currentBlock.top());
        _currentBlock.pop();
        Block b1 = generateBlock();
        _currentBlock.push(b1);
        _tests.push(b1);       
    }
    //for loops
    void endWhileDo();
    void endDoWhile();
    void insertFor(std::string iterator, Data from, Data to)
    {
        closeForBlock();
        Block b1 = _tests.top();
        _tests.pop();
        Block b2 = _tests.top();
        _tests.pop();

        Block firstControlBlock = createBeforeForBlock(iterator, from, to);
        firstControlBlock.blockIfFalse = b2.blockName;
        firstControlBlock.blockIfTrue = b1.blockName;

        Block secondControlBlock = createSecondControlBlock(iterator);
        secondControlBlock.blockIfFalse = b2.blockName;
        secondControlBlock.blockIfTrue = b1.blockName;        
        
        for(size_t i = 0; i < _blocks.size(); i++)
        {
            if (_blocks[i] == b2)
            {
                _blocks.insert(_blocks.begin() + i, firstControlBlock);
                break;
            }
        }

        _blocks.push_back(_currentBlock.top());
        _currentBlock.pop();
        Block testing = generateBlock();
        _currentBlock.push(testing);

        for(size_t i = 0; i < _blocks.size(); i++)
        {
            if (_blocks[i] == b1)
            {
                _blocks.insert(_blocks.begin() + i, secondControlBlock);
                return;
            }
        }        

    }

    Block createBeforeForBlock(std::string iterator, Data from, Data to)
    {
        Block b = generateBlock();
        Line iteratorInit = getLine(from);
        iteratorInit.one = iterator;
        b.lines.push_back(iteratorInit);

        Line counterInit = getLine(to);
        counterInit.one = iterator + forControlName;
        b.lines.push_back(counterInit);

        Line counterFinal = getLine(from);
        counterFinal.operation = "SUB";
        counterFinal.one = counterInit.one;
        counterFinal.two = iterator;
        b.lines.push_back(counterFinal);

        Line jump;
        jump.operation = "JZERO";
        jump.one = counterInit.one;
        b.lines.push_back(jump);
        return b;
    }

    Block createSecondControlBlock(std::string iterator)
    {
        Block b = generateBlock();
        Line incLine;
        incLine.operation = "INC";
        incLine.one = iterator;
        b.lines.push_back(incLine);

        Line decLine;
        decLine.operation = "DEC";
        decLine.one = iterator + forControlName;
        b.lines.push_back(decLine);

        Line line;
        line.operation = "JZERO";
        line.one = iterator + forControlName;
        b.lines.push_back(line);    

        return b;
    }

    Line getLine(Data d)
    {
        Line l;
        if (!d.name.empty())
        {
            l.operation = "COPY";
            l.two = d.name;
        }
        else
        {
            l.operation = "CONST";
            l.two = std::to_string(d.value);
        }
        return l;
    }

    private:
    void reset();
    void handleNonCommutativeOperation(cStrRef resultName, cStrRef operation);

    int _registerCount = 0;

    std::string _operation;
    std::string _firstExtraParameter;
    std::string _secondExtraParameter;
    std::string _arrayValue;
    std::stack<Block> _conditionBlocks;
    std::stack<Block> _tests;

    void print123()
    {
        for(auto b : _blocks)
        {
            std::cout<<"BLOCKS="<<b.blockName<<std::endl;
        }
    }
};