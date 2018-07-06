#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_HASH_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_HASH_HPP

#pragma once

#include <atlas/core/Macros.hpp>

#include <cinttypes>
#include <limits>
#include <emmintrin.h>

namespace std
{
    using uint128_t = __m128i;
}


namespace bsoid
{
    namespace polygonizer
    {
        template <typename T>
        struct BsoidHash
        {
            static constexpr T bits = std::numeric_limits<T>::digits / 3;
            static constexpr T mask = ~(static_cast<T>(0) << bits);
            static constexpr T hash(T x, T y, T z)
            {
                return (((x & mask) << bits | (y & mask)) << bits | (z & mask));
            }
        };

        using BsoidHash64 = BsoidHash<std::uint64_t>;
        using BsoidHash128 = BsoidHash<std::uint128_t>;
    }
}

#endif