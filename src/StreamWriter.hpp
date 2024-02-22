#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <bit>
#include <span>
#include <algorithm>

namespace nbt {
    class StreamWriter {
      public:
        template <typename T>
        void write(T val) {
            constexpr auto len = sizeof(T);
            auto start = (uint8_t*)&val;
            auto end = start + len;
            if constexpr (len > 1 && std::endian::native == std::endian::little) {
                std::reverse(start, end);
            }
            m_bytes.insert(m_bytes.end(), start, end);
        }

        void writeRaw(std::vector<uint8_t> bytes);
        void writeRaw(std::initializer_list<uint8_t> bytes);
        void writeRaw(std::span<uint8_t> data);
        void writeStr(const std::string& str);

        inline const std::vector<uint8_t>& getBytes() const { return m_bytes; }
        inline void clear() { m_bytes.clear(); }

        template <typename T>
        StreamWriter& operator<<(T val) {
            write(val);
            return *this;
        }

      private:
        std::vector<uint8_t> m_bytes;
    };
} // namespace nbt