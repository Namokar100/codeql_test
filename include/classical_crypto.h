#ifndef CLASSICAL_CRYPTO_H
#define CLASSICAL_CRYPTO_H

#include <string>
#include <vector>

namespace crypto {
namespace classical {

// AES encryption/decryption functions with vulnerabilities
std::vector<unsigned char> aes_encrypt(const std::string& plaintext, const std::string& key, const std::string& iv);
std::string aes_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key, const std::string& iv);

// RSA encryption/decryption functions with vulnerabilities
std::vector<unsigned char> rsa_encrypt(const std::string& plaintext, const std::string& public_key_file);
std::string rsa_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& private_key_file);

// Generate RSA key pair with specified bit size
bool generate_rsa_key_pair(const std::string& public_key_file, const std::string& private_key_file, int key_size);

// Utility functions
std::string bytes_to_hex(const std::vector<unsigned char>& data);
std::vector<unsigned char> hex_to_bytes(const std::string& hex);

} // namespace classical
} // namespace crypto

#endif // CLASSICAL_CRYPTO_H 