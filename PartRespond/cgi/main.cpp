#include "clsCGI.hpp"

int main()
{
    clsCGI CGI;
    std::string Str;
    Str.resize(65000);
    int Pos;
    int fd = -1;
    size_t sum = 0;
    fd = CGI.RunCGI();
    if (fd == -1)
    {
        std::cout << "error CGI" << std::endl;
        return 1;
    }
    CGI.GetclsParseOutCGI().SetPipe_Fd(fd);
    if ((Pos = read(fd,&Str[0], 10000)) == -1)
        return 1;
    sum++;
    Str.resize(Pos);
    if (waitpid(CGI.GetPid(), NULL, WNOHANG) > 0)
        CGI.GetclsParseOutCGI().SetProcessIsFinish(true);
    CGI.GetclsParseOutCGI().ReceivingData(Str);
    sum++;
    int status;
    while(Pos)
    {
        sum++;
        Str.resize(10000);
        if ((Pos = read(fd,&Str[0], 10000)) == -1)
        {
            perror("error read");
            break;
        }
        Str.resize(Pos);
        CGI.GetclsParseOutCGI().ReceivingData(Str);
        
    }
    int exit_code = -2;
    while((exit_code = waitpid(CGI.GetPid(), NULL, WNOHANG)) != CGI.GetPid()) {};
    CGI.GetclsParseOutCGI().SetProcessIsFinish(true);
    CGI.GetclsParseOutCGI().ReceivingData(Str);
    // std::cout << "Return Waitpid :"<< exit_code<< " Id process : "<< CGI.GetPid()<< " Exit status : " << WEXITSTATUS(status) << " counter : "<< sum << std::endl;
    // std::cout<< "<---Header--->" << std::endl;
    std::cout << CGI.GetclsParseOutCGI().GetHeadersFieldFinal()<< std::endl;
    std::cout << CGI.GetclsParseOutCGI().GetBody() << std::endl;
    std::cout << CGI.GetclsParseOutCGI().GetFileNameBody() << std::endl;
}