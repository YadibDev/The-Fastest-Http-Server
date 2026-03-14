#include "clsCGI.hpp"

int main()
{
    clsCGI CGI;
    std::string Str;
    Str.resize(65000);
    int fd = CGI.RunCGI();
    if (fd == -1)
        return 1;
    CGI.GetclsParseOutCGI().SetPipe_Fd(fd);
    if (read(fd,&Str[0], 65000) == -1)
        return 1;
    std::cout <<"<---out pipe--->"<<std::endl;
    std::cout << Str<<std::endl;
    std::cout <<"<---out pipe--->"<<std::endl;
    CGI.GetclsParseOutCGI().ReceivingData(Str);
    std::cout<< "<---Header--->" << std::endl; 
    std::cout << CGI.GetclsParseOutCGI().GetHeadersFieldFinal()<< std::endl;
    std::cout << CGI.GetclsParseOutCGI().GetBody() << std::endl;
    std::cout << CGI.GetclsParseOutCGI().GetFaleNameBody() << std::endl;
}