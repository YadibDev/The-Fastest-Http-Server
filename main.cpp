#include <stdio.h>

int main()
{

    int a = 1337;
    long long addr = reinterpret_cast<long long>(&a);
    // int *p = addr;
    

    printf("%lld\n", addr);
}