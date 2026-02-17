/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperString.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 20:34:41 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/15 20:38:56 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef HEALPER_STRING_HPP
# define HEALPER_STRING_HPP
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string>
#include <ctime>

std::string ConvertStringToLower(std::string &Str);
bool Ischar(const std::string &Sep, char C);
int SkeeSep(const std::string &Str, const std::string &Sep);
int SkeeSep(const std::string &Str, char Sep);
std::vector<std::string> Split(std::string Str, char Sep , int TimesSplit);
int ReadData(int FD, std::string &Data, ssize_t Size);
std::string GetNextLine(int FD, std::string &BigData, ssize_t Size);
std::string  TrimStr(std::string Str,const std::string &Sep);
bool IsStringDigit(const std::string &StringDigit);
bool Iswhaitspace(char C);bool CmpStr(const std::string &Str1, const std::string &Str2, bool Switch);
size_t FindCRLF(const std::string &Str, const std::string &CRLF);
std::string	Convert_Hex(const std::string &Str, int Num);
void StoredType(std::map<std::string, std::string> &StoredType, const std::string &FileName);
std::string DateTime();

#endif