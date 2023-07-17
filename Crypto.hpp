#pragma once
#include <vector>
#include <string>
unsigned long leftrotate(unsigned long x, unsigned long offset);

template<typename T>
T toBigEndian(T num);

std::string base64(std::vector<unsigned char>bytes);

std::vector<unsigned long> SHA1(std::string str);

std::vector<unsigned long> SHA1(std::vector<unsigned char> vect);