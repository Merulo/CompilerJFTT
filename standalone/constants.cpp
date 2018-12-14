#include <iostream>
#include <functional>
#include <vector>
#include <climits>
#include <cmath>


int getCostWithIncAndAdd(long long target)
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

std::pair<int, int> getCostWithCasualDec(long long target)
{
    long long  nextPowerOfTwo = pow(2, ceil(log(target)/log(2)));
    if (nextPowerOfTwo == target || nextPowerOfTwo < 0)
    {
        return {INT_MAX, 0};
    }

    std::pair<int, int> result = {getCostWithIncAndAdd(target + 1) + 1, 1};
    for(long long i = 2; i <= nextPowerOfTwo - target; i++)
    {
        int newCost = getCostWithIncAndAdd(target + i) + i;
        if (newCost < result.first)
        {
            result.first = newCost;
            result.second = i;
        }
    }
    return result;
}


void findOptimal(const std::vector<long long>& numbers)
{
    int costINC = INT_MAX;
    if (numbers.size() == 1)
    {
        costINC = getCostWithIncAndAdd(numbers[0]);
        auto costDecPair = getCostWithCasualDec(numbers[0]);

        std::cout<<costINC<<std::endl;
        std::cout<<costDecPair.first<<" with " <<costDecPair.second<<" times DEC" <<std::endl;
    }
    return;
}






int main()
{
    std::cout << "Get number to generate: ";
    long long value;
    std::cin >> value;
    findOptimal({value});

    return 0;
}