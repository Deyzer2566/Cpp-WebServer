#include "Utilite.hpp"
#include <string>
#include <algorithm>
#include <vector>
#include <climits>
#include <cmath>
typedef std::vector<unsigned short> Unicode;
bool isStrInt(const std::string s)
{
	return !s.empty() && (s.find_first_not_of("0123456789") == s.npos);
}
std::string Unicode_to_ASCII(Unicode str, unsigned short US, unsigned char AS)
{
	std::string ret;
	for (Unicode::iterator it = str.begin(); it != str.end(); it++)
		ret += char(*it - US) + AS;
	return ret;
}
std::vector<std::string> split(std::string str, std::string del)
{
	std::vector<std::string>ret;
	for (size_t a = 0; a != str.npos; a++)
	{
		size_t tt = str.find(del, a);
		if (tt == str.npos)
		{
			ret.push_back(str.substr(a, (str.size() - a)));
			break;
		}
		else
			ret.push_back(str.substr(a, (tt - a)));
		a = tt + (del.length() - 1);
	}
	return ret;
}
Unicode UTF_8(std::string in, std::string marker)
{
	std::transform(in.begin(), in.end(), in.begin(), tolower);
	std::vector<unsigned char> bytes;
	Unicode ret;
	for (size_t a = 0; a < in.npos;)
	{
		size_t f = in.find(marker, a);
		std::string word = in.substr(f, 2);
	}
	for (std::string::iterator it = in.begin(); it != in.end();)
	{
		unsigned char i = (((*(it) >= 'a' && *(it) <= 'f') ? (*(it)-'a' + 10) : (*(it)-'0')) * 16 +
			((*(it + 1) >= 'a' && *(it + 1) <= 'f') ? (*(it + 1) - 'a' + 10) : (*(it + 1) - '0')));
		bytes.push_back(i);
		it += 2;
	}
	for (size_t t = 0; t < bytes.size();)
	{
		size_t l = 0;
		while ((bytes[t] >> (7 - l++)) & 1);
		ret.push_back(0);
		for (size_t i = 0; i <= (7 - l); i++)
			ret.back() |= (bytes[t] & (1 << i));
		for (size_t i = t + 1; i < t + l - 1; i++)
		{
			ret.back() <<= 6;
			ret.back() |= (bytes[i] & 0x3F);
		}
		t += (l>1) ? l - 1 : 1;
	}
	return ret;
}
unsigned int getByteFromHex(std::string str)
{
	unsigned int ret = 0;
	for (size_t i = 0; i < str.size(); i++)
		ret += ((str[i] >= 'a' && str[i] <= 'f') ? (str[i] - 'a' + 10) : (str[i] - '0')) * std::pow(16, str.size() - i - 1);
	return ret;
}
Unicode UTF_8_to_Unicode(std::string in, std::string marker)
{
	Unicode ret;
	size_t i = in.find(marker);
	size_t i1 = 0;
	size_t size = marker.size();
	while (true)
	{
		if (i == in.npos)
			i = in.size();
		if (i - i1 > 0)
			ret.insert(ret.end(), in.begin() + i1, in.begin() + i);
		if (i == in.size())
			break;
		size_t s = 0;
		std::transform(in.begin() + i, in.begin() + i + 2 + size, in.begin() + i, tolower);
		unsigned char m = getByteFromHex(in.substr(i + size, 2));
		while ((m >> (7 - ++s)) & 1);
		unsigned short meaning = m & 0x3F;
		std::transform(in.begin() + i + 3, in.begin() + i + (2 + size) * s, in.begin() + i + (2 + size), tolower);
		for (size_t i2 = i + 2 + size; i2 < i + (2 + size) * s; i2 += (2 + size))
		{
			if (in.find(marker, i2) != i2)
				return{};
			unsigned char m = getByteFromHex(in.substr(i2 + size, 2));
			meaning <<= 6;
			meaning |= m & 0x3F;
		}
		ret.push_back(meaning);
		in.erase(i, s*(2 + size));
		i1 = i;
		i = in.find(marker);
	}
	return ret;
}
