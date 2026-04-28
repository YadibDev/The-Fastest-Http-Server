#include <unistd.h>
#include "Utils/HelperFunctions.hpp"
unsigned enum  hh{LL, kk, BB};
int main() {
   
    // int i = 100;
    unsigned int Arr[10];
    HelperFunctions::ft_memset(&Arr, -1, sizeof(Arr));
    for (int i = 0; i < 10; i++)
        std::cout << Arr[i] << std::endl;
    // If execve returns, it means an error occurred
    return 0;
}