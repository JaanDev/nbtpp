#include <nbtpp.hpp>
#include <functional>
#include <iostream>

using namespace nbt;

int main() {
    auto value = CompoundValue();
    auto& items = value.getItems();

    items["someInteger"] = std::make_shared<SimpleValue>(2024);
    items["tsoi"] = std::make_shared<SimpleValue>("is alive");
    auto list = std::make_shared<ListValue>(TagID::String);
    list->appendValues({
        std::make_shared<SimpleValue>("In bloom"),
        std::make_shared<SimpleValue>("Heart-shaped box"),
        std::make_shared<SimpleValue>("About a girl"),
    });
    items["songs"] = list;
    items["some bytes"] =
        std::make_shared<ArrayValue<char>>(std::initializer_list<char> {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66});

    auto newVal = std::make_shared<CompoundValue>();
    items["another one"] = newVal;
    auto& newItems = newVal->getItems();
    newItems["a byte"] = std::make_shared<SimpleValue>((char)103);
    newItems["a short"] = std::make_shared<SimpleValue>((short)4096);
    newItems["an int"] = std::make_shared<SimpleValue>(999999);
    newItems["a float"] = std::make_shared<SimpleValue>(123.45f);
    newItems["a string"] = std::make_shared<SimpleValue>("hi");

    try {
        saveToFile("test.nbt", &value);
    } catch (const std::runtime_error& e) {
        std::cerr << std::format("Failed to write file ({})", e.what()) << std::endl;
    }

    return 0;
}