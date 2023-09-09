#include "Crypto.hpp"
#include "Endian.hpp"
uint32_t leftrotate(uint32_t x, uint32_t offset)
{
	return (x << offset) | (x >> (32 - offset));
}
std::string base64(std::vector<uint8_t>bytes)
{
	std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int cr = 0;
	while (bytes.size() % (size_t)3 != (size_t)0)
	{
		bytes.push_back(0);
		cr++;
	}
	std::string ret;
	for (size_t i = 0; i<bytes.size(); i += 3)
	{
		uint8_t splited[4]{};
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
std::array<uint32_t, 5> SHA1(std::string str)
{
    return SHA1(std::vector<uint8_t>(str.begin(),str.end()));
}
std::array<uint32_t, 5> SHA1(std::vector<uint8_t> vect)
{
	size_t oldsize = vect.size();
	size_t size = oldsize;
	oldsize = changeEndianIfNotBigEndian(oldsize<<3);//==*8
	size = ((vect.size() + sizeof(size_t)) / 64 + 1) * 64 - sizeof(size_t);
	vect.push_back(0x80);
	vect.resize(size,0);
	vect.insert(vect.end(), (uint8_t*)&oldsize, (uint8_t*)&oldsize + sizeof(size_t));
	std::array<uint32_t, 5> h = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };
	for (size_t i = 0; i < vect.size(); i += 64)
	{
		std::vector<uint32_t> w((uint32_t*)&vect[i], (uint32_t*)(&vect[i]+64));
		for (uint32_t& j : w)
			j = changeEndianIfNotBigEndian(j);
		w.resize(80,0);
		for (int j = 16; j < 80; j++)
			w[j] = leftrotate((w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16]), 1);
		uint32_t a = h[0],
			b = h[1],
			c = h[2],
			d = h[3],
			e = h[4];
		for (int j = 0; j < 80; j++)
		{
			uint32_t f = 0, k = 0;
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
			uint32_t temp = leftrotate(a, 5) + f + e + k + w[j];
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
