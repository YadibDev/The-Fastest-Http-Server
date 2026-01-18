#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>

int main()
{
    int fds[2];

    pipe(fds);
    
    std::cout << fds[0] << std::endl;
    std::cout << fds[1] << std::endl;
    open("abcd", O_CREAT);
}