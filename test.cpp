#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
int main()
{
    int fd = open("file.txt", O_RDONLY, 0777);
    
    std::cout << fd << std::endl;

    char buf[2];
    std::cout << recv(fd, buf, 2, 0) << std::endl;;
    std::cout << read(fd, buf, 2) << std::endl;;
    std::cout << read(fd, buf, 2) << std::endl;;
}