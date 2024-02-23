#include "StreamWriter.hpp"

namespace nbt {
    void StreamWriter::writeRaw(std::vector<uint8_t> bytes) {
        m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
    }

    void StreamWriter::writeRaw(std::initializer_list<uint8_t> bytes) {
        m_bytes.insert(m_bytes.end(), bytes);
    }

    void StreamWriter::writeRaw(std::span<uint8_t> data) {
        m_bytes.insert(m_bytes.end(), data.begin(), data.end());
    }

    void StreamWriter::writeStr(const std::string& str) {
        auto len = static_cast<uint16_t>(str.length());
        write(len);
        writeRaw({(uint8_t*)str.c_str(), len});
    }
} // namespace nbt