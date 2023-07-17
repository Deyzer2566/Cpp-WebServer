#pragma once
namespace Endian
{
	bool isHostBigEndian()
	{
		int x = 1;
		return !(*(char*)&x == 1);
	}
	template <typename T>
	T swapBEH(T value)
	{
		if (isHostBigEndian())
			return value;
		else
		{
			T ret = 0;
			for (size_t i = 0; i < sizeof(T); i++)
				*((unsigned char*)&ret + sizeof(T) - 1 - i) = *((unsigned char*)&value + i);
			return ret;
		}
	}
	/*unsigned short swapBEH(unsigned short value)
	{
		if (isHostBigEndian())
			return value;
		else
			return (value << 8) | (value >> 8);
	}
	unsigned long swapBEH (unsigned long value)
	{
		if (isHostBigEndian())
			return value;
		else
			return (value << 24) | (value<<8 & 0xff0000) | (value>>8 & 0xff00) | (value>>24 & 0xff);
	}
	unsigned long long swapBEH(unsigned long long value)
	{
		if (isHostBigEndian())
			return value;
		else
			return (value << 56 & 0xff00000000000000) | (value << 40 & 0xff000000000000) | (value << 32 & 0xff0000000000) | (value << 24 & 0xff00000000) |
			(value >> 56 & 0xff) | (value >> 40 & 0xff00) | (value >> 32 & 0xff0000) | (value >> 24 & 0xff000000);
	}*/
}