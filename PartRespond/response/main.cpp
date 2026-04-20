
#include "../mainprocess/Webserv.hpp"
int main()
{
    RequestHandler _DataRequest;
    clsResponse Response(_DataRequest);
    std::cout << sizeof(clsResponse)<< std::endl;
}