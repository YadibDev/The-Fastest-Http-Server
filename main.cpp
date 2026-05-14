#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

#include <fcntl.h>

int main()
{
    char buffer[10000];
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;

    address.sin_port = htons(8082);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(0);

    std::cout << connect(fd, (sockaddr *)&address, sizeof(sockaddr_in)) << std::endl;
    // ;
    int buffer_size = 100000; // Set your desired buffer size
    std::cout << "set socket return :" << setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size)) << std::endl;;

    int retrurnValue;
    for (int i = 0; i < 15; i++)
    {
        retrurnValue = send(fd, buffer, 10000, MSG_DONTWAIT | MSG_NOSIGNAL);
        perror("error");
        std::cout << "return : " << retrurnValue << std ::endl;
    }
    exit(1);
    for (int i = 0; i < 10000; i++)
    {

        write(fd, "GET /cgi-bin/script.py HTTP/1.1\r\nHost: hello\r\n\r\n", strlen("GET /cgi-bin/script.py HTTP/1.1\r\nHost: hello\r\n\r\n"));

        // sleep(1);

        std::cout << "RESPOND" << std::endl;
        int r = read(fd, buffer, sizeof(buffer));
        buffer[r] = '\0';
        std::cout << "======================" << std::endl;
        std::cout << buffer << std::endl;
    }

    close(fd);

}