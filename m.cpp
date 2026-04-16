#include <iostream>

int main()
{
    int a = 5;
    int *b = &a;
    int &c = *b;


    c = 1000;
    std::cout << a << std::endl;
}