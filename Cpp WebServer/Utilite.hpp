#pragma once
#include <string>
#include <vector>
#include <cstdint>
typedef std::vector<uint16_t> Unicode;
bool isStrInt(const std::string s);
std::string Unicode_to_ASCII(Unicode str, uint16_t US, uint8_t AS);
std::vector<std::string> split(std::string str, std::string del);
Unicode UTF_8(std::string in, std::string marker);
unsigned int getByteFromHex(std::string str);
Unicode UTF_8_to_Unicode(std::string in, std::string marker);
