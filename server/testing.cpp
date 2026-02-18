

// #include "ServerSock.hpp"
// #include "EpollHandler.hpp"
// #include <ctime>
// int main()
// {
//     // srand(time(NULL));

// #include <cstdlib> // for srand, rand
// #include <ctime>   // for time
// #include <cstdio>  // for perror
// #include <vector>  // ensure vector is available


//     try
//     {
//         randomPorts[0].push_back(8080);
//         randomIp[0].push_back(0);
//         randomPorts[1].push_back(9090);
//         randomIp[1].push_back(2130706433);

//         int size = 2;
//         EpollHandler eventHandler;
//         std::vector<ServerSock> Servers(2, ServerSock());
//         for (int i = 0; i < size; i++)
//         {
//             Servers[i].buildSockets(randomPorts[i], randomIp[i]);
//             if (eventHandler.addServerSockets(Servers[i]))
//                 std::cout << "Server " << i << " created by success\n";
//             std::cout << "Done" << std::endl;
//         }
//         sockaddr_in test;
//         socklen_t t;
//         std::cout << getsockname(*Servers[0].getServerSockets().begin(), (sockaddr *)&test, &t) << std::endl;;
//         std::cout << getpeername(*Servers[0].getServerSockets().begin(), (sockaddr *)&test, &t) << std::endl;;
//         perror("hak :");

//     }
//     catch (std::exception &e)
//     {
//         std::cout << COLOR_BOLD << COLOR_RED;
//         std::cout << "Error : " << e.what() << std::endl;
//         std::cout << COLOR_RESET;
//     }

// }