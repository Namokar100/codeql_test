#include "vulnerable_crypto.h"
#include <cstring>
#include <iostream>
#include <ctime>
#include <random>
#include <fstream>
#include <algorithm>

namespace crypto {
namespace vulnerable {

// VULNERABILITY: Hardcoded cryptographic key and IV
// This is extremely dangerous as anyone who has the source code has access to the key
std::vector<unsigned char> encrypt_with_hardcoded_key(const std::string& plaintext) {
    // Hardcoded key and IV
    const std::string hardcoded_key = "ThisIsAHardcodedKey12345";
    const std::string hardcoded_iv = "HardcodedIV123456";
    
    std::vector<unsigned char> ciphertext;
    
    // Simple XOR-based encryption for demo (not real AES)
    ciphertext.resize(plaintext.size());
    
    for (size_t i = 0; i < plaintext.size(); ++i) {
        // Use hardcoded key and IV to encrypt (VULNERABILITY)
        unsigned char k = hardcoded_key[i % hardcoded_key.length()];
        unsigned char v = hardcoded_iv[i % hardcoded_iv.length()];
        ciphertext[i] = plaintext[i] ^ k ^ v;
    }
    
    return ciphertext;
}

std::string decrypt_with_hardcoded_key(const std::vector<unsigned char>& ciphertext) {
    // Same hardcoded key and IV used in encryption
    const std::string hardcoded_key = "ThisIsAHardcodedKey12345";
    const std::string hardcoded_iv = "HardcodedIV123456";
    
    std::vector<unsigned char> plaintext;
    plaintext.resize(ciphertext.size());
    
    // XOR decryption (same operation as encryption with XOR)
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        unsigned char k = hardcoded_key[i % hardcoded_key.length()];
        unsigned char v = hardcoded_iv[i % hardcoded_iv.length()];
        plaintext[i] = ciphertext[i] ^ k ^ v;
    }
    
