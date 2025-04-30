#include "vulnerable_crypto.h"
#include <cstring>
#include <iostream>
#include <ctime>
#include <random>
#include <fstream>
#include <algorithm>

namespace crypto {
namespace vulnerable {

// Helper function to convert bytes to hex string
std::string bytes_to_hex(const std::vector<unsigned char>& bytes) {
    std::string hex_string;
    char hex_digits[] = "0123456789ABCDEF";
    
    for (const auto& byte : bytes) {
        hex_string.push_back(hex_digits[(byte >> 4) & 0x0F]);
        hex_string.push_back(hex_digits[byte & 0x0F]);
    }
    
    return hex_string;
}

// VULNERABILITY: Hardcoded cryptographic key and IV
// This is extremely dangerous as anyone who has the source code has access to the key
std::vector<unsigned char> encrypt_with_hardcoded_key(const std::string& plaintext) {
    // Hardcoded key and IV - VULNERABILITY: This is a major security issue
    const unsigned char hardcoded_key[AES_BLOCK_SIZE] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    const unsigned char hardcoded_iv[AES_BLOCK_SIZE] = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
    };
    
    // Simulate AES encryption
    std::vector<unsigned char> ciphertext;
    ciphertext.resize(plaintext.size());
    
    // Simple XOR encryption for simulation
    for (size_t i = 0; i < plaintext.size(); ++i) {
        unsigned char k = hardcoded_key[i % AES_BLOCK_SIZE];
        unsigned char iv = hardcoded_iv[i % AES_BLOCK_SIZE];
        ciphertext[i] = plaintext[i] ^ k ^ iv;
    }
    
    return ciphertext;
}

std::string decrypt_with_hardcoded_key(const std::vector<unsigned char>& ciphertext) {
    // Same hardcoded key and IV used in encryption - VULNERABILITY
    const unsigned char hardcoded_key[AES_BLOCK_SIZE] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    const unsigned char hardcoded_iv[AES_BLOCK_SIZE] = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
    };
    
    std::vector<unsigned char> plaintext;
    plaintext.resize(ciphertext.size());
    
    // Simple XOR decryption (same as encryption with XOR)
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        unsigned char k = hardcoded_key[i % AES_BLOCK_SIZE];
        unsigned char iv = hardcoded_iv[i % AES_BLOCK_SIZE];
        plaintext[i] = ciphertext[i] ^ k ^ iv;
    }
    
    return std::string(plaintext.begin(), plaintext.end());
}

// VULNERABILITY: Uses a weak key size (512 bits) for RSA
// Modern standards require at least 2048 bits for RSA
bool generate_weak_rsa_key(const std::string& public_key_file, const std::string& private_key_file) {
    // VULNERABILITY: Using only 512 bits for RSA key (too weak)
    int bits = 512;
    
    // For simulation, we'll just write files with "WEAK KEY" comments
    std::ofstream public_key(public_key_file);
    if (!public_key.is_open()) {
        return false;
    }
    
    public_key << "-----BEGIN RSA PUBLIC KEY-----\n";
    public_key << "// VULNERABILITY: This key uses only " << bits << " bits\n";
    public_key << "// This is a simulation of a weak RSA key\n";
    public_key << "-----END RSA PUBLIC KEY-----\n";
    public_key.close();
    
    // VULNERABILITY: Not encrypting the private key
    std::ofstream private_key(private_key_file);
    if (!private_key.is_open()) {
        return false;
    }
    
    private_key << "-----BEGIN RSA PRIVATE KEY-----\n";
    private_key << "// VULNERABILITY: This key uses only " << bits << " bits\n";
    private_key << "// VULNERABILITY: This private key is not encrypted\n";
    private_key << "// This is a simulation of a weak RSA key\n";
    private_key << "-----END RSA PRIVATE KEY-----\n";
    private_key.close();
    
    return true;
}

