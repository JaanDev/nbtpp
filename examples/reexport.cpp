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

        try {
            auto newName = std::string(fileName) + "_reexported";
            std::println("Re-exporting file {} to {}", fileName, newName);
            
            auto value = loadFromFile(fileName);
            saveToFile(newName, &value);
        } catch (const std::runtime_error& e) {
            std::println("Failed to process file {}: {}", fileName, e.what());
        }
    }
    return 0;
}