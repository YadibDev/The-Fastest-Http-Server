#include <unistd.h>
#include <string.h>
#include <iostream>

enum hh{LL = -22, kk, BB};
int main() {
   
    // int i = 100;
    hh Arr[10];
    memset(&Arr, kk, sizeof(Arr));

	if (Arr[0] == kk)
		std::cout << "Arr[i]" << std::endl;
    for (int i = 0; i < 10; i++)
        std::cout << Arr[i] << std::endl;
    // If execve returns, it means an error occurred
    return 0;
}