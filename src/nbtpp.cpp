#include "nbtpp.hpp"

#include <fstream>
#ifdef nbtpp_zlib
#include <zlib.h>
#endif

namespace nbt {
    SimpleValue::SimpleValue(SimpleType value) : m_value(value) {}

    void SimpleValue::serialize(StreamWriter& writer) const {
        auto id = getID();
        switch (id) {
        case TagID::Byte: {
            writer << std::get<char>(m_value);
        } break;
        case TagID::Short: {
            writer << std::get<short>(m_value);
        } break;
        case TagID::Int: {
            writer << std::get<int>(m_value);
        } break;
        case TagID::Long: {
            writer << std::get<long long>(m_value);
        } break;
        case TagID::Float: {
            writer << std::get<float>(m_value);
        } break;
        case TagID::Double: {
            writer << std::get<double>(m_value);
        } break;
        case TagID::String: {
            writer.writeStr(std::get<std::string>(m_value));
        } break;
        default: {
            std::println("[nbtpp] Invalid type {} for SimpleValue!", static_cast<int>(id));
        } break;
        }
    }

    void SimpleValue::deserialize(StreamReader& reader, TagID id) {
        switch (id) {
        case TagID::Byte: {
            m_value = reader.read<char>();
        } break;
        case TagID::Short: {
            m_value = reader.read<short>();
        } break;
        case TagID::Int: {
            m_value = reader.read<int>();
        } break;
        case TagID::Long: {
            m_value = reader.read<long long>();
        } break;
        case TagID::Float: {
            m_value = reader.read<float>();
        } break;
        case TagID::Double: {
            m_value = reader.read<double>();
        } break;
        case TagID::String: {
            m_value = reader.readStr();
        } break;
        default: {
            std::println("[nbtpp] Invalid type {} for SimpleValue!", static_cast<int>(id));
        } break;
        }
    }

    TagID SimpleValue::getID() const {
        if (std::holds_alternative<char>(m_value)) {
            return TagID::Byte;
        } else if (std::holds_alternative<short>(m_value)) {
            return TagID::Short;
        } else if (std::holds_alternative<int>(m_value)) {
            return TagID::Int;
        } else if (std::holds_alternative<long long>(m_value)) {
            return TagID::Long;
        } else if (std::holds_alternative<float>(m_value)) {
            return TagID::Float;
        } else if (std::holds_alternative<double>(m_value)) {
            return TagID::Double;
        } else if (std::holds_alternative<std::string>(m_value)) {
            return TagID::String;
        } else {
            std::println("[nbtpp] Unknown type of a SimpleValue!");
            return TagID::None;
        }
    }

    ListValue::ListValue(TagID itemsID) : m_itemsID(itemsID) {}

    void ListValue::serialize(StreamWriter& writer) const {
        writer << m_itemsID << static_cast<unsigned int>(m_items.size());
        for (const auto& val : m_items) {
            auto valID = val->getID();
            if (valID == m_itemsID) {
                val->serialize(writer);
            } else {
                std::println("[nbtpp] Failed to serialize a value (id {}) of the ListValue (should be {})",
                             static_cast<int>(valID), static_cast<int>(m_itemsID));
            }
        }
    }

    void ListValue::deserialize(StreamReader& reader, TagID id) {
        reader >> m_itemsID;
        auto len = reader.read<unsigned int>();
        m_items.clear();
        m_items.resize(len);
        for (auto& val : m_items) {
            val = valueForID(reader, m_itemsID);
        }
    }

    void ListValue::appendValues(std::initializer_list<std::shared_ptr<Value>> values) {
        m_items.insert(m_items.end(), values);
    }

    void CompoundValue::serialize(StreamWriter& writer) const {
        for (const auto& [name, val] : m_items) {
            writer << val->getID();
            writer.writeStr(name);
            val->serialize(writer);
        }
        writer << TagID::End;
    }

    void CompoundValue::deserialize(StreamReader& reader, TagID id) {
        while (true) {
            auto tag = reader.read<TagID>();
            if (tag == TagID::End) {
                break;
            }

            auto name = reader.readStr();
            auto value = valueForID(reader, tag);
            m_items[name] = value;
        }
    }

    std::shared_ptr<Value> valueForID(StreamReader& reader, TagID id) {
        switch (id) {
        case TagID::Byte:
        case TagID::Short:
        case TagID::Int:
        case TagID::Long:
        case TagID::Float:
        case TagID::Double:
        case TagID::String: {
            auto val = std::make_shared<SimpleValue>();
            val->deserialize(reader, id);
            return val;
        } break;
        case TagID::List: {
            auto val = std::make_shared<ListValue>(TagID::None);
            val->deserialize(reader, id);
            return val;
        } break;
        case TagID::Compound: {
            auto val = std::make_shared<CompoundValue>();
            val->deserialize(reader, id);
            return val;
        } break;
        case TagID::IntArray: {
            auto val = std::make_shared<ArrayValue<int>>();
            val->deserialize(reader, id);
            return val;
        } break;
        case TagID::ByteArray: {
            auto val = std::make_shared<ArrayValue<char>>();
            val->deserialize(reader, id);
            return val;
        } break;
        case TagID::LongArray: {
            auto val = std::make_shared<ArrayValue<long long>>();
            val->deserialize(reader, id);
            return val;
        } break;
        default: {
            std::println("[nbtpp] Invalid tag {} in valueForID", static_cast<int>(id));
            return nullptr;
        } break;
        }
    }

