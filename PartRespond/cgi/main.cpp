#include "clsCGI.hpp"

int main(int arg, char**argc, char **env)
{
    clsCGI CGI;
    std::string Str;
    Str.resize(65000);
    int Pos;
    int fd = CGI.RunCGI();
    if (fd == -1)
    {
        std::cout << "error CGI" << std::endl;
        return 1;
    }
    CGI.GetclsParseOutCGI().SetPipe_Fd(fd);
    if ((Pos = read(fd,&Str[0], 65000)) == -1)
        return 1;
    // std::cout<< Str << std::endl; 
    int status;
    while(Pos)
    {
        std::cout<< Str << std::endl; 
        if ((Pos = read(fd,&Str[0], 65000)) == -1)
        return 1;
    }
    int exit_code = waitpid(CGI.GetPid(), &status, WNOHANG);
    std::cout << exit_code<< " "<< CGI.GetPid()<< std::endl;
    CGI.GetclsParseOutCGI().ReceivingData(Str);
    std::cout<< "<---Header--->" << std::endl; 
    std::cout << CGI.GetclsParseOutCGI().GetHeadersFieldFinal()<< std::endl;
    std::cout << CGI.GetclsParseOutCGI().GetBody() << std::endl;
    std::cout << CGI.GetclsParseOutCGI().GetFaleNameBody() << std::endl;
}