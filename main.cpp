#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
    int pipe_fds[2];
    pipe(pipe_fds);

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    if (getsockname(pipe_fds[0], (struct sockaddr *)&addr, &len) == -1)
    {
        // perror("getsockname");
        // Will print: "getsockname: Socket operation on non-socket"
        // printf("errno = %d (ENOTSOCK = %d)\n", errno, ENOTSOCK);
    }
}