// VULNERABILITY: Uses an insecure random number generator (rand instead of secure RNG)
std::vector<unsigned char> generate_insecure_random_bytes(size_t length) {
    std::vector<unsigned char> bytes(length);
    
    // VULNERABILITY: Using time() as a seed is predictable
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // VULNERABILITY: Using rand() instead of a cryptographically secure RNG
    for (size_t i = 0; i < length; ++i) {
        bytes[i] = static_cast<unsigned char>(rand() % 256);
    }
    
    return bytes;
}

// VULNERABILITY: Uses an insecure encryption mode (ECB mode which doesn't hide patterns)
std::vector<unsigned char> encrypt_ecb_mode(const std::string& plaintext, const std::string& key) {
    // Ensure the key is the right size for AES-128
    unsigned char aes_key[AES_BLOCK_SIZE];
    memset(aes_key, 0, AES_BLOCK_SIZE);
    memcpy(aes_key, key.c_str(), std::min(key.size(), size_t(AES_BLOCK_SIZE)));
    
    // Pad the plaintext to be a multiple of AES_BLOCK_SIZE
    size_t padded_size = ((plaintext.size() + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    std::vector<unsigned char> padded_plaintext(padded_size, 0);
    memcpy(padded_plaintext.data(), plaintext.c_str(), plaintext.size());
    
    // Add PKCS#7 padding
    unsigned char padding_value = padded_size - plaintext.size();
    if (padding_value > 0) {
        for (size_t i = plaintext.size(); i < padded_size; ++i) {
            padded_plaintext[i] = padding_value;
        }
    }
    
    // Prepare output buffer for ciphertext
    std::vector<unsigned char> ciphertext(padded_size);
    
    // VULNERABILITY: Use ECB mode which doesn't hide patterns in the data
    // Each block is encrypted independently
    for (size_t i = 0; i < padded_size; i += AES_BLOCK_SIZE) {
        // Simulate AES encryption for each block
        for (size_t j = 0; j < AES_BLOCK_SIZE && i + j < padded_size; ++j) {
            // Simple XOR for simulation
            ciphertext[i + j] = padded_plaintext[i + j] ^ aes_key[j];
        }
    }
    
    return ciphertext;
}

std::string decrypt_ecb_mode(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    if (ciphertext.size() % AES_BLOCK_SIZE != 0) {
        return "Invalid ciphertext length";
    }
    
    // Ensure the key is the right size for AES-128
    unsigned char aes_key[AES_BLOCK_SIZE];
    memset(aes_key, 0, AES_BLOCK_SIZE);
    memcpy(aes_key, key.c_str(), std::min(key.size(), size_t(AES_BLOCK_SIZE)));
    
    // Prepare output buffer for plaintext
    std::vector<unsigned char> plaintext(ciphertext.size());
    
    // VULNERABILITY: Use ECB mode
    // Each block is decrypted independently
    for (size_t i = 0; i < ciphertext.size(); i += AES_BLOCK_SIZE) {
        // Simulate AES decryption for each block
        for (size_t j = 0; j < AES_BLOCK_SIZE && i + j < ciphertext.size(); ++j) {
            // Simple XOR for simulation
            plaintext[i + j] = ciphertext[i + j] ^ aes_key[j];
        }
    }
    
    // Remove PKCS#7 padding
    unsigned char padding_value = plaintext.back();
    if (padding_value > 0 && padding_value <= AES_BLOCK_SIZE) {
        // Verify the padding
        bool valid_padding = true;
        for (size_t i = plaintext.size() - padding_value; i < plaintext.size(); ++i) {
            if (plaintext[i] != padding_value) {
                valid_padding = false;
                break;
            }
        }
        
        if (valid_padding) {
            plaintext.resize(plaintext.size() - padding_value);
        }
    }
    
    return std::string(plaintext.begin(), plaintext.end());
}

// VULNERABILITY: Uses a static IV (initialization vector)
std::vector<unsigned char> encrypt_with_static_iv(const std::string& plaintext, const std::string& key) {
    // VULNERABILITY: Using a static IV
    const unsigned char static_iv[AES_BLOCK_SIZE] = {
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11,
        0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99
    };
    
    // Ensure key is the right size
    unsigned char aes_key[AES_BLOCK_SIZE];
    memset(aes_key, 0, AES_BLOCK_SIZE);
    memcpy(aes_key, key.c_str(), std::min(key.size(), size_t(AES_BLOCK_SIZE)));
    
    std::vector<unsigned char> ciphertext;
    ciphertext.resize(plaintext.size());
    
    // Simple CBC mode simulation with static IV
    unsigned char prev_block[AES_BLOCK_SIZE];
    memcpy(prev_block, static_iv, AES_BLOCK_SIZE);
    
    // Process each block
    for (size_t i = 0; i < plaintext.size(); ++i) {
        // XOR with previous block (or IV for the first block)
        unsigned char temp = plaintext[i] ^ prev_block[i % AES_BLOCK_SIZE];
        
        // XOR with key (simplified encryption)
        ciphertext[i] = temp ^ aes_key[i % AES_BLOCK_SIZE];
        
        // Update prev_block for next iteration
        prev_block[i % AES_BLOCK_SIZE] = ciphertext[i];
    }
    
    return ciphertext;
}

std::string decrypt_with_static_iv(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    // VULNERABILITY: Same static IV used in encryption
    const unsigned char static_iv[AES_BLOCK_SIZE] = {
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11,
        0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99
    };
    
    // Ensure key is the right size
    unsigned char aes_key[AES_BLOCK_SIZE];
    memset(aes_key, 0, AES_BLOCK_SIZE);
    memcpy(aes_key, key.c_str(), std::min(key.size(), size_t(AES_BLOCK_SIZE)));
    
    std::vector<unsigned char> plaintext;
    plaintext.resize(ciphertext.size());
    
    // Simple CBC mode simulation with static IV
    unsigned char prev_block[AES_BLOCK_SIZE];
    memcpy(prev_block, static_iv, AES_BLOCK_SIZE);
    
    // Process each block
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        // XOR with key
        unsigned char temp = ciphertext[i] ^ aes_key[i % AES_BLOCK_SIZE];
        
        // XOR with previous ciphertext block (or IV for the first block)
        plaintext[i] = temp ^ prev_block[i % AES_BLOCK_SIZE];
        
        // Update prev_block for next iteration
        prev_block[i % AES_BLOCK_SIZE] = ciphertext[i];
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
            return false; // Return as soon as a mismatch is found - TIMING ATTACK VULNERABILITY
        }
    }
    
    return true;
}

