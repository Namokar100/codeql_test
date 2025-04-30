cmake_minimum_required(VERSION 3.10)
project(crypto_test VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/classical_crypto.cpp
    src/quantum_crypto.cpp
    src/vulnerable_crypto.cpp
)

# Create executable
add_executable(crypto_test ${SOURCES})

# Include directories
target_include_directories(crypto_test PRIVATE include)

# Add compile options for warnings - make it compiler-specific
if(MSVC)
    target_compile_options(crypto_test PRIVATE /W4)
else()
    target_compile_options(crypto_test PRIVATE -Wall -Wextra)
endif() 