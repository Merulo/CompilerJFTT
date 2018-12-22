#include "NumberGenerator.hpp"

std::vector<std::string> NumberGenerator::_result;

std::vector<std::string> NumberGenerator::generateConstFrom(
    unsigned long long target, 
    std::vector<std::pair<std::string, unsigned long long>> values)
{
    _result.clear();
    int c1 = getCostWithIncAndAdd(target);
    //if c1 is min;
    generateWithIncAndAdd(target, values[0].first);

    return _result;
}


int NumberGenerator::getCostWithIncAndAdd(unsigned long long target)
{
    int cost = 0;
    while (target > 10)
    {
        if (target % 2 == 0)
        {
            target = target / 2;
            cost += 5;
        }
        else
        {
            target--;
            cost++;
        }
    }
    return cost + target;
}

void NumberGenerator::generateWithIncAndAdd(unsigned long long target, std::string name)
{
    while (target > 10)
    {
        if (target % 2 == 0)
        {
            _result.push_back("ADD " + name + " " + name);
            target = target / 2;
        }
        else
        {
            _result.push_back("INC " + name);
            target--;
        }
    }
    for(int i = 0; i < target; i++)
    {
        _result.push_back("INC " + name);
    }
    _result.push_back("SUB " + name + " " + name);
    std::reverse(_result.begin(), _result.end());
}