#ifndef VULNERABLE_CRYPTO_H
#define VULNERABLE_CRYPTO_H

#include <string>
#include <vector>

// Define OpenSSL-like constants and types for the simulated implementation
#define MD5_DIGEST_LENGTH 16
#define AES_BLOCK_SIZE 16
#define DES_BLOCK_SIZE 8

namespace crypto {
namespace vulnerable {

// VULNERABILITY: Uses a hardcoded key and IV
std::vector<unsigned char> encrypt_with_hardcoded_key(const std::string& plaintext);
std::string decrypt_with_hardcoded_key(const std::vector<unsigned char>& ciphertext);

// VULNERABILITY: Uses a weak key size (512 bits)
bool generate_weak_rsa_key(const std::string& public_key_file, const std::string& private_key_file);

// VULNERABILITY: Uses an insecure random number generator
std::vector<unsigned char> generate_insecure_random_bytes(size_t length);

// VULNERABILITY: Uses an insecure encryption mode (ECB mode)
std::vector<unsigned char> encrypt_ecb_mode(const std::string& plaintext, const std::string& key);
std::string decrypt_ecb_mode(const std::vector<unsigned char>& ciphertext, const std::string& key);

// VULNERABILITY: Uses a static IV (initialization vector)
std::vector<unsigned char> encrypt_with_static_iv(const std::string& plaintext, const std::string& key);
std::string decrypt_with_static_iv(const std::vector<unsigned char>& ciphertext, const std::string& key);

// VULNERABILITY: Timing attack vulnerability in password comparison
bool vulnerable_password_check(const std::string& input_password, const std::string& stored_password);

// VULNERABILITY: Doesn't clear sensitive data from memory
std::string decrypt_and_leak_key(const std::vector<unsigned char>& ciphertext, const std::string& key);

// NEW: VULNERABILITY: Uses MD5 (weak hash algorithm)
std::string md5_hash_password(const std::string& password);

// NEW: VULNERABILITY: Uses weak DES encryption
std::vector<unsigned char> des_encrypt(const std::string& plaintext, const std::string& key);
std::string des_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key);

// NEW: VULNERABILITY: Uses buffer overflow vulnerability
char* buffer_overflow_vulnerability(const char* input);

// NEW: VULNERABILITY: SQL injection vulnerability
std::string sql_query_builder(const std::string& user_input);

// NEW: VULNERABILITY: Uses outdated protocol (SSLv2)
bool initialize_outdated_ssl_context();

// Helper functions
std::string bytes_to_hex(const std::vector<unsigned char>& bytes);

} // namespace vulnerable
} // namespace crypto

#endif // VULNERABLE_CRYPTO_H 