# nbtpp
A C++ library to work with Minecraft's NBT files. It uses modern C++ features like std containers, templates and more.

## What?
Minecraft uses its own JSON analog for tree data storage which is called [NBT](https://minecraft.fandom.com/wiki/NBT_format). It is used in many places of the game and is present in binary and text forms. This library currently only supports the binary NBT format, however the text format may also be supported in future.

## Why?
This library is one of the few I could find at all, so i guess it could be pretty useful for scripts or other apps.

## How?
### Installing
This lib supports cmake which is the preferred way of using it.
1. `git submodule add https://github.com/JaanDev/nbtpp.git`
2. In your `CMakeLists.txt`:
   ```cmake
   # You can change the default options here (see CMake options)
   add_subdirectory(nbtpp)
   ...
   target_link_libraries(<your project name> PRIVATE nbtpp)
   ```
3. In your code:
   ```cpp
   #include <nbtpp.hpp>

   // check examples dir in the repo for some examples!
   ```

### CMake options
|Name|Desc|Default|
|---|---|---|
|NBTPP_EXAMPLES|Build nbtpp examples|OFF
|NBTPP_ZLIB|Build nbtpp with zlib support for compressed files|ON

### Usage
See the [examples](examples) dir at the repo for some comprehensive examples.

## Contributing
Feel free to open an issue or send a pull request. They are always welcome =)

## Contacts
`jaan2897` on Discord.