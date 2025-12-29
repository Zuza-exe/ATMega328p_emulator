cmake_minimum_required(VERSION 3.16)

project(ATMega328p_emulator
        VERSION 0.1
        LANGUAGES CXX)

# standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# pliki źródłowe
add_executable(emulator
    src/main.cpp
    src/cpu.cpp
    src/memory.cpp
)

# katalog z nagłówkami
target_include_directories(emulator
    PRIVATE include
)

# ostrzeżenia (bardzo polecam)
target_compile_options(emulator
    PRIVATE -Wall -Wextra -Wpedantic
)
