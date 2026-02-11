#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>

int hexCharToDec(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

long hexToDec(const std::string& hex)
{
    long decimalValue = 0;
    
    for (std::size_t i = 0; i < hex.length(); ++i)
    {
        int digit = hexCharToDec(hex[i]);
        
        if (digit == -1)
            return -1;
        
        decimalValue = (decimalValue << 4) | digit;
    }
    
    return decimalValue;
}

int main()
{
    int pipe_fds[2];
    std::cout << hexToDec("5K") << std::endl;
}