
#include <iostream>


class B
{
    public:
        B() {std::cout << "say hi" << std::endl;};
        B(const B &other) {std::cout << "say hi" << std::endl;};
        ~B() {std::cout << "Destruct" << std::endl;};
};

#include <vector>


int main()
{
    std::vector<B> allb;


    B test;
    allb.push_back(test);
    allb.push_back(test);

    std::cout << "---------\n" << std::endl;;
    allb.erase(allb.begin());
    allb.erase(allb.begin());
    std::cout << "---------\n";
}