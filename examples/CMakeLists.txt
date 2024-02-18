cmake_minimum_required(VERSION 3.5.0)
project(nbtpp_examples)

function(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} nbtpp)
endfunction()

add_example(printNBT)
add_example(reexport)
add_example(customNBT)
add_example(compressed)