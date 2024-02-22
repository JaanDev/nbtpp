#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include <array>
#include <memory>
#include <print>

#include "StreamReader.hpp"
#include "StreamWriter.hpp"

namespace nbt {
    enum class TagID : uint8_t {
        End = 0,
        Byte,
        Short,
        Int,
        Long,
        Float,
        Double,
        ByteArray,
        String,
        List,
        Compound,
        IntArray,
        LongArray,
        None = 0xFF // custom
    };

    class Value {
      public:
        virtual void serialize(StreamWriter& writer) const = 0;
        virtual void deserialize(StreamReader& reader, TagID id) = 0;

        virtual TagID getID() const { return TagID::None; }
    };

    class SimpleValue : public Value {
      public:
        using SimpleType = std::variant<char, short, int, long long, float, double, std::string>;

        SimpleValue() {};
        SimpleValue(SimpleType value);

        virtual void serialize(StreamWriter& writer) const override;
        virtual void deserialize(StreamReader& reader, TagID id) override;
        virtual TagID getID() const override;

        inline const SimpleType& get() const { return m_value; }
        inline void set(const SimpleType& val) { m_value = val; }

      protected:
        SimpleType m_value;
    };

    class ListValue : public Value {
      public:
        ListValue(TagID itemsID);

        virtual void serialize(StreamWriter& writer) const override;
        virtual void deserialize(StreamReader& reader, TagID id) override;
        virtual TagID getID() const override { return TagID::List; }
        inline TagID getItemsID() const { return m_itemsID; }

        inline std::vector<std::shared_ptr<Value>>& getItems() { return m_items; }
        void appendValues(std::initializer_list<std::shared_ptr<Value>> values);

      protected:
        std::vector<std::shared_ptr<Value>> m_items;
        TagID m_itemsID;
    };

    template <typename T>
    class ArrayValue : public Value {
        static_assert(std::is_same_v<T, char> || std::is_same_v<T, int> || std::is_same_v<T, long long>,
                      "ArrayValue can only accept char, int or long long!");

      public:
        ArrayValue() {}
        ArrayValue(std::initializer_list<T> values) : m_items(values) {}

        virtual void serialize(StreamWriter& writer) const override;
        virtual void deserialize(StreamReader& reader, TagID id) override;
        virtual TagID getID() const override;

        inline std::vector<T>& getItems() { return m_items; }

      protected:
        std::vector<T> m_items;
    };

    class CompoundValue : public Value {
      public:
        using CompoundValueType = std::unordered_map<std::string, std::shared_ptr<Value>>;

        virtual void serialize(StreamWriter& writer) const override;
        virtual void deserialize(StreamReader& reader, TagID id) override;
        virtual TagID getID() const override { return TagID::Compound; }

        inline CompoundValueType& getItems() { return m_items; }

      protected:
        CompoundValueType m_items;
    };

    using ByteArrayValue = ArrayValue<char>;
    using IntArrayValue = ArrayValue<int>;
    using LongArrayValue = ArrayValue<long long>;

    using ValueType = std::variant<SimpleValue, ByteArrayValue, ListValue, CompoundValue, IntArrayValue, LongArrayValue>;

    template <typename T>
    inline void ArrayValue<T>::serialize(StreamWriter& writer) const {
        writer << static_cast<unsigned int>(m_items.size());
        if (getID() == TagID::ByteArray) {
            writer.writeRaw({(uint8_t*)m_items.data(), m_items.size()});
        } else {
            for (const auto& val : m_items) {
                writer << val;
            }
        }
    }

    template <typename T>
    inline void ArrayValue<T>::deserialize(StreamReader& reader, TagID id) {
        auto size = reader.read<unsigned int>();
        m_items.resize(size);
        if (id == TagID::ByteArray) {
            reader.read({(uint8_t*)m_items.data(), size});
        } else {
            for (auto& val : m_items) {
                reader >> val;
            }
        }
    }

    template <typename T>
    inline TagID ArrayValue<T>::getID() const {
        if (std::is_same_v<char, T>) {
            return TagID::ByteArray;
        } else if (std::is_same_v<int, T>) {
            return TagID::IntArray;
        } else if (std::is_same_v<long long, T>) {
            return TagID::LongArray;
        } else {
            return TagID::None;
        }
    }

    std::shared_ptr<Value> valueForID(StreamReader& reader, TagID id);

    CompoundValue loadFromBytes(std::span<uint8_t> bytes);
    CompoundValue loadFromFile(const std::string& path);
    CompoundValue loadFromCompressedFile(const std::string& path);

    void saveToFile(const std::string& path, Value* val);
    void saveToCompressedFile(const std::string& path, Value* val);
    std::vector<uint8_t> saveToBytes(Value* val);
} // namespace nbt