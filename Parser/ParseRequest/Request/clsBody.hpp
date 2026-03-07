#ifndef CLSBODY_HPP
#define CLSBODY_HPP

#include <iostream>
#include <string>

class clsRequest;

enum bodyPlace
{
    NONE,
    RAM,
    DISK
};

enum whichBoundary
{
    startBoundary,
    endBoundary,
    nothing
};
enum bodySteps
{
    SETTING_VARS,
    READING_HEADERS,
    READING_BODY,
    DONE
};

class clsBody
{
private:
    bodyPlace _bodyLocation;
    bodySteps _state;
    std::string _fileName;
    int fd;
    std::string _bodyBuffer;
    bool        _isError;
    ssize_t     _Length;
    bool _isMultiPart;
    bool _isChunk;
public:
    // geters
    const std::string &getFileName() const;
    const std::string &getBodyInRam() const;
    const bodyPlace &getBodyLocation() const; // is in ram or disk
    const bool &getIsError() const;
    const bodySteps &getState() const;
    // methods
    bool thereIsAline(const std::string &buffer, size_t &start, char c = '\n', char after = '\r');
    void bodyHandler(const std::string &buffer, clsRequest &request);
    void normalBody(const char *, ssize_t nBytes);
    void Reset();
    // helpers
    whichBoundary checkBoundary(std::string &bound, std::string &data);
};

#endif