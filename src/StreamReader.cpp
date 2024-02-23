#include "StreamReader.hpp"

#include <cstring>

namespace nbt {
    StreamReader::StreamReader(std::span<uint8_t> data) : m_data(data.data()), m_len(data.size()) {}

    void StreamReader::read(std::span<uint8_t> data) {
        auto len = data.size();
        if (m_len < len) {
            std::cerr << std::format("[nbtpp] Failed to read {} bytes from the buffer (only {} left)!", len, m_len) << std::endl;
            return;
        }

        memcpy(data.data(), m_data, len);
        m_len -= len;
        m_data += len;
    }

    std::string StreamReader::readStr() {
        auto len = read<uint16_t>();

        if (len == 0) {
            return "";
        }

        auto bytes = new char[len + 1];
        bytes[len] = 0;
        read({(uint8_t*)bytes, len});
        auto str = std::string(bytes, len);
        delete[] bytes;
        return str;
    }

    void StreamReader::skip(size_t len) {
        if (len > m_len) {
            std::cerr << std::format("[nbtpp] Failed to skip {} bytes (only {} left)!", len, m_len) << std::endl;
            return;
        }

        m_len -= len;
        m_data += len;
    }
} // namespace nbt