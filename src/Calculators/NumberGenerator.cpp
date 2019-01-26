#include "NumberGenerator.hpp"

std::vector<Line> NumberGenerator::_result;

std::vector<Line> NumberGenerator::generateConstFrom(
    unsigned long long target, 
    std::vector<std::pair<std::string, unsigned long long>> values)
{
    _result.clear();
    
    for(auto option = values.begin() + 1; option != values.end(); option++)
    {
        if(target > option->second)
        {
            unsigned long long diff = target - option->second;
            if (diff < 15)
            {
                _result.push_back({"#PERFORMING COPY TO REACH " + std::to_string(target)});
                _result.push_back({"COPY", values.front().first, option->first});
                while(diff > 0)
                {
                    _result.push_back({"INC", values.front().first});
                    diff --;
                }
                return _result;
            }
        }
        else if (target <= option->second)
        {
            unsigned long long diff = option->second - target;
            if (diff < 15)
            {
                _result.push_back({"#PERFORMING COPY TO REACH " + std::to_string(target)});                
                _result.push_back({"COPY", values.front().first, option->first});
                while(diff > 0)
                {
                    _result.push_back({"DEC", values.front().first});
                    diff --;
                }
                return _result;
            }
        }
    }


    getCostWithIncAndAdd(target);
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
            _result.push_back({"ADD", name, name});
            target = target / 2;
        }
        else
        {
            _result.push_back({"INC", name});
            target--;
        }
    }
    for(size_t i = 0; i < target; i++)
    {
        _result.push_back({"INC", name});
    }
    _result.push_back({"SUB", name, name});
    std::reverse(_result.begin(), _result.end());
}
