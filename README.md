# Cryptography Test Project for CodeQL

This project is specifically designed to test CodeQL's ability to detect cryptographic vulnerabilities in C++ code. It includes both secure and **intentionally insecure** cryptographic implementations for testing purposes.

## Overview

The project demonstrates:

1. Classical cryptography (AES, RSA)
2. Simulated quantum-resistant cryptography (lattice-based)
3. Hybrid approaches combining classical and quantum methods
4. **Deliberately vulnerable implementations** with common cryptographic mistakes

## Purpose

This codebase is designed to be scanned by CodeQL to evaluate how effectively it can identify cryptographic vulnerabilities in C++ code. The vulnerabilities included are typical issues that security scanning tools should be able to detect.

## Intentional Vulnerabilities

The codebase contains the following **intentional** vulnerabilities:

- Hardcoded cryptographic keys and IVs
- Weak key sizes (RSA 512 bits)
- Insecure random number generation
- Use of ECB mode for encryption
- Static initialization vectors
- Timing attack vulnerabilities
- Memory leaks with sensitive data
- Unencrypted private key storage
- Weak cipher choices

## Building the Project

The project uses CMake for building:

```bash
mkdir build
cd build
cmake ..
make
```

## Running the Demo

After building, run the demo program:

```bash
./crypto_test
```

The program will demonstrate various cryptographic functions including the vulnerable ones.

## Warning

**DO NOT USE THIS CODE IN PRODUCTION!**

This codebase contains **intentionally vulnerable** code for testing security analysis tools. It should never be used in real-world applications.

## CodeQL Testing

When running CodeQL analysis on this codebase, it should identify many of the intentional cryptographic vulnerabilities. This can be used to:

1. Test the effectiveness of CodeQL security rules
2. Develop custom rules for specific cryptographic issues
3. Understand how cryptographic vulnerabilities appear in code

## License

This code is provided for educational and testing purposes only under the MIT license. 