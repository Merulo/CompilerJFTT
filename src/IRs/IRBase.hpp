#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>

#include "SymbolTable.hpp"
#include "DataTypes/Line.hpp"

typedef const std::string & cStrRef;

class IRBase
{
    public:
    IRBase(std::string IRName);
    void print(cStrRef fileName = "");
    std::vector<Line> getLines();
    void setSymbolTable(std::shared_ptr<SymbolTable> symbolTable);

    protected:
    std::shared_ptr<SymbolTable> _symbolTable;
    std::vector<Line> _lines;

    private:
    void writeToStream(std::ostream& stream);
    std::string _IRName;
};