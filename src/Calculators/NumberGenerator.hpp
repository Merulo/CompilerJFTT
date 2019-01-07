#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "DataTypes/Line.hpp"

class NumberGenerator
{
public:
    static std::vector<Line> generateConstFrom(
        unsigned long long target, 
        std::vector<std::pair<std::string, unsigned long long>> values);

private:
    static std::vector<Line> _result;

    static int getCostWithIncAndAdd(unsigned long long target);
    static void generateWithIncAndAdd(unsigned long long target, std::string name);

};
