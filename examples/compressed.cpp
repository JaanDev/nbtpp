#include <nbtpp.hpp>
#include <functional>
#include <iostream>

using namespace nbt;

int main(int argc, char** argv) {
#ifdef nbtpp_zlib
    if (argc < 2) {
        std::cerr << "Please specify one or more input files!\n" << std::endl;
        return 1;
    }

    for (auto i = 1u; i < argc; i++) {
        auto fileName = std::string(argv[i]);

        try {
            std::println("Processing file {}", fileName);
            auto value = loadFromCompressedFile(fileName);
            saveToFile(fileName + "_2", &value);
            saveToCompressedFile(fileName + "_3", &value);
        } catch (const std::runtime_error& e) {
            std::println("Failed to process file {}: {}", fileName, e.what());
        }
    }

#else
    std::println("Compile nbtpp with zlib support!");
#endif

    return 0;
}