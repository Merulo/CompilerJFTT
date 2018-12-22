#include <vector>
#include <string>
#include <algorithm>

class NumberGenerator
{
public:
    static std::vector<std::string> generateConstFrom(
        unsigned long long target, 
        std::vector<std::pair<std::string, unsigned long long>> values);

private:
    static std::vector<std::string> _result;

    static int getCostWithIncAndAdd(unsigned long long target);
    static void generateWithIncAndAdd(unsigned long long target, std::string name);

};
