cmake_minimum_required(VERSION 3.10)
project(crypto_test VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set OpenSSL root directory for Windows
set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")

# We need to create a simpler approach since we can't build with OpenSSL directly
# Let's implement a solution that doesn't rely on OpenSSL for building
# but still contains the vulnerable patterns that CodeQL can detect

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

# Define a preprocessor macro to indicate we're building without actual OpenSSL
add_definitions(-DSIMULATED_CRYPTO_ONLY)

# Add compile options for warnings and optimization
if(MSVC)
    target_compile_options(crypto_test PRIVATE /W4)
else()
    target_compile_options(crypto_test PRIVATE -Wall -Wextra -O3 -DNDEBUG)
endif() 