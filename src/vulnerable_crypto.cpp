#include "vulnerable_crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <cstring>
#include <iostream>
#include <ctime>

namespace crypto {
namespace vulnerable {

// VULNERABILITY: Hardcoded cryptographic key and IV
// This is extremely dangerous as anyone who has the source code has access to the key
std::vector<unsigned char> encrypt_with_hardcoded_key(const std::string& plaintext) {
    // Hardcoded key and IV
    const std::string hardcoded_key = "ThisIsAHardcodedKey12345";
    const std::string hardcoded_iv = "HardcodedIV123456";
    
    std::vector<unsigned char> ciphertext;
    
    // Create and initialize context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return ciphertext;
    }
    
    // Initialize encryption operation with hardcoded values
    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, 
                          reinterpret_cast<const unsigned char*>(hardcoded_key.c_str()), 
                          reinterpret_cast<const unsigned char*>(hardcoded_iv.c_str())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    }
    
    // Allocate memory for ciphertext
    ciphertext.resize(plaintext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int ciphertext_len = 0;
    
    // Encrypt
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                         reinterpret_cast<const unsigned char*>(plaintext.c_str()), 
                         static_cast<int>(plaintext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<unsigned char>();
    }
    ciphertext_len = len;
    
    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<unsigned char>();
    }
    ciphertext_len += len;
    
    // Resize to actual ciphertext length
    ciphertext.resize(ciphertext_len);
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
    return ciphertext;
}

