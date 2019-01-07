#include <iostream>
#include <functional>
#include <vector>
#include <climits>
#include <cmath>



int main()
{
    int f[100];
    int s[100];
    int i[100];

    int n;
    int k;
    int l;

    n = 2;
    f[0] = 1;
    s[0] = 1;
    i[0] = 0;

    for(int j = 1; j <= n; j++)
    {
        k = std::max(j - 1, 0);
        l = std::max(k - 1, 0);

	    i[j] =i[k]+1;
	    f[j] =f[k]+f[l];
        s[j] =s[k]*i[j];

        std::cout<<i[j]<<std::endl;
        std::cout<<f[j]<<std::endl;
        std::cout<<s[j]<<std::endl;

    }

    // std::cout<<s[n]<<std::endl;
    // std::cout<<f[n]<<std::endl;

    return 0;
}