    CompoundValue loadFromBytes(std::span<uint8_t> bytes) {
        if (!bytes.size() || !bytes.data()) {
            throw std::runtime_error("Invalid input data");
        }

        CompoundValue val;
        auto r = StreamReader(bytes);
        r.skip(3); // 0x0A 0x00 0x00: root compound tag which is not closed for some reason
        val.deserialize(r, TagID::Compound);
        return val;
    }

    CompoundValue loadFromFile(const std::string& path) {
        std::ifstream fileStream(path, std::ios::binary);
        if (!fileStream) {
            throw std::runtime_error(std::format("Failed to open file \"{}\"", path));
        }

        auto beg = fileStream.tellg();
        fileStream.seekg(0, std::ios::end);
        auto len = fileStream.tellg() - beg;
        fileStream.seekg(0, std::ios::beg);

        auto buf = new char[len];
        fileStream.read(buf, len);

        fileStream.close();

        CompoundValue val;
        auto r = StreamReader({(uint8_t*)buf, static_cast<size_t>(len)});
        r.skip(3); // 0x0A 0x00 0x00: root compound tag which is not closed for some reason
        val.deserialize(r, TagID::Compound);

        delete[] buf;

        return val;
    }

    CompoundValue loadFromCompressedFile(const std::string& path) {
#ifdef nbtpp_zlib
        std::ifstream fileStream(path, std::ios::binary);
        if (!fileStream)
            throw std::runtime_error("Failed to open input file");

        auto beg = fileStream.tellg();
        fileStream.seekg(0, std::ios::end);
        auto len = fileStream.tellg() - beg;
        fileStream.seekg(0, std::ios::beg);

        auto source = std::make_unique<char[]>(len);
        fileStream.read(source.get(), len);
        fileStream.close();

        z_stream strm {};
        strm.next_in = (Bytef*)source.get();
        strm.avail_in = len;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;

        bool done = false;

        if (inflateInit2(&strm, (16 + MAX_WBITS)) != Z_OK)
            throw std::runtime_error("Failed to create a zlib stream");

        std::string uncomp;

        while (!done) {
            auto avail = std::min(strm.avail_in, 1024u * 8u);
            auto prevSize = uncomp.size();
            uncomp.resize(prevSize + avail);

            strm.next_out = (Bytef*)(uncomp.data() + prevSize);
            strm.avail_out = avail;

            auto code = inflate(&strm, Z_SYNC_FLUSH);
            if (code == Z_STREAM_END)
                done = true;
            else if (code != Z_OK)
                throw std::runtime_error(std::format("Zlib error while decompressing (code {})", code));
        }

        inflateEnd(&strm);

        return loadFromBytes({(uint8_t*)uncomp.data(), uncomp.size()});
#else
        throw std::runtime_error("Compile nbtpp with zlib!");
#endif
    }

    void saveToFile(const std::string& path, Value* val) {
        auto f = std::ofstream(path, std::ios::binary | std::ios::trunc);
        if (!f) {
            throw std::runtime_error(std::format("Failed to open file \"{}\" for saving", path));
        }

        auto w = StreamWriter();
        w.writeRaw({0x0A, 0x00, 0x00}); // root compound tag which is not closed for some reason
        val->serialize(w);

        const auto& bytes = w.getBytes();
        f.write((char*)bytes.data(), bytes.size());
        f.close();
    }

    void saveToCompressedFile(const std::string& path, Value* val) {
#ifdef nbtpp_zlib
        auto f = std::ofstream(path, std::ios::binary | std::ios::trunc);
        if (!f) {
            throw std::runtime_error("Failed to open output file for saving");
        }

        auto w = StreamWriter();
        w.writeRaw({0x0A, 0x00, 0x00}); // root compound tag which is not closed for some reason
        val->serialize(w);
        const auto& bytes = w.getBytes();

        auto srclen = bytes.size();
        auto dstlen = compressBound(srclen);

        auto dst = std::make_unique<char[]>(dstlen);
        if (!dst)
            throw std::runtime_error("Failed to create an output buffer");

        z_stream strm {};
        strm.next_in = (Bytef*)bytes.data();
        strm.avail_in = bytes.size();
        strm.next_out = (Bytef*)dst.get();
        strm.avail_out = dstlen;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;

        if (deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
            throw std::runtime_error("Failed to create a zlib stream");
        }

        deflate(&strm, Z_FINISH);

        f.write(dst.get(), strm.total_out);
        f.close();

        deflateEnd(&strm);
#else
        throw std::runtime_error("Compile nbtpp with zlib!");
#endif
    }

    std::vector<uint8_t> saveToBytes(Value* val) {
        auto w = StreamWriter();
        w.writeRaw({0x0A, 0x00, 0x00}); // root compound tag which is not closed for some reason
        val->serialize(w);
        return w.getBytes();
    }
} // namespace nbt
