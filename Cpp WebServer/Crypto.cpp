#include "Crypto.hpp"
#include <vector>
#include <string>
#include <iostream>
unsigned long leftrotate(unsigned long x, unsigned long offset)
{
	return (x << offset) | (x >> (32 - offset));
}
template<typename T>
T toBigEndian(T num)
{
	T p = 1;
	if (*(char*)&p == 0)
		return num;
	for (size_t i = 0; i < sizeof(T)/2; i++)
	{
		char temp = ((char*)&num)[i];
		((char*)&num)[i] = ((char*)&num)[sizeof(T) - i-1];
		((char*)&num)[sizeof(T) - i - 1] = temp;
	}
	return num;
}
std::string base64(std::vector<unsigned char>bytes)
{
	std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int cr = 0;
	while (bytes.size() % 3 != 0)
	{
		bytes.push_back(0);
		cr++;
	}
	std::string ret;
	for (size_t i = 0; i<bytes.size(); i += 3)
	{
		unsigned char splited[4]{};
		int LastBits = cr*(i == bytes.size() - 3);
		for (int j = 0; j<24 - 6 * LastBits; j++)
		{
			splited[j / 6] |= (((bytes[i + j / 8] >> (7 - j % 8)) & 1) << (5 - j % 6));
		}
		for (int j = 0; j<4 - LastBits; j++)
		{
			ret += alphabet[splited[j]];
		}
	}
	for (int i = 0; i<cr; i++)
		ret += '=';
	return ret;
}
std::vector<unsigned long> SHA1(std::string str)
{
	return SHA1(std::vector<unsigned char>(str.begin(), str.end()));
}
std::vector<unsigned long> SHA1(std::vector<unsigned char> vect)
{
	size_t oldsize = vect.size();
	size_t size = oldsize;
	oldsize = toBigEndian(oldsize<<3);//==*8
	size = ((vect.size() + sizeof(size_t)) / 64 + 1) * 64 - sizeof(size_t);
	vect.push_back(0x80);
	vect.resize(size,0);
	vect.insert(vect.end(), (unsigned char*)&oldsize, (unsigned char*)&oldsize + sizeof(size_t));
	std::vector<unsigned long> h = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };
	for (size_t i = 0; i < vect.size(); i += 64)
	{
		std::vector<unsigned long> w((unsigned long*)&vect[i], (unsigned long*)(&vect[i]+64));
		for (unsigned long& j : w)
			j = toBigEndian(j);
		w.resize(80,0);
		for (int j = 16; j < 80; j++)
			w[j] = leftrotate((w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16]), 1);
		unsigned long a = h[0],
			b = h[1],
			c = h[2],
			d = h[3],
			e = h[4];
		for (int j = 0; j < 80; j++)
		{
			unsigned long f = 0, k = 0;
			if (j >= 0 && j < 20)
			{
				f = (b & c) | ((~b) & d);
				k = 0x5A827999;
			}
			else if (j >= 20 && j < 40)
			{
				f = b ^ c ^ d;
				k = 0x6ED9EBA1;
			}
			else if (j >= 40 && j < 60)
			{
				f = (b & c) | (b & d) | (c & d);
				k = 0x8F1BBCDC;
			}
			else if (j >= 60 && j < 80)
			{
				f = b ^ c ^ d;
				k = 0xCA62C1D6;
			}
			unsigned long temp = leftrotate(a, 5) + f + e + k + w[j];
			e = d;
			d = c;
			c = leftrotate(b, 30);
			b = a;
			a = temp;
		}

 		h[0] += a;
		h[1] += b;
		h[2] += c;
		h[3] += d;
		h[4] += e;
	}
	return h;
}
