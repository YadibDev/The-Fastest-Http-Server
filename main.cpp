#include <iostream>


class B
{
        public:

    ~B()
    {
        std::cout << "B" << std::endl;
    }
};

class A
{
    B &_b;

    public:
    A(B &b): _b(b)
    {

    }
    ~A()
    {
        std::cout << "A" << std::endl;
    }
};



int main()
{
    B b;
    A a(b);
}