// VULNERABILITY: Doesn't clear sensitive data from memory
std::string decrypt_and_leak_key(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    // VULNERABILITY: Copy the key to a buffer and never clear it
    char* sensitive_key_buffer = new char[key.length() + 1];
    strcpy(sensitive_key_buffer, key.c_str());
    
    // Decrypt the data (simplified for simulation)
    std::vector<unsigned char> plaintext(ciphertext.size());
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        plaintext[i] = ciphertext[i] ^ sensitive_key_buffer[i % key.length()];
    }
    
    // VULNERABILITY: Never freeing the sensitive buffer
    // delete[] sensitive_key_buffer; // This line is intentionally commented out to create a memory leak
    
    return std::string(plaintext.begin(), plaintext.end());
}

// NEW: VULNERABILITY: Uses MD5 (weak hash algorithm)
std::string md5_hash_password(const std::string& password) {
    // VULNERABILITY: Using MD5 for password hashing (weak and broken hash)
    // This is a simplified simulation of MD5 hashing
    unsigned char digest[MD5_DIGEST_LENGTH];
    
    // Simple hash simulation
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        digest[i] = 0;
        for (size_t j = 0; j < password.length(); ++j) {
            digest[i] ^= password[(i + j) % password.length()];
        }
    }
    
    // Convert the binary hash to hex string
    return bytes_to_hex(std::vector<unsigned char>(digest, digest + MD5_DIGEST_LENGTH));
}

