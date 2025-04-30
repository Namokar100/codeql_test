# Building the Crypto Test Project Without OpenSSL

This version of the project has been modified to not use OpenSSL or any external cryptographic libraries. Instead, it uses simplified implementations of crypto algorithms for demonstration purposes.

## Important Notes

1. **Educational Purpose Only**: The cryptographic implementations in this codebase are simplified and NOT secure. They are for educational and demonstration purposes only.

2. **Not for Production Use**: Do not use these implementations in any real-world applications.

3. **Simplified Algorithms**: The "AES" and "RSA" implementations are not actual AES or RSA algorithms - they are simplified approximations to demonstrate concepts while avoiding dependencies.

## Building the Project

Since we're not using OpenSSL, building is simpler:

```bash
mkdir build
cd build
cmake ..
make
```

## Crypto Implementation Details

### Classical Crypto

- **AES-like Encryption**: Replaced with a simplified XOR-based encryption
- **RSA-like Encryption**: Replaced with a simplified modular arithmetic implementation
- **Key Generation**: Uses basic math operations instead of OpenSSL crypto primitives

### Quantum-Resistant Crypto

- **Lattice-based Encryption**: Simple simulation (was already simplified even in the OpenSSL version)
- **Quantum Key Distribution**: Simulated using standard C++ random functions

### Vulnerable Crypto

All vulnerable functions are still present but implemented without OpenSSL:

- Hardcoded keys
- Weak key sizes
- Insecure random number generation
- ECB mode vulnerabilities
- Static IVs
- Timing attacks
- Memory leaks

## CodeQL Testing

This codebase should still be useful for testing CodeQL's ability to detect crypto vulnerabilities, even without using OpenSSL. The vulnerable patterns are preserved in our custom implementations. 