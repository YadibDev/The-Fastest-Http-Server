#include "clsBody.hpp"

int main()
{
    PollOfClient test;
    stPollRequest stData;
    stData.io_chunk = test.io_chunk;
    stData.known_headers = test.known_headers;
    stData.unknown_headers = test.unknown_headers;
    stData.request_metadata = test.request_metadata;
    stData.read_body_ptr = &test.read_body;
    clsBody bodyParser(stData);

    uint16_t offset = 0;

    stData.known_headers[HttpTables::H_TRANSFER_ENCODING].Hash = 1;

    int fd = open("/home/yadib/goinfre/The-Fastest-Http-Server/Parser/ParseRequest/Request/video.txt", O_RDONLY | O_CREAT);

    int len = 30281;
    const size_t maxBody = 100000;
    while (1)
    {
        srand(time(NULL));
        int size = read(fd, &stData.io_chunk[offset], SIZE_BUFFER - offset);
        if (size == 0)
        {
            std::cout << "file is reades\n\n";
            return 1;
        }
        if (size > 0)
        {
            offset += size;
            std::cout << "ofset == > " << offset << std::endl;
        }

        bodyParser.bodyHandler(&offset, maxBody, false, "/home/yadib/goinfre/The-Fastest-Http-Server/Parser/ParseRequest/Request/result2.mp4");

        if (bodyParser.getState() == clsBody::DONE_GOOD)
        {
            std::cout << "clsBody::DONE_GOOD" << std::endl;
            std::cout << bodyParser.getError().getMsgError();
            return 0;
        }
        else if (bodyParser.getState() == clsBody::DONE_WIHTERROR)
        {
            std::cout << "clsBody::DONE_WIHTERROR" << std::endl;
            std::cout << bodyParser.getError().getMsgError() << std::endl;
            ;
            return 1;
        }
        else
            std::cout << "still work" << std::endl;

        len -= size;
    }
}

//   c++ -g main.cpp clsBody.cpp Header.cpp ../../../Utils/HelperFunctions.cpp ../../../Utils/HttpError.cpp HeaderTable.cpp  Utils.cpp