#pragma once

#include <string>
#include <iostream>
#include <vector>

struct Line
{
    std::string name;
    std::string one;
    std::string two;
    std::string three;
};

class ThreeAddressCode
{
    public:
    void addNewCode(const std::string& name, const std::string& one = "", const std::string& two = "", const std::string& three = "")
    {
        Line line;
        line.name = name;
        line.one = one;
        line.two = two;
        line.three = three;
        if (two.empty())
        {
            line.two = _loadedTwo;
            _loadedTwo = "";
        }
        if (three.empty())
        {
            line.three = _loadedThree;
            _loadedThree = "";
        }
        _lines.push_back(line);
    }

    void loadLocalParameters(const std::string& two = "", const std::string& three = "")
    {
        _loadedTwo = two;
        _loadedThree = three;
    }

    void print()
    {
        std::cerr<<"ThreeAddressCode:"<<std::endl;
        for(auto l : _lines)
        {
            std::cerr<<l.name<<": "<<l.one<<" "<<l.two<<" "<<l.three<<std::endl;
        }
    }

    private:
    std::vector<Line> _lines;
    std::string _loadedTwo;
    std::string _loadedThree;


    
};