#include <bit>
#include <concepts>
#include <cstdint>

template <typename T>
    requires std::unsigned_integral<T>
T to_little_endian(T value) {
    if constexpr (std::endian::native == std::endian::big) {
        return std::byteswap(value);
    }
    return value;
}