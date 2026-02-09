#include "Request.hpp"

clsRequest::Request() : _state(READING_LINE) {}
    
void clsRequest::parse(const std::string& rawData);
    
bool clsRequest::isCompleted() const { return _state == COMPLETED; }