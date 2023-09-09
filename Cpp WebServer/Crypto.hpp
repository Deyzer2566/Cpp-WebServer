#pragma once
#include <vector>
#include <string>
#include <array>
#include <cstdint>
uint32_t leftrotate(uint32_t x, uint32_t offset);

std::string base64(std::vector<uint8_t>bytes);

std::array<uint32_t, 5> SHA1(std::string str);

std::array<uint32_t, 5> SHA1(std::vector<uint8_t> vect);
