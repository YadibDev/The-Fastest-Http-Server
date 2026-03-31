#ifndef UTILS_HPP
#define UTILS_HPP

#include "HttpTypes.hpp"
#include <iostream>
#include <cstring>

bool isSpace(char c);
void skipSpaceLastIndex(const char *buffer, uint16_t size, uint16_t &offset);
int compare_view(const s_view& view, const char* base_buffer, const std::string& other);
stPollRequest makeRequest(PollOfClient &client);
void print_view(const char *buffer, const s_view &view);

#endif