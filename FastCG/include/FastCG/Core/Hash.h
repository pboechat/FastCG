#ifndef FASTCG_HASH_H
#define FASTCG_HASH_H

#include <cstddef>
#include <cstdint>

namespace FastCG
{
    // source: https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html
    inline void CRC(uint32_t &h, uint32_t ki)
    {
        auto highOrder = h & 0xf8000000; // extract high-order 5 bits from h
                                         // 0xf8000000 is the hexadecimal representation
                                         //   for the 32-bit number with the first five
                                         //   bits = 1 and the other bits = 0
        h = h << 5;                      // shift h left by 5 bits
        h = h ^ (highOrder >> 27);       // move the highOrder 5 bits to the low-order
                                         //   end and XOR into h
        h = h ^ ki;                      // XOR h and ki
    }

    constexpr uint32_t FNV_PRIME = 0x01000193; // 16777619
    constexpr uint32_t FNV_OFFSET_BASIS = 0x811C9DC5;

    inline uint32_t FNV1a(const uint8_t *bytes, size_t size)
    {
        uint32_t hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < size; ++i)
        {
            hash ^= bytes[i];
            hash *= FNV_PRIME;
        }
        return hash;
    }

    template <typename T>
    struct IdentityHasher
    {
        size_t operator()(const T &rValue) const
        {
            return (size_t)rValue;
        }
    };

    template <typename T>
    struct FNV1aHasher
    {
        size_t operator()(const T &rObj) const
        {
            return (size_t)FNV1a(reinterpret_cast<const uint8_t *>(&rObj), sizeof(T));
        }
    };

}

#endif