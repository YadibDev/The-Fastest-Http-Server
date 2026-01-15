

#include "ServerSock.hpp"


int main()
{
    srand(time(NULL));
    ServerSock Server;

    vector<unsigned short> randomPorts;

    try
    {
        randomPorts.push_back(8080);
        Server.buildSockets(randomPorts, 0);
    }
    catch (std::exception &e)
    {
        std::cout << COLOR_BOLD << COLOR_RED;
        std::cout << "Error : " << e.what() << std::endl;
        std::cout << COLOR_RESET;
    }

}