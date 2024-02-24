#include <nbtpp.hpp>
#include <functional>
#include <iostream>

using namespace nbt;

int main() {
    auto value = CompoundValue();
    auto& items = value.getItems();

    items["someInteger"] = new SimpleValue(2024);
    items["hello"] = new SimpleValue("world");
    items["songs"] = new ListValue(TagID::String, {
        new SimpleValue("In bloom"),
        new SimpleValue("Heart-shaped box"),
        new SimpleValue("About a girl")
    });
    items["some bytes"] = new ArrayValue<char>({0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66});

    auto newVal = new CompoundValue();
    items["another one"] = newVal;
    auto& newItems = newVal->getItems();
    newItems["a byte"] = new SimpleValue((char)103);
    newItems["a short"] = new SimpleValue((short)4096);
    newItems["an int"] = new SimpleValue(999999);
    newItems["a float"] = new SimpleValue(123.45f);
    newItems["a string"] = new SimpleValue("hello");

    try {
        saveToFile("test.nbt", &value);
    } catch (const std::runtime_error& e) {
        std::cerr << std::format("Failed to write file ({})", e.what()) << std::endl;
    }

    return 0;
}