std::string decrypt_with_hardcoded_key(const std::vector<unsigned char>& ciphertext) {
    // Same hardcoded key and IV used in encryption
    const std::string hardcoded_key = "ThisIsAHardcodedKey12345";
    const std::string hardcoded_iv = "HardcodedIV123456";
    
    std::vector<unsigned char> plaintext;
    
    // Create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return "";
    }
    
    // Initialize decryption operation with hardcoded values
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, 
                          reinterpret_cast<const unsigned char*>(hardcoded_key.c_str()), 
                          reinterpret_cast<const unsigned char*>(hardcoded_iv.c_str())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    
    // Allocate memory for plaintext
    plaintext.resize(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int plaintext_len = 0;
    
    // Decrypt
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                         ciphertext.data(), 
                         static_cast<int>(ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len = len;
    
    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;
    
    // Resize to actual plaintext length
    plaintext.resize(plaintext_len);
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
    return std::string(plaintext.begin(), plaintext.end());
}

// VULNERABILITY: Uses a weak key size (512 bits)
// Modern standards require at least 2048 bits for RSA
bool generate_weak_rsa_key(const std::string& public_key_file, const std::string& private_key_file) {
    // 512-bit keys are considered extremely weak for RSA
    const int weak_key_size = 512;
    
    RSA* rsa = RSA_new();
    BIGNUM* bn = BN_new();
    
    // Set public exponent
    if (BN_set_word(bn, RSA_F4) != 1) {
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    // Generate weak key pair
    if (RSA_generate_key_ex(rsa, weak_key_size, bn, nullptr) != 1) {
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    // Save public key
    FILE* public_fp = fopen(public_key_file.c_str(), "w");
    if (!public_fp) {
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    if (PEM_write_RSA_PUBKEY(public_fp, rsa) != 1) {
        fclose(public_fp);
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    fclose(public_fp);
    
    // Save private key
    FILE* private_fp = fopen(private_key_file.c_str(), "w");
    if (!private_fp) {
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    // VULNERABILITY: Not encrypting the private key file
    if (PEM_write_RSAPrivateKey(private_fp, rsa, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        fclose(private_fp);
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    fclose(private_fp);
    BN_free(bn);
    RSA_free(rsa);
    
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
    
    // Create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return ciphertext;
    }
    
    // VULNERABILITY: Using ECB mode which is insecure for most purposes
    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, 
                          reinterpret_cast<const unsigned char*>(key.c_str()), 
                          nullptr) != 1) { // ECB doesn't use an IV
        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    }
    
    // Allocate memory for ciphertext
    ciphertext.resize(plaintext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int ciphertext_len = 0;
    
    // Encrypt
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                         reinterpret_cast<const unsigned char*>(plaintext.c_str()), 
                         static_cast<int>(plaintext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<unsigned char>();
    }
    ciphertext_len = len;
    
    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<unsigned char>();
    }
    ciphertext_len += len;
    
    // Resize to actual ciphertext length
    ciphertext.resize(ciphertext_len);
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
    return ciphertext;
}

std::string decrypt_ecb_mode(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    std::vector<unsigned char> plaintext;
    
    // Create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return "";
    }
    
    // Initialize decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, 
                          reinterpret_cast<const unsigned char*>(key.c_str()), 
                          nullptr) != 1) { // ECB doesn't use an IV
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    
    // Allocate memory for plaintext
    plaintext.resize(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int plaintext_len = 0;
    
    // Decrypt
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                         ciphertext.data(), 
                         static_cast<int>(ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len = len;
    
    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;
    
    // Resize to actual plaintext length
    plaintext.resize(plaintext_len);
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
    return std::string(plaintext.begin(), plaintext.end());
}

// VULNERABILITY: Uses a static IV (initialization vector)
std::vector<unsigned char> encrypt_with_static_iv(const std::string& plaintext, const std::string& key) {
    std::vector<unsigned char> ciphertext;
    
    // VULNERABILITY: Using a static IV makes the encryption predictable
    const std::string static_iv = "StaticIV123456789";
    
    // Create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return ciphertext;
    }
    
    // Initialize encryption operation with static IV
    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, 
                          reinterpret_cast<const unsigned char*>(key.c_str()), 
                          reinterpret_cast<const unsigned char*>(static_iv.c_str())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    }
    
    // Allocate memory for ciphertext
    ciphertext.resize(plaintext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int ciphertext_len = 0;
    
    // Encrypt
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                         reinterpret_cast<const unsigned char*>(plaintext.c_str()), 
                         static_cast<int>(plaintext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<unsigned char>();
    }
    ciphertext_len = len;
    
    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<unsigned char>();
    }
    ciphertext_len += len;
    
    // Resize to actual ciphertext length
    ciphertext.resize(ciphertext_len);
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
    return ciphertext;
}

std::string decrypt_with_static_iv(const std::vector<unsigned char>& ciphertext, const std::string& key) {
    std::vector<unsigned char> plaintext;
    
    // Same static IV used in encryption
    const std::string static_iv = "StaticIV123456789";
    
    // Create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return "";
    }
    
    // Initialize decryption operation with static IV
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, 
                          reinterpret_cast<const unsigned char*>(key.c_str()), 
                          reinterpret_cast<const unsigned char*>(static_iv.c_str())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    
    // Allocate memory for plaintext
    plaintext.resize(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int plaintext_len = 0;
    
    // Decrypt
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                         ciphertext.data(), 
                         static_cast<int>(ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len = len;
    
    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;
    
    // Resize to actual plaintext length
    plaintext.resize(plaintext_len);
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
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
    
    // Create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        // VULNERABILITY: Memory leak - not freeing the key_buffer
        return "";
    }
    
    // Initialize decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, 
                          reinterpret_cast<const unsigned char*>(key_buffer), 
                          reinterpret_cast<const unsigned char*>(iv.c_str())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // VULNERABILITY: Memory leak - not freeing the key_buffer
        return "";
    }
    
    // Allocate memory for plaintext
    plaintext.resize(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int plaintext_len = 0;
    
    // Decrypt
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                         ciphertext.data(), 
                         static_cast<int>(ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // VULNERABILITY: Memory leak - not freeing the key_buffer
        return "";
    }
    plaintext_len = len;
    
    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        // VULNERABILITY: Memory leak - not freeing the key_buffer
        return "";
    }
    plaintext_len += len;
    
    // Resize to actual plaintext length
    plaintext.resize(plaintext_len);
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
    // VULNERABILITY: Should clear and free the key_buffer, but doesn't
    // Should do: memset(key_buffer, 0, key.length());
    // Should do: delete[] key_buffer;
    
    return std::string(plaintext.begin(), plaintext.end());
}

} // namespace vulnerable
} // namespace crypto 