    return std::string(plaintext.begin(), plaintext.end());
}

// VULNERABILITY: Uses a weak key size (512 bits)
// Modern standards require at least 2048 bits for RSA
bool generate_weak_rsa_key(const std::string& public_key_file, const std::string& private_key_file) {
    // Simplified demo RSA with very small parameters (weak)
    unsigned long p = 11;  // Small prime
    unsigned long q = 7;   // Small prime
    unsigned long n = p * q;                  // modulus
    unsigned long phi = (p - 1) * (q - 1);    // Euler's totient
    
    // Choose public exponent
    unsigned long e = 13;
    
    // Calculate private exponent
    unsigned long d = 0;
    for (unsigned long i = 1; i < phi; i++) {
        if ((i * e) % phi == 1) {
            d = i;
            break;
        }
    }
    
    // Store keys in files
    std::ofstream public_key(public_key_file);
    if (!public_key.is_open()) {
        return false;
    }
    public_key << "WEAK RSA PUBLIC KEY SIMULATION\n";
    public_key << "n=" << n << "\n";
    public_key << "e=" << e << "\n";
    public_key.close();
    
    // VULNERABILITY: Private key is not encrypted/protected
    std::ofstream private_key(private_key_file);
    if (!private_key.is_open()) {
        return false;
    }
    private_key << "WEAK RSA PRIVATE KEY SIMULATION\n";
    private_key << "n=" << n << "\n";
    private_key << "d=" << d << "\n";
    private_key.close();
    
    return true;
}

// VULNERABILITY: Uses an insecure random number generator
std::vector<unsigned char> generate_insecure_random_bytes(size_t length) {
    std::vector<unsigned char> bytes(length);
    
    // VULNERABILITY: Using time() as a seed is predictable
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // VULNERABILITY: rand() is not cryptographically secure
    for (size_t i = 0; i < length; ++i) {
        bytes[i] = static_cast<unsigned char>(rand() % 256);
    }
    
    return bytes;
}

// VULNERABILITY: Uses ECB mode which doesn't hide patterns in the plaintext
std::vector<unsigned char> encrypt_ecb_mode(const std::string& plaintext, const std::string& key) {
    std::vector<unsigned char> ciphertext;
    
    // Simplified ECB mode: each block is encrypted independently
    // This implementation is a demonstration of ECB behavior, not real AES
    
    // Block size for demo (16 bytes)
    const size_t block_size = 16;
    
    // Pad plaintext to a multiple of block size
    std::string padded_plaintext = plaintext;
    size_t padding_needed = (block_size - (plaintext.size() % block_size)) % block_size;
    padded_plaintext.append(padding_needed, static_cast<char>(padding_needed));
    
    // Encrypt each block independently (ECB mode vulnerability)
    ciphertext.resize(padded_plaintext.size());
    
    for (size_t i = 0; i < padded_plaintext.size(); i += block_size) {
        // Simple encryption for each block (same operation for identical blocks - ECB weakness)
        for (size_t j = 0; j < block_size && i + j < padded_plaintext.size(); ++j) {
            unsigned char k = key[j % key.size()];
            ciphertext[i + j] = padded_plaintext[i + j] ^ k;
        }
    }
    
    return ciphertext;
}

std::string decrypt_ecb_mode(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    // Block size for demo (16 bytes)
    const size_t block_size = 16;
    
    if (ciphertext.size() % block_size != 0) {
        return "Invalid ciphertext length";
    }
    
    std::vector<unsigned char> plaintext(ciphertext.size());
    
    // Decrypt each block independently
    for (size_t i = 0; i < ciphertext.size(); i += block_size) {
        for (size_t j = 0; j < block_size && i + j < ciphertext.size(); ++j) {
            unsigned char k = key[j % key.size()];
            plaintext[i + j] = ciphertext[i + j] ^ k;
        }
    }
    
    // Handle padding
    size_t padding_size = plaintext.back();
    if (padding_size > block_size) {
        return "Invalid padding";
    }
    
    // Remove padding
    plaintext.resize(plaintext.size() - padding_size);
    
    return std::string(plaintext.begin(), plaintext.end());
}

// VULNERABILITY: Uses a static IV (initialization vector)
std::vector<unsigned char> encrypt_with_static_iv(const std::string& plaintext, const std::string& key) {
    std::vector<unsigned char> ciphertext;
    
    // VULNERABILITY: Using a static IV makes the encryption predictable
    const std::string static_iv = "StaticIV123456789";
    
    ciphertext.resize(plaintext.size());
    
    // Simple simulation of CBC mode with static IV (vulnerability)
    unsigned char previous_block[16] = {0};
    
    // Copy static IV to previous_block
    for (size_t i = 0; i < 16 && i < static_iv.length(); ++i) {
        previous_block[i] = static_iv[i];
    }
    
    // Encrypt each byte with key and chaining
    for (size_t i = 0; i < plaintext.size(); ++i) {
        // XOR with previous block (CBC mode)
        unsigned char temp = plaintext[i] ^ previous_block[i % 16];
        
        // XOR with key (simplified encryption)
        ciphertext[i] = temp ^ key[i % key.length()];
        
        // Update previous block for next iteration
        previous_block[i % 16] = ciphertext[i];
    }
    
    return ciphertext;
}

std::string decrypt_with_static_iv(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    std::vector<unsigned char> plaintext;
    
    // Same static IV used in encryption
    const std::string static_iv = "StaticIV123456789";
    
    plaintext.resize(ciphertext.size());
    
    // Simple simulation of CBC mode decryption with static IV
    unsigned char previous_block[16] = {0};
    
    // Copy static IV to previous_block
    for (size_t i = 0; i < 16 && i < static_iv.length(); ++i) {
        previous_block[i] = static_iv[i];
    }
    
    // Decrypt each byte
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        // Decrypt with key
        unsigned char temp = ciphertext[i] ^ key[i % key.length()];
        
        // XOR with previous block to get plaintext
        plaintext[i] = temp ^ previous_block[i % 16];
        
        // Store current ciphertext byte for next iteration
        previous_block[i % 16] = ciphertext[i];
    }
    
    return std::string(plaintext.begin(), plaintext.end());
}

// VULNERABILITY: Timing attack vulnerability in password comparison
bool vulnerable_password_check(const std::string& input_password, const std::string& stored_password) {
    // VULNERABILITY: Early return creates a timing side-channel
    if (input_password.length() != stored_password.length()) {
        return false;
    }
    
    // VULNERABILITY: Character-by-character comparison with early return creates timing side-channel
    for (size_t i = 0; i < input_password.length(); ++i) {
        if (input_password[i] != stored_password[i]) {
            return false; // Return as soon as a mismatch is found
        }
    }
    
    return true;
}

// VULNERABILITY: Doesn't clear sensitive data from memory
std::string decrypt_and_leak_key(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    std::vector<unsigned char> plaintext;
    
    // Create a buffer for the key that will not be cleared
    char* key_buffer = new char[key.length() + 1];
    strcpy(key_buffer, key.c_str());
    
    // Use the key for decryption
    std::string iv = "InitializationVec";
    
    plaintext.resize(ciphertext.size());
    
    // Simple XOR decryption with key and IV
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        unsigned char k = key_buffer[i % key.length()];
        unsigned char v = iv[i % iv.length()];
        plaintext[i] = ciphertext[i] ^ k ^ v;
    }
    
    // VULNERABILITY: Should clear and free the key_buffer, but doesn't
    // Should do: memset(key_buffer, 0, key.length());
    // Should do: delete[] key_buffer;
    
    return std::string(plaintext.begin(), plaintext.end());
}

} // namespace vulnerable
} // namespace crypto 