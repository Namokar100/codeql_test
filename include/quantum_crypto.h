#ifndef QUANTUM_CRYPTO_H
#define QUANTUM_CRYPTO_H

#include <string>
#include <vector>

namespace crypto {
namespace quantum {

// Simulated Quantum Key Distribution (BB84 Protocol)
std::vector<unsigned char> generate_qkd_key(size_t key_length, double error_rate = 0.0);

// Post-quantum cryptographic algorithm (simulated lattice-based)
std::vector<unsigned char> lattice_encrypt(const std::string& plaintext, const std::vector<unsigned char>& public_key);
std::string lattice_decrypt(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& private_key);

// Generate simulated lattice-based key pair
bool generate_lattice_key_pair(std::vector<unsigned char>& public_key, std::vector<unsigned char>& private_key, int security_level);

// Hybrid encryption using classical and post-quantum algorithms
std::vector<unsigned char> hybrid_encrypt(const std::string& plaintext, const std::string& rsa_public_key_file, const std::vector<unsigned char>& lattice_public_key);
std::string hybrid_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& rsa_private_key_file, const std::vector<unsigned char>& lattice_private_key);

} // namespace quantum
} // namespace crypto

#endif // QUANTUM_CRYPTO_H 