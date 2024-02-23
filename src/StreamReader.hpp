#pragma once
#include <cstdint>
#include <span>
#include <algorithm>
#include <iostream>
#include <format>

namespace nbt {
    class StreamReader {
      public:
        StreamReader(std::span<uint8_t> data);

        template <typename T>
        StreamReader& operator>>(T& other) {
            constexpr auto size = sizeof(other);
            if (m_len < size) {
                std::cerr << std::format("[nbtpp] Failed to read {} bytes from the buffer (only {} left)!", size, m_len)
                          << std::endl;
                return *this;
            }

            other = *((T*)m_data);
            if constexpr (size > 1 && std::endian::native == std::endian::little)
                std::reverse((uint8_t*)&other, (uint8_t*)&other + size);
            m_len -= size;
            m_data += size;

            return *this;
        }

        template <typename T>
        T read() {
            T val;
            *this >> val;
            return val;
        }

        inline size_t len() const { return m_len; }
        void read(std::span<uint8_t> data);
        std::string readStr();
        void skip(size_t len);

      private:
        uint8_t* m_data;
        size_t m_len;
    };
} // namespace nbt