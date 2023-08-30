#pragma once
static bool isHostBigEndian()
{
    int x = 1;
    return !(*(char*)&x == 1);
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
            *((unsigned char*)&ret + sizeof(T) - 1 - i) = *((unsigned char*)&value + i);
        return ret;
    }
}
