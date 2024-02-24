#include <nbtpp.hpp>
#include <functional>
#include <iostream>

using namespace nbt;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Please specify one or more input files!\n" << std::endl;
        return 1;
    }

    for (auto i = 1u; i < argc; i++) {
        auto fileName = argv[i];

        auto nameForID = [](TagID id) -> std::string {
            switch (id) {
            case TagID::Byte:
                return "Byte";
            case TagID::Short:
                return "Short";
            case TagID::Int:
                return "Int";
            case TagID::Long:
                return "Long";
            case TagID::Float:
                return "Float";
            case TagID::Double:
                return "Double";
            case TagID::ByteArray:
                return "ByteArray";
            case TagID::String:
                return "String";
            case TagID::List:
                return "List";
            case TagID::Compound:
                return "Compound";
            case TagID::IntArray:
                return "IntArray";
            case TagID::LongArray:
                return "LongArray";
            default:
                return "Unknown";
            }
        };

        std::function<void(Value*, int)> prettyPrint;
        prettyPrint = [nameForID, &prettyPrint](Value* val, int depth) {
            auto valID = val->getID();
            auto name = nameForID(valID);

            // this is pretty hacky
            auto simpleValue = (SimpleValue*)val;
            auto listValue = (ListValue*)val;
            auto byteArrayValue = (ArrayValue<char>*)val;
            auto intArrayValue = (ArrayValue<int>*)val;
            auto longArrayValue = (ArrayValue<long long>*)val;
            auto compoundValue = (CompoundValue*)val;

            auto printIndent = [](int depth) { printf("%s", std::string(depth * 2, ' ').c_str()); };

            printIndent(depth);

            switch (valID) {
            case TagID::Byte: {
                auto x = std::get<char>(simpleValue->get());
                std::cout << std::format("* [{}] {}", name, (int)x) << std::endl;
            } break;
            case TagID::Short: {
                auto x = std::get<short>(simpleValue->get());
                std::cout << std::format("* [{}] {}", name, x) << std::endl;
            } break;
            case TagID::Int: {
                auto x = std::get<int>(simpleValue->get());
                std::cout << std::format("* [{}] {}", name, x) << std::endl;
            } break;
            case TagID::Long: {
                auto x = std::get<long long>(simpleValue->get());
                std::cout << std::format("* [{}] {}", name, x) << std::endl;
            } break;
            case TagID::Float: {
                auto x = std::get<float>(simpleValue->get());
                std::cout << std::format("* [{}] {}", name, x) << std::endl;
            } break;
            case TagID::Double: {
                auto x = std::get<double>(simpleValue->get());
                std::cout << std::format("* [{}] {}", name, x) << std::endl;
            } break;
            case TagID::String: {
                auto x = std::get<std::string>(simpleValue->get());
                std::cout << std::format("* [{}] {}", name, x) << std::endl;
            } break;
            case TagID::List: {
                auto vals = listValue->getItems();
                std::cout << std::format("- [{}] {} items of {}", name, vals.size(), nameForID(listValue->getItemsID()))
                          << std::endl;
                depth++;
                for (const auto x : vals) {
                    prettyPrint(x, depth);
                }
                depth--;
            } break;
            case TagID::ByteArray: {
                auto vals = byteArrayValue->getItems();
                std::cout << std::format("- [{}] {} items", name, vals.size()) << std::endl;
                printIndent(depth);
                printf("* ");
                for (auto x : vals) {
                    printf("%02X ", (int)x);
                }
                printf("\n");
            } break;
            case TagID::IntArray: {
                auto vals = intArrayValue->getItems();
                std::cout << std::format("- [{}] {} items", name, vals.size()) << std::endl;
                printIndent(depth);
                printf("* ");
                for (auto x : vals) {
                    printf("%i ", x);
                }
                printf("\n");
            } break;
            case TagID::LongArray: {
                auto vals = longArrayValue->getItems();
                std::cout << std::format("- [{}] {} items", name, vals.size()) << std::endl;
                printIndent(depth);
                printf("* ");
                for (auto x : vals) {
                    printf("%lld ", x);
                }
                printf("\n");
            } break;
            case TagID::Compound: {
                auto vals = compoundValue->getItems();
                std::cout << std::format("- [{}] {} items", name, vals.size()) << std::endl;
                depth++;
                for (const auto& [name, x] : vals) {
                    printIndent(depth);
                    std::cout << std::format("+ name: {}", name) << std::endl;
                    prettyPrint(x, depth);
                }
                depth--;
            } break;
            default: {
                std::cout << std::format("Unknown value") << std::endl;
            } break;
            }
        };

        try {
            std::cout << std::format("Printing file {}", fileName) << std::endl;

            auto compound = nbt::loadFromFile(fileName);
            prettyPrint(&compound, 0);
        } catch (const std::runtime_error& e) {
            std::cerr << std::format("Failed to process file {}: {}", fileName, e.what()) << std::endl;
        }
    }

    return 0;
}