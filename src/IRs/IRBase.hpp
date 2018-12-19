#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#include "DataTypes/Line.hpp"

typedef const std::string & cStrRef;

class IRBase
{
    public:
    IRBase(std::string IRName);
    void print(cStrRef fileName = "");

    protected:
    std::vector<Line> _lines;

    private:
    void writeToStream(std::ostream& stream);
    std::string _IRName;
};