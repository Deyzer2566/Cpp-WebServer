#pragma once
#include <vector>
#include <string>
#include <array>
unsigned long leftrotate(unsigned long x, unsigned long offset);

std::string base64(std::vector<unsigned char>bytes);

std::array<unsigned long, 5> SHA1(std::string str);

std::array<unsigned long, 5> SHA1(std::vector<unsigned char> vect);
