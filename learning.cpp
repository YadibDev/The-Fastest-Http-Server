#include <string>
#include <cstring>
#include <iostream>

enum whichBoundary
{
    startBoundary,
    endBoundary,
    nothing
};

whichBoundary checkBoundary(std::string &bound, std::string &data)
{
    if (data.size() < bound.size() + 2)
        return nothing;
    else if (std::strncmp(data.c_str(), bound.c_str(), bound.size()) == 0)
    {
        size_t ofset = bound.size();
        if (std::strncmp(&data[ofset], "\r\n", 2) == 0)
            return startBoundary;
        else if (data.size() >= bound.size() + 4)
        {
            if (std::strncmp(&data[ofset], "--\r\n", 4) == 0)
                return endBoundary;
        }
    }
    return nothing;
}
/*
    first thing:
        find first boundary
        if doesn't exist skipp all data until \r\n
    second thing:
        i found boundary start
        loop though this:
            read headers store them
            move to the next section 
            read body until find boundary or end boundary
            if find boundary or end boundary create file
                if error free all data related to this and remove files and send bad request
*/

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
int main()
{
    char arr[100] = "tmp/arrXXXXXX";
    int fd = mkstemp(arr);
    
    std::cout << arr << std::endl;
}
// int main()
// {
//     std::string boundary = "--"
//                            "----MyBoundary123";
//     std::string request =
//         "------MyBoundary123\r\n"
//         "Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n"
//         "Content-Type: text/plain\r\n"
//         "\r\n"
//         "Hello from multipart file!\n"
//         "This is line 2.\r\n"
//         "------MyBoundary123--\r\n";
//     whichBoundary boundResult = checkBoundary(boundary, request);

//     if (boundResult == nothing)
//         std::cout << "NOthing\n";
//     else if (boundResult == endBoundary)
//         std::cout << "end boundary\n";
//     else if (boundResult == startBoundary)
//         std::cout << "start boundary\n";
// }