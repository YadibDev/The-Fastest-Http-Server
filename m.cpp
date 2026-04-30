#include <iostream>
#include <string>
#include <vector>


class A {
public:               // جعل المنشئ والعناصر عامة لتتمكن main من الوصول إليها
    std::string str;
    char *ptr;

    A() {
        str = "Hello";
        ptr = &str[0];  // مؤشر إلى أول حرف في السلسلة
    }
};

void    d(std::vector<A> &vA)
{
    A a;
    std::cout << (a.ptr) << std::endl;
    vA.push_back(a);
}

int main() {

    std::vector<A> vA;
    d(vA);
    std::cout << (vA[0].ptr) << std::endl;

    return 0;
}