// NEW: VULNERABILITY: Uses weak DES encryption
std::vector<unsigned char> des_encrypt(const std::string& plaintext, const std::string& key) {
    // VULNERABILITY: Using DES (broken encryption algorithm)
    // Simulate DES encryption
    
    // Prepare key for DES (only uses 8 bytes)
    unsigned char des_key[8];
    memset(des_key, 0, 8);
    memcpy(des_key, key.c_str(), std::min(key.size(), size_t(8)));
    
    // Pad the plaintext to be a multiple of 8 bytes
    size_t padded_size = ((plaintext.size() + 7) / 8) * 8;
    std::vector<unsigned char> padded_plaintext(padded_size, 0);
    memcpy(padded_plaintext.data(), plaintext.c_str(), plaintext.size());
    
    // Add PKCS#7 padding
    unsigned char padding_value = padded_size - plaintext.size();
    if (padding_value > 0) {
        for (size_t i = plaintext.size(); i < padded_size; ++i) {
            padded_plaintext[i] = padding_value;
        }
    }
    
    // Prepare output buffer for ciphertext
    std::vector<unsigned char> ciphertext(padded_size);
    
    // Simple encryption for simulation
    for (size_t i = 0; i < padded_size; ++i) {
        ciphertext[i] = padded_plaintext[i] ^ des_key[i % 8];
    }
    
    return ciphertext;
}

std::string des_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    if (ciphertext.size() % 8 != 0) {
        return "Invalid ciphertext length";
    }
    
    // VULNERABILITY: Using DES (broken encryption algorithm)
    // Simulate DES decryption
    
    // Prepare key for DES (only uses 8 bytes)
    unsigned char des_key[8];
    memset(des_key, 0, 8);
    memcpy(des_key, key.c_str(), std::min(key.size(), size_t(8)));
    
    // Prepare output buffer for plaintext
    std::vector<unsigned char> plaintext(ciphertext.size());
    
    // Simple decryption for simulation
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        plaintext[i] = ciphertext[i] ^ des_key[i % 8];
    }
    
    // Remove PKCS#7 padding
    unsigned char padding_value = plaintext.back();
    if (padding_value > 0 && padding_value <= 8) {
        // Verify the padding
        bool valid_padding = true;
        for (size_t i = plaintext.size() - padding_value; i < plaintext.size(); ++i) {
            if (plaintext[i] != padding_value) {
                valid_padding = false;
                break;
            }
        }
        
        if (valid_padding) {
            plaintext.resize(plaintext.size() - padding_value);
        }
    }
    
    return std::string(plaintext.begin(), plaintext.end());
}

// NEW: VULNERABILITY: Uses buffer overflow vulnerability
char* buffer_overflow_vulnerability(const char* input) {
    // VULNERABILITY: Using fixed buffer size without bounds checking
    char* buffer = new char[64];
    
    // VULNERABILITY: Using strcpy without checking the length of input
    strcpy(buffer, input); // Buffer overflow if input is longer than 64 bytes
    
    return buffer;
}

// NEW: VULNERABILITY: SQL injection vulnerability
std::string sql_query_builder(const std::string& user_input) {
    // VULNERABILITY: Direct concatenation of user input into SQL query
    std::string query = "SELECT * FROM users WHERE username = '" + user_input + "'";
    return query;
}

// NEW: VULNERABILITY: Uses outdated protocol (SSLv2)
bool initialize_outdated_ssl_context() {
    // VULNERABILITY: Using an outdated and insecure SSL protocol
    std::cout << "Initializing SSLv2 context (VULNERABILITY: SSLv2 is deprecated and insecure)" << std::endl;
    
    // Simulate SSLv2 context initialization
    // Return true to indicate success in the simulation
    return true;
}

} // namespace vulnerable
} // namespace crypto 