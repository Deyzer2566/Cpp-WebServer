#pragma once
#include <cstdint>
#include <cstddef>
static bool isHostBigEndian()
{
    int16_t x = 1;
    return !(*(int8_t*)&x == 1);
}
template <typename T>
T changeEndianIfNotBigEndian(T value)
{
    if (isHostBigEndian())
        return value;
    else
    {
        T ret = 0;
        for (size_t i = 0; i < sizeof(T); i++)
            *((uint8_t*)&ret + sizeof(T) - 1 - i) = *((uint8_t*)&value + i);
        return ret;
    }
}
