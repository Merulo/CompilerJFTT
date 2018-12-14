#include <iostream>
#include <functional>
#include <vector>
#include <climits>
#include <cmath>

void divide(int a, int b)
{
    int i = 0;
    while(b < a)
    {
        i++;
        b = b + b;
    }
    int w = 1;
    i--;
    b = b / 2;
    a = a - b;
    while(i > 0)
    {
        w = w + w;
        b = b / 2;
        i--;
        if (a >= b)
        {
            w++;
            a = a - b;
        }
    }


}





int main()
{
    for(int i = 0; i < 100; i++)
    {
        for (int j = 0; j < i; j++)
        {
            divide(i, j);
        }
    }

    